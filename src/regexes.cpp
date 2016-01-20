#include "regexes.h"

std::regex regexes[] = { 
    std::regex("fn"),
    std::regex("extern"),
    std::regex("/\\*"),
    std::regex("//.*"),
    std::regex("[a-zA-Z_][a-zA-Z0-9_]*"),
    std::regex("[0-9]+\\.[0-9]*"),
    std::regex("[0-9]+"),
    std::regex("="),    
    std::regex("=="),    
    std::regex("!="),    
    std::regex("<"),    
    std::regex("<="),    
    std::regex(">"),    
    std::regex(">="),    
    std::regex("\\("),    
    std::regex("\\)"),    
    std::regex("\\{"),    
    std::regex("\\}"),    
    std::regex("\\."),    
    std::regex("\\,"),    
    std::regex("\\+"),    
    std::regex("-"),    
    std::regex("\\*"),    
    std::regex("/"),    
    std::regex(";"),    
    std::regex(":"),    
    std::regex("!"),  
    std::regex("\\?")   
};

unsigned int num_regexes = sizeof(regexes)/sizeof(std::regex);


