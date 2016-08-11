#ifndef COLOROUTPUT_H
#define COLOROUTPUT_H

#define ANSI_RED "\x1b[31m" 
#define ANSI_CLEAR "\x1b[0m"
#define ERROR(X) std::cout<<ANSI_RED<<X<<ANSI_CLEAR;

#endif
