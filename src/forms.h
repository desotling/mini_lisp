#ifndef FORMS_H
#define FORMS_H
#include <iostream>
#include <unordered_map>
#include "./value.h"
//#include "./eval_env.h"
class EvalEnv;

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;

#endif