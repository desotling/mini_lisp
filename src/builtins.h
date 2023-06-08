#ifndef BUILTINS_H
#define BUILTINS_H
#include <iostream>
#include <unordered_map>
#include "./value.h"

extern const std::unordered_map<std::string, BuiltinFuncType*> BUILTIN_FUNCS;

#endif