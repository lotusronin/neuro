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
    TokenType::tushort,
    TokenType::tshort,
    TokenType::tuint,
    TokenType::tint,
    TokenType::tulongint,
    TokenType::tlongint,
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
    TokenType::tstruct,
    TokenType::cast,
    TokenType::ssizeof,
    TokenType::tunion,
    TokenType::vnull
};

const char* keyword_array[32] = {
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
    "u16",
    "s16",
    "u32",
    "s32",
    "u64",
    "s64",
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
    "struct",
    "cast",
    "sizeof",
    "union",
    "null"
};
unsigned int num_keywords = sizeof(keyword_array)/sizeof(const char*);

LexerTarget::LexerTarget(std::string& name, bool debug) {
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

bool LexerTarget::isDebug() const {
    return debug_out;
}

std::string LexerTarget::targetName() const {
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
        
        while(content[f_idx] == 12 || content[f_idx] == 15 || content[f_idx] == '\n') {
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
            } else {
                ++colNum;
                ++f_idx;
            }
        } else if(content[f_idx] == '*') {
            if(content[f_idx+1] == '/') {
                --comment_depth;
                colNum += 2;
                f_idx += 2;
                //std::cout << "Block comment ends. Depth " << comment_depth << "\n";
            } else {
                ++colNum;
                ++f_idx;
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
        //if(tokenizedFile.size() < 15) std::cout << tok.token << '\n';
        tokenizedFile.push_back(tok);
        tok = lex_internal();
    }
    tokenizedFile.push_back(tok);
    tokenizedFile.push_back(EOFTOKEN);
    currentIdx = 0;
}

void printLn(char* s) {
    std::cout << "Ln is...\n";
    if(s[0] == '\0')
        return;

    while(s[0] != '\n') {
        std::cout << s[0];
        s++;
    }
    std::cout << "\n";
}

std::regex stripped("\"([^\"]*)\"");
std::string newlines = "\\n";
std::string newlinec = "\n";

Token LexerTarget::lex_internal() {

    TokenType longest_match_type;
    int longest_match = 0;
    const char* c_str_tok = nullptr;

    /*
     * Match other tokens
     */
    
    char* remaining; // = ln+colNum;
    longest_match = 1; //default to 1
TOP:
    remaining = ln+colNum;
    switch(remaining[0]) {
        case ' ':
        case '\t':
            colNum++;
            f_idx++;
            goto TOP;
            break;
        case '\r':
        case '\n':
            lineNum++;
            colNum = 0;
            f_idx++;
            ln = content+f_idx;
            goto TOP;
            break;
        case '\0':
            return EOFTOKEN;
            break;
        case '(':
            longest_match_type = TokenType::lparen;
            c_str_tok = "(";
            break;
        case ')':
            longest_match_type = TokenType::rparen;
            c_str_tok = ")";
            break;
        case '.':
            longest_match_type = TokenType::dot;
            c_str_tok = ".";
            break;
        case ',':
            longest_match_type = TokenType::comma;
            c_str_tok = ",";
            break;
        case '{':
            longest_match_type = TokenType::lbrace;
            c_str_tok = "{";
            break;
        case '}':
            longest_match_type = TokenType::rbrace;
            c_str_tok = "}";
            break;
        case '[':
            longest_match_type = TokenType::lsqrbrace;
            c_str_tok = "[";
            break;
        case ']':
            longest_match_type = TokenType::rsqrbrace;
            c_str_tok = "]";
            break;
        case ':':
            {
                longest_match_type = TokenType::colon;
                c_str_tok = ":";
                if(remaining[1] == ':') {
                    longest_match++;
                    longest_match_type = TokenType::dblcolon;
                    c_str_tok = "::";
                }
            }
            break;
        case ';':
            longest_match_type = TokenType::semicolon;
            c_str_tok = ";";
            break;
        case '@':
            longest_match_type = TokenType::dereference;
            c_str_tok = "@";
            break;
        case '~':
            longest_match_type = TokenType::tilda;
            c_str_tok = "~";
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
                char* cstr = (char*)malloc(longest_match+1);
                std::strncpy(cstr,ln+colNum,longest_match);
                cstr[longest_match] = '\0';
                c_str_tok = cstr;
                
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
                std::string token = std::string(ln+colNum,longest_match);
                std::smatch m;
                if(std::regex_match(token, m, stripped)) {
                    token = m[1].str();
                    size_t index;
                    while( (index = token.find(newlines)) != std::string::npos) {
                        token.replace(index, newlines.length(), newlinec);
                        index += newlinec.length();
                    }
                }
                char* cstr = (char*)malloc(token.size()+1);
                std::strcpy(cstr,token.c_str());
                c_str_tok = cstr;
            }
            break;
        case '\'':
            {
                int match_len = 1;
                bool escaped = false;
                char c = content[f_idx+match_len];
                if(c == 15 || c == 12) {
                    std::cout << "Error, malformed character literal.\n";
                    return EOFTOKEN;
                }
                match_len++;
                if(c == '\\') {
                    escaped = true;
                }
                c = content[f_idx+match_len];
                if(escaped) {
                    switch(c) {
                        case 'n':
                        case 'a':
                        case '0':
                        case '\\':
                        case '\'':
                        case '\"':
                        case 'r':
                            match_len++;
                            break;
                        default:
                            std::cout << "Error, malformed character literal. Line " << lineNum << " Col " << colNum << '\n';
                            return EOFTOKEN;
                            break;
                        }
                    c = content[f_idx+match_len];
                }
                if(c != '\'') {
                    std::cout << "Error, expected a ' in character literal. Line " << lineNum << " Col " << colNum << '\n';
                    return EOFTOKEN;
                }
                match_len++;
                longest_match = match_len;
                longest_match_type = TokenType::charlit;
                char* cstr = (char*)malloc(longest_match+1);
                std::strncpy(cstr,ln+colNum,longest_match);
                cstr[longest_match] = '\0';
                c_str_tok = cstr;
            }
            break;
        case '+':
            {
                if(remaining[1] == '+') {
                    longest_match++;
                    longest_match_type = TokenType::increment;
                    c_str_tok = "++";
                } else if(remaining[1] == '=') {
                    longest_match++;
                    longest_match_type = TokenType::addassign;
                    c_str_tok = "+=";
                } else {
                    longest_match_type = TokenType::plus;
                    c_str_tok = "+";
                }
            }
            break;
        case '-':
            {
                if(remaining[1] == '-') {
                    longest_match++;
                    longest_match_type = TokenType::increment;
                    c_str_tok = "--";
                } else if(remaining[1] == '=') {
                    longest_match++;
                    longest_match_type = TokenType::subassign;
                    c_str_tok = "-=";
                } else {
                    longest_match_type = TokenType::minus;
                    c_str_tok = "-";
                }
            }
            break;
        case '/':
            {
                /*
                 * Check for block comments
                 */
                if(remaining[1] == '*') {
                    ++comment_depth;
                    colNum += 2;
                    f_idx += 2;
                    lexcomment();
                    return lex_internal();
                }

                /*
                 * Check for line comments
                 */
                else if(remaining[1] == '/') {
                    int commentIdx = 2;
                    colNum += 2;
                    f_idx += 2;
                    while(remaining[commentIdx] != '\n' && remaining[commentIdx] != '\r') {
                        if(content[f_idx] == '\0') {
                            return EOFTOKEN;
                        }
                        colNum++;
                        f_idx++;
                        commentIdx++;
                    }
                    f_idx++;
                    colNum = 0;
                    lineNum++;
                    ln = content+f_idx;
                    //TODO(marcus): can probable replace this with goto
                    return lex_internal();
                }
                longest_match_type = TokenType::fslash;
                c_str_tok = "/";
                if(remaining[1] == '=') {
                    longest_match++;
                    longest_match_type = TokenType::divassign;
                    c_str_tok = "/=";
                }
            }
            break;
        case '*':
            {
                longest_match_type = TokenType::star;
                c_str_tok = "*";
                if(remaining[1] == '=') {
                    longest_match++;
                    longest_match_type = TokenType::mulassign;
                    c_str_tok = "*=";
                }
            }
            break;
        case '^':
            {
                longest_match_type = TokenType::carrot;
                c_str_tok = "^";
            }
            break;
        case '%':
            {
                longest_match_type = TokenType::mod;
                c_str_tok = "%";
            }
            break;
        case '#':
            {
                longest_match_type = TokenType::hashtag;
                c_str_tok = "#";
            }
            break;
        case '=':
            {
                longest_match_type = TokenType::assignment;
                c_str_tok = "=";
                if(remaining[1] == '=') {
                    longest_match = 2;
                    longest_match_type = TokenType::equality;
                    c_str_tok = "==";
                }
            }
            break;
        case '<':
            {
                longest_match_type = TokenType::lessthan;
                c_str_tok = "<";
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::ltequal;
                    longest_match = 2;
                    c_str_tok = "<=";
                }
            }
            break;
        case '>':
            {
                longest_match_type = TokenType::greaterthan;
                c_str_tok = ">";
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::gtequal;
                    longest_match = 2;
                    c_str_tok = ">=";
                }
            }
            break;
        case '!':
            {
                longest_match_type = TokenType::exclaim;
                c_str_tok = "!";
                if(remaining[1] == '=') {
                    longest_match_type = TokenType::nequality;
                    longest_match = 2;
                    c_str_tok = "!=";
                }
            }
            break;
        case '|':
            {
                longest_match_type = TokenType::bar;
                c_str_tok = "|";
                if(remaining[1] == '|') {
                    longest_match_type = TokenType::dblbar;
                    longest_match = 2;
                    c_str_tok = "||";
                }
            }
        case '&':
            {
                longest_match_type = TokenType::ampersand;
                c_str_tok = "&";
                if(remaining[1] == '&') {
                    longest_match_type = TokenType::dblampersand;
                    longest_match = 2;
                    c_str_tok = "&&";
                }
            }
            break;
        default:
            {
                //TODO(marcus): error checks
                int len_t = 0;
                char current=remaining[len_t];
                while(isalnum(current) || current == '_')
                {
                    len_t++;
                    current=remaining[len_t];
                }
                longest_match = len_t;
                longest_match_type = TokenType::id;
                char buff[longest_match+1];
                std::strncpy(buff,ln+colNum,longest_match);
                buff[longest_match] = '\0';
                for(unsigned int j = 0; j < num_keywords; j++) {
                   if(std::strcmp(buff, keyword_array[j]) == 0) {
                       longest_match = strlen(keyword_array[j]);
                       longest_match_type = keyword_type[j];
                       c_str_tok = keyword_array[j];
                       break;
                   }
                }
                if(longest_match_type == TokenType::id) {
                    char* cstr = (char*)malloc(longest_match+1);
                    std::strncpy(cstr,ln+colNum,longest_match);
                    cstr[longest_match] = '\0';
                    c_str_tok = cstr;
                }
            }
            break;
    }
    
    f_idx += longest_match;

    Token ret = {
		longest_match_type, //type
		colNum, //col
		lineNum, //line
		c_str_tok //token
	};
    colNum += longest_match;

    DEBUGLEX(std::cout << "token: " << std::string(ln+colNum-longest_match,longest_match) << "\n\n";)
    //std::cout << colNum << ' ' << f_idx << '\t' << content[f_idx] << '\n';
    return ret;

}

Token LexerTarget::peek() const {
    return tokenizedFile[currentIdx];
}

Token LexerTarget::peekNext() const {
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
