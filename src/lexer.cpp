#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <utility>
#include <ctype.h>
#include "lexer.h"
#include "tokens.h"

#define DEBUGLEX(a) if(debug_out){a}

Token EOFTOKEN {
	TokenType::eof, //type
	0, //col
	0, //line
	"EOF" //token
};

LexerTarget::LexerTarget(std::string name, bool debug) {
    filename = name;
    content = read_file(name);
    lineNum = 0;
    colNum = 0;
    sub_begin = sub_len = 0;
    comment_depth = 0;
    debug_out = debug;
    currentTok = EOFTOKEN;
    nextTok = EOFTOKEN;
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
        std::smatch match_blk_comment_open;
        std::smatch match_blk_comment_close;
        
        if(lineNum >= content.size()) {
            DEBUGLEX(std::cout << "Comment reaches the end of the file\n";)
            //we've reached end of file, stop
            comment_depth = 0;
            return;
        }
    
        std::string ln = content.at(lineNum);
        
        while(colNum >= ln.size()) {
            //std::cout << ln << '\n';
            lineNum++;
            colNum = 0;
            if(lineNum >= content.size()) {
                DEBUGLEX(std::cout << "Comment reaches the end of the file\n";)
                comment_depth = 0;
                return;
            }
            ln = content.at(lineNum);
        }
    
        std::string t = ln.substr(colNum);

        if(std::regex_search(t,match_blk_comment_open,std::regex("^/\\*"))) {
            for(unsigned int i = 0; i < match_blk_comment_open.size(); i++) {
                if(match_blk_comment_open.position(i) != 0) continue;
                ++comment_depth;
                colNum += 2;
                //std::cout << "Block comment begins. Depth " << comment_depth << "\n";
            }
        } else if(std::regex_search(t,match_blk_comment_close,std::regex("^\\*/"))) {
            for(unsigned int i = 0; i < match_blk_comment_close.size(); i++) {
                if(match_blk_comment_close.position(i) != 0) continue;
                --comment_depth;
                colNum += 2;
                //std::cout << "Block comment ends. Depth " << comment_depth << "\n";
            }
        } else {
            ++colNum;
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
    tok = lex_internal();
    tokenizedFile.reserve(100);
    while(tok.type != TokenType::eof) {
        tokenizedFile.push_back(tok);
        tok = lex_internal();
    }
    tokenizedFile.push_back(tok);
    tokenizedFile.push_back(EOFTOKEN);
    currentIdx = 0;
}

Token LexerTarget::lex_internal() {
    std::string token = "";

    if(lineNum >= content.size()) {
        //currentTok = EOFTOKEN;
        updateTokens(EOFTOKEN);
        return currentTok;
    }
    
    std::string ln = content.at(lineNum);
    while(colNum >= ln.size()) {
        lineNum++;
        colNum = 0;
        if(lineNum >= content.size()) {
            //currentTok = EOFTOKEN;
            updateTokens(EOFTOKEN);
            return currentTok;
        }
        ln = content.at(lineNum);
    }

    while(isspace(ln[colNum])) {
        colNum++;

        //std::cout << "there is a space!!!\n";
        while(colNum >= ln.size()) {
            if(lineNum+1 >= content.size()) {
                //currentTok = EOFTOKEN;
                updateTokens(EOFTOKEN);
                return currentTok;
            } else {
                lineNum++;
                ln = content.at(lineNum);
                colNum = 0;
            }
        }
    }
            

    std::regex longest_regex_match;
    TokenType longest_match_type;
    int longest_match = 0;


    std::smatch match_comment;
    std::smatch match_blk_comment;
    std::string t = ln.substr(colNum);

    /*
     * check for block comments
     */
    if(t[0] == '/') {
        if(std::regex_search(t,match_blk_comment,comment_block_regex)) {
            for(unsigned int i = 0; i < match_blk_comment.size(); i++) {
                if(match_blk_comment.position(i) != 0) continue;
                ++comment_depth;
                colNum += 2;
                //std::cout << "Block comment begins. Depth " << comment_depth << "\n";
                //std::cout << "OOGIE BOOGIE BOO\n";
                lexcomment();
                return lex_internal();
            }
        }
    }
   

    //std::cout << "colNum = " << colNum << '\n';
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
    if(t[0] == '/') {
        if(std::regex_search(t,match_comment,comment_line_regex)) {
            for(unsigned int i = 0; i < match_comment.size(); i++) {
                if(match_comment.position(i) != 0) continue;

                colNum = 0;
                lineNum++;
                if(lineNum >= content.size()) {
                    //currentTok = EOFTOKEN;
                    updateTokens(EOFTOKEN);
                    return currentTok;
                }
                ln = content.at(lineNum);
                return lex_internal();
            }
        }
    }

    /*
     * Match other tokens
     */
    
    std::string remaining = ln.substr(colNum);
    switch(remaining[0]) {
        case '(':
            longest_match = 1;
            longest_match_type = TokenType::lparen;
            break;
        case ')':
            longest_match = 1;
            longest_match_type = TokenType::rparen;
            break;
        case '.':
            longest_match = 1;
            longest_match_type = TokenType::dot;
            break;
        case ',':
            longest_match = 1;
            longest_match_type = TokenType::comma;
            break;
        case '{':
            longest_match = 1;
            longest_match_type = TokenType::lbrace;
            break;
        case '}':
            longest_match = 1;
            longest_match_type = TokenType::rbrace;
            break;
        case ':':
            longest_match = 1;
            longest_match_type = TokenType::colon;
            break;
        case ';':
            longest_match = 1;
            longest_match_type = TokenType::semicolon;
            break;
        case '@':
            longest_match = 1;
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
                std::smatch tmp;
                bool matched = std::regex_search(remaining,tmp,str_lit_pair.first,std::regex_constants::match_continuous);
                if(matched) {
                    longest_match = tmp.length(0);
                    longest_match_type = str_lit_pair.second;
                }
            }
            break;
        case '+':
            {
                longest_match = 1;
                longest_match_type = TokenType::plus;
            }
            break;
        case '-':
            {
                longest_match = 1;
                longest_match_type = TokenType::minus;
            }
            break;
        case '/':
            {
                longest_match = 1;
                longest_match_type = TokenType::fslash;
            }
            break;
        case '*':
            {
                longest_match = 1;
                longest_match_type = TokenType::star;
            }
            break;
        case '^':
            {
                longest_match = 1;
                longest_match_type = TokenType::carrot;
            }
            break;
        case '%':
            {
                longest_match = 1;
                longest_match_type = TokenType::carrot;
            }
        case '=':
            {
                longest_match = 1;
                longest_match_type = TokenType::assignment;
                if(remaining[1] == '=') {
                    longest_match = 2;
                    longest_match_type = TokenType::equality;
                }
            }
            break;
        case '<':
        case '>':
        case '!':
        case '|':
        case '&':
            {
                std::smatch tmp;
                for (unsigned int i = 0; i < num_operator_regexes; i++) {
                    bool matched = std::regex_search(remaining,tmp,operator_regexes[i].first, std::regex_constants::match_continuous);
                    if(matched) {
                    for(unsigned int j = 0; j < tmp.size(); j++) {
                        if(tmp.length(j) > longest_match) {
                            longest_match = tmp.length(j);
                            longest_regex_match = operator_regexes[i].first;
                            longest_match_type = operator_regexes[i].second;
                        }
                    }
                    }
                }
            }
            break;
        default:
            {
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

                //TODO(marcus): error checks
                int len_t = 0;
                char current;
                do {
                    current =remaining[len_t];
                    len_t++;
                }while(isalnum(current) || current == '_');
                len_t--;
                longest_match = len_t;
                longest_match_type = TokenType::id;
                std::string matched_string = ln.substr(colNum,longest_match);
                for(unsigned int j = 0; j < num_keywords; j++) {
                   if(matched_string == keyword_array[j]) {
                       longest_match = strlen(keyword_array[j]);
                       longest_match_type = keyword_type[j];
                       break;
                   }
                }
                /*

                std::smatch tmp;
                bool matched = std::regex_search(remaining,tmp,regexes[0].first, std::regex_constants::match_continuous);
                if(matched) {
                    if(tmp.length(0) > longest_match) {
                        longest_match = tmp.length(0);
                        longest_regex_match = regexes[0].first;
                        longest_match_type = regexes[0].second;
                    }
                    std::string matched_string = ln.substr(colNum,longest_match);
                    for(unsigned int j = 0; j < num_keywords; j++) {
                       if(matched_string == keyword_array[j]) {
                           longest_match = strlen(keyword_array[j]);
                           longest_match_type = keyword_type[j];
                           break;
                       }
                    }
                }*/
            }
            break;
    }
    
    
    token = ln.substr(colNum,longest_match);
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
    Token ret = {
		longest_match_type, //type
		colNum, //col
		lineNum, //line
		token //token
	};
    colNum += longest_match;
    updateTokens(ret);
    //currentTok = ret;

    
    DEBUGLEX(std::cout << "token: " << token << "\n\n";)
    return currentTok;

}

Token LexerTarget::peek() {
    return tokenizedFile[currentIdx];
    //return currentTok;
}

Token LexerTarget::peekNext() {
    if(tokenizedFile.size()-2 > currentIdx) {
        return tokenizedFile[currentIdx+1];
    }
    return tokenizedFile[currentIdx];
    //return nextTok;
}

void LexerTarget::updateTokens(Token& t) {
    currentTok = nextTok;
    nextTok = t;
}

std::vector<std::string> read_file(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<std::string> content;
    
    if(!in.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return content;
    }
    
    std::string line;
    while(std::getline(in, line)) {
        content.push_back(line);
        //std::cout << line << '\n';
    }
    //std::cout << '\n';
    
    return content;
}

