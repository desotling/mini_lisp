#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <unordered_map>
#include <string>
#include "./value.h"

class EvalEnv {
public:
    std::unordered_map<std::string, ValuePtr> symbolPair;
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args); 
};

#endif