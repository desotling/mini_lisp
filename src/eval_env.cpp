#include "./eval_env.h"
#include "./forms.h"
#include "./builtins.h"
#include "./error.h"

#include <typeinfo>
#include <vector>
#include <ranges>
#include <iterator>
#include <algorithm>

EvalEnv::EvalEnv(){
    for(auto&&[name, func] : BUILTIN_FUNCS){
        symbolPair.insert_or_assign(name, std::make_shared<BuiltinProcValue>(&func));
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (expr->isPair()) {
        std::vector<ValuePtr> v = expr->toVector();
        auto expr_pair = static_cast<PairValue&>(*expr);
        if (auto name = expr_pair.getleft()->asSymbol()) {
            if (auto it = SPECIAL_FORMS.find(*name); it != SPECIAL_FORMS.end()) {//特殊形式
                return it->second(expr_pair.getleft()->toVector(), *this);
            } else {//内置过程
                ValuePtr proc = this->eval(v[0]);
                auto value = static_cast<PairValue&>(*expr);
                std::vector<ValuePtr> args = this->evalList(value.getright());  //将列表的剩余元素都求值一遍然后收集起来
                return this->apply(proc,args);  // 最后用 EvalEnv::apply 实现调用
            }
        }
    } else if(auto name = expr->asSymbol()) {
        auto value = symbolPair.find(*name);
        if(value != symbolPair.end()){
            auto v = value->second;
            return v;           
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    } else {
        throw LispError("Unimplemented");
    }
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        auto ptr = static_cast<BuiltinProcValue&>(*proc);
        auto func = ptr.getfunc();
        return (*func)(args);
        
    } else {
        throw LispError("Unimplemented");
    }
}



