#include "./parser.h"
#include "./error.h"
#include <memory>

Parser::Parser(std::deque<TokenPtr> tokens){
    this->tokens = std::move(tokens);
}

ValuePtr Parser::parse() {
    auto token = std::move(tokens.front());
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<NumericValue>(value);
    }
    else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<BooleanValue>(value);
    }
    else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        tokens.pop_front();
        return std::make_shared<StringValue>(value);
    }
    else if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        tokens.pop_front();
        return std::make_shared<SymbolValue>(value);
    }
    else if (token->getType() == TokenType::LEFT_PAREN) {
        tokens.pop_front();
        return parseTails();
    }
    else if (token->getType() == TokenType::QUOTE) {
        tokens.pop_front();
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    else if (token->getType() == TokenType::UNQUOTE) {
        tokens.pop_front();
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("unquote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    else if (token->getType() == TokenType::QUASIQUOTE) {
        tokens.pop_front();
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quasiquote"),
            std::make_shared<PairValue>(
                this->parse(),
                std::make_shared<NilValue>()
            )
        );
    }
    else {
        tokens.pop_front();
        throw SyntaxError("Unimplemented");
    }
}

ValuePtr Parser::parseTails() {
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        NilValue value;
        return std::make_shared<NilValue>(value);
    }
    auto car = this->parse();
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        if(tokens.front()->getType() == TokenType::RIGHT_PAREN) {
            tokens.pop_front();
        } else {
            throw SyntaxError("Unimplemented");
        }
        PairValue value(car, cdr);
        return std::make_shared<PairValue>(value);
    } else {
        auto cdr = this->parseTails();
        PairValue value(car, cdr);
        return std::make_shared<PairValue>(value);
    }
}
