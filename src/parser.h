#ifndef PARSER_H
#define PARSER_H

#include "./value.h"
#include "./token.h"

class Parser {
private:
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens);
    ValuePtr parse();
    ValuePtr parseTails();
};


#endif