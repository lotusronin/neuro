#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <utility>
#include <ctype.h>
#include <chrono>
#include <cstring>
#include "lexer.h"
#include "tokens.h"

#define DEBUGLEX(a) if(debug_out){a}

Token EOFTOKEN {
	TokenType::eof, //type
	0, //col
	0, //line
	"EOF" //token
};

TokenType keyword_type[] = {
    TokenType::fn,
    TokenType::foreign,
    TokenType::import,
    TokenType::tchar,
    TokenType::tint,
    TokenType::tbool,
    TokenType::tfloat,
    TokenType::tdouble,
    TokenType::tvoid,
    TokenType::tuchar,
    TokenType::tchar,
    TokenType::tuint,
    TokenType::tint,
    TokenType::tfloat,
    TokenType::tdouble,
    TokenType::sif,
    TokenType::sfor,
    TokenType::swhile,
    TokenType::sdefer,
    TokenType::sreturn,
    TokenType::selse,
    TokenType::sbreak,
    TokenType::scontinue,
    TokenType::tstruct
};

const char* keyword_array[] = {
    "fn",
    "extern",
    "import",
    "char",
    "int",
    "bool",
    "float",
    "double",
    "void",
    "u8",
    "s8",
    "u32",
    "s32",
    "f32",
    "f64",
    "if",
    "for",
    "while",
    "defer",
    "return",
    "else",
    "break",
    "continue",
    "struct"
};
unsigned int num_keywords = sizeof(keyword_array)/sizeof(const char*);


LexerTarget::LexerTarget(std::string name, bool debug) {
    filename = name;
    
    //auto start = std::chrono::steady_clock::now();
    //content = read_file(name);
    content= read_file(name);
    //auto finish = std::chrono::steady_clock::now();
    //auto diff = finish - start;
    //std::cout << "File Read Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms\n";
    ln = content;

    lineNum = 0;
    colNum = 0;
    sub_begin = sub_len = 0;
    comment_depth = 0;
    f_idx = 0;
    debug_out = debug;
}

LexerTarget::~LexerTarget() {
}

bool LexerTarget::isDebug() {
    return debug_out;
}

std::string LexerTarget::targetName() {
    return filename;
}

void LexerTarget::lexcomment() {
    
    while(comment_depth > 0) {
        //std::cout << "Parsing comment block\n";
        
        if(content[f_idx] == '\0') {
            DEBUGLEX(std::cout << "Comment reaches the end of the file\n";)
            //we've reached end of file, stop
            comment_depth = 0;
            return;
        }
        
        while(content[f_idx] == 12 || content[f_idx] == 15) {
            //std::cout << ln << '\n';
            lineNum++;
            colNum = 0;
            f_idx++;
            ln = content+f_idx;
            if(content[f_idx] == '\0') {
                DEBUGLEX(std::cout << "Comment reaches the end of the file\n";)
                comment_depth = 0;
                return;
            }
        }
    
        if(content[f_idx] == '/') {
            if(content[f_idx+1] == '*') {
                ++comment_depth;
                colNum += 2;
                f_idx += 2;
                //std::cout << "Block comment begins. Depth " << comment_depth << "\n";
            }
        } else if(content[f_idx] == '*') {
            if(content[f_idx] == '/') {
                --comment_depth;
                colNum += 2;
                f_idx += 2;
                //std::cout << "Block comment ends. Depth " << comment_depth << "\n";
            }
        } else {
            ++colNum;
            ++f_idx;
        }
    }
}

Token LexerTarget::lex() {
    if(tokenizedFile.size()-2 > currentIdx) {
        Token ret = tokenizedFile[currentIdx+1];
        currentIdx++;
        return ret;
    }
    return tokenizedFile[currentIdx];
}

void LexerTarget::lexFile() {
    Token tok = lex_internal();
    //tok = lex_internal();
    tokenizedFile.reserve(200);
    while(tok.type != TokenType::eof) {
        //if(tokenizedFile.size() < 10) std::cout << tok.token << '\n';
        tokenizedFile.push_back(tok);
        tok = lex_internal();
    }
    tokenizedFile.push_back(tok);
    tokenizedFile.push_back(EOFTOKEN);
    currentIdx = 0;

    /*
    std::cout << "Lexer " << sizeof(LexerTarget) << '\n';
    std::cout << "Token " << sizeof(Token) << '\n';
    std::cout << "String " << sizeof(std::string) << '\n';
    std::cout << "const char* " << sizeof(const char*) << '\n';
    std::cout << "int " << sizeof(int) << '\n';
    std::cout << "TokenType " << sizeof(TokenType) << '\n';
    /**/
}

Token LexerTarget::lex_internal() {
    std::string token = "";

    if(content[f_idx] == '\0') {
        return EOFTOKEN;
    }
    
    //char* ln = content+f_idx;
    //15 == CR, 12 == LF
    while(content[f_idx] == 15 || content[f_idx] == 12 || content[f_idx] == '\n') {
        lineNum++;
        colNum = 0;
        f_idx++;
        if(content[f_idx] == '\0') {
            return EOFTOKEN;
        }
        ln = content+f_idx;
    }


    //TODO(marcus): isspace may be true for \r and \f, so count would be off
    while(isspace(ln[colNum])) {
        colNum++;
        f_idx++;

        //std::cout << "there is a space!!!\n";
        while(content[f_idx] == 12 || content[f_idx] == 15) {
            lineNum++;
            f_idx++;
            colNum = 0;
            if(content[f_idx] == '\0') {
                return EOFTOKEN;
            }
            ln = content + f_idx;
        }
    }
            

    TokenType longest_match_type;
    int longest_match = 0;

    //std::string t = ln.substr(colNum);
    char* t = ln+colNum;
    //char* t = content+f_idx;

    /*
     * check for block comments
     */
    if(t[0] == '/') {
        if(t[1] == '*') {
            ++comment_depth;
            colNum += 2;
            f_idx += 2;
            lexcomment();
            return lex_internal();
        }
        //std::cout << "colNum = " << colNum << '\n';
        //TODO(marcus): make this work with content/ln beign char*
        DEBUGLEX(
        std::cout << ln << '\n';
        if(colNum == 0) {
            std::cout << "^\n";
        } else {
            for(unsigned int c = 0; c < colNum-1; c++) {
                std::cout << '-';
            }
            std::cout << "-^\n";
        }
        )


        /*
         * Check for line comments
         */
        if(t[1] == '/') {
            colNum += 2;
            f_idx += 2;
            while(t[colNum] != 12 && t[colNum] != 15) {
                if(content[f_idx] == '\0') {
                    return EOFTOKEN;
                }
                colNum++;
                f_idx++;
            }
            f_idx++;
            colNum = 0;
            lineNum++;
            ln = content+f_idx;
            return lex_internal();
        }
    }

    /*
     * Match other tokens
     */
    
    char* remaining = ln+colNum;
    //std::string remaining = ln.substr(colNum);
    longest_match = 1; //default to 1
    switch(remaining[0]) {
        case '\0':
            return EOFTOKEN;
            break;
        case '(':
            longest_match_type = TokenType::lparen;
            break;
        case ')':
            longest_match_type = TokenType::rparen;
            break;
        case '.':
            longest_match_type = TokenType::dot;
            break;
        case ',':
            longest_match_type = TokenType::comma;
            break;
        case '{':
            longest_match_type = TokenType::lbrace;
            break;
        case '}':
            longest_match_type = TokenType::rbrace;
            break;
        case ':':
            longest_match_type = TokenType::colon;
            break;
        case ';':
            longest_match_type = TokenType::semicolon;
            break;
        case '@':
            longest_match_type = TokenType::dereference;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
                //TODO(marcus): throw lexer error if input is bad number is malformed
                int len_lit = 0;
                longest_match_type = TokenType::intlit;
                char current;
                do {
                    current = remaining[len_lit];
                    len_lit++;
                } while(current >= '0' && current <= '9');
                
                if(current == '.') {
                    //parse floating point
                    do {
                        current = remaining[len_lit];
                        len_lit++;
                    } while(current >= '0' && current <= '9');
                    len_lit--;
                    longest_match_type = TokenType::floatlit;
                    longest_match = len_lit;
                } else {
                    len_lit--;
                    longest_match = len_lit;
                } 
                
            }
            break;
        case '"':
            {
                //TODO(marcus): handle errors/end of file
                int match_len = 1;
                bool matched = false;
                while(!matched) {
                    char c = content[f_idx+match_len];
                    if(c == 15 || c == 12) {
                        //handle multiline strings
                        lineNum++;
                        colNum = 0;
                    } if(c == '"' && content[f_idx+match_len-1] != '\\') {
                        matched = true;
                    }
                    match_len++;
                }
                longest_match = match_len;
                longest_match_type = TokenType::strlit;
            }
            break;
        case '+':
            {
                longest_match_type = TokenType::plus;
            }
            break;
        case '-':
            {
                longest_match_type = TokenType::minus;
            }
            break;
        case '/':
            {
                longest_match_type = TokenType::fslash;
            }
            break;
        case '*':
            {
                longest_match_type = TokenType::star;
            }
            break;
        case '^':
            {
                longest_match_type = TokenType::carrot;
            }
            break;
        case '%':
            {
                longest_match_type = TokenType::carrot;
            }
        case '=':
            {
                longest_match_type = TokenType::assignment;
                if(remaining[1] == '=') {
                    longest_match = 2;
                    longest_match_type = TokenType::equality;
                }
            }
            break;
        case '<':
            {
                longest_match_type = TokenType::lessthan;
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::ltequal;
                    longest_match = 2;
                }
            }
            break;
        case '>':
            {
                longest_match_type = TokenType::greaterthan;
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::gtequal;
                    longest_match = 2;
                }
            }
            break;
        case '!':
            {
                longest_match_type = TokenType::exclaim;
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::nequality;
                    longest_match = 2;
                }
            }
            break;
        case '|':
            {
                longest_match_type = TokenType::bar;
                if(remaining[1] == '|') {
                    longest_match_type = TokenType::dblbar;
                    longest_match = 2;
                }
            }
        case '&':
            {
                longest_match_type = TokenType::ampersand;
                if(remaining[1] == '&') {
                    longest_match_type = TokenType::dblampersand;
                    longest_match = 2;
                }
            }
            break;
        default:
            {
                //TODO(marcus): error checks
                int len_t = 0;
                char current;
                do {
                    current=remaining[len_t];
                    len_t++;
                }while(isalnum(current) || current == '_');
                len_t--;
                longest_match = len_t;
                longest_match_type = TokenType::id;
                std::string matched_string(ln+colNum,longest_match);
                for(unsigned int j = 0; j < num_keywords; j++) {
                   if(matched_string == keyword_array[j]) {
                       longest_match = strlen(keyword_array[j]);
                       longest_match_type = keyword_type[j];
                       break;
                   }
                }
            }
            break;
    }
    
    f_idx += longest_match;
    
    
    token = std::string(ln+colNum,longest_match);
    if(longest_match_type == TokenType::strlit) {
        std::regex stripped("\"([^\"]*)\"");
        std::smatch m;
        if(std::regex_match(token, m, stripped)) {
            token = m[1].str();
            size_t index;
            std::string newlines = "\\n";
            std::string newlinec = "\n";
            while( (index = token.find_first_of(newlines)) != std::string::npos) {
                token.replace(index, newlines.length(), newlinec);
                index += newlinec.length();
            }
        }
    }
    char* c_str_tok = (char*)malloc(token.size()+1);
    std::strcpy(c_str_tok,token.c_str());
    Token ret = {
		longest_match_type, //type
		colNum, //col
		lineNum, //line
		c_str_tok //token
	};
    colNum += longest_match;

    DEBUGLEX(std::cout << "token: " << token << "\n\n";)
    //std::cout << colNum << ' ' << f_idx << '\t' << content[f_idx] << '\n';
    return ret;

}

Token LexerTarget::peek() {
    return tokenizedFile[currentIdx];
}

Token LexerTarget::peekNext() {
    if(tokenizedFile.size()-2 > currentIdx) {
        return tokenizedFile[currentIdx+1];
    }
    return tokenizedFile[currentIdx];
}

char* read_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    in.seekg(0,std::ios::end);
    int size = in.tellg();
    char* content = (char*) malloc(size+1);
    in.seekg(0,std::ios::beg);
    in.read(content,size);
    content[size] = '\0';
    //std::cout << "File size was... " << size << '\n';
    return content;
}
