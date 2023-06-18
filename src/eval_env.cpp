#include "./eval_env.h"
#include "./forms.h"
#include "./builtins.h"
#include "./error.h"

#include <typeinfo>
#include <vector>
#include <ranges>
#include <iterator>
#include <algorithm>

std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    std::shared_ptr<EvalEnv> env(new EvalEnv);
    for (auto&& [name, func] : BUILTIN_FUNCS) {
        env->defineBinding(name, std::make_shared<BuiltinProcValue>(func));
    }
    return env;
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    if (params.size() != args.size()) {
        throw LispError("Procedure expected " + std::to_string(params.size()) +
                        " parameters, got " + std::to_string(args.size()));
    }
    std::shared_ptr<EvalEnv> childEnv(new EvalEnv());
    childEnv->parent = shared_from_this();
    for (std::size_t i = 0; i < params.size(); i++) {
        childEnv->defineBinding(params[i], args[i]);
    }
    return childEnv;
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (expr->isPair()) {
        using namespace std::literals;//使用s后缀
        std::vector<ValuePtr> v = expr->toVector();
        auto expr_pair = static_cast<PairValue&>(*expr);
        if (auto name = expr_pair.getleft()->asSymbol()) {
            if (auto it = SPECIAL_FORMS.find(*name); it != SPECIAL_FORMS.end()) {//特殊形式
                std::cout << 6 << std::endl;
                const SpecialFormType* value = it->second;
                return value(expr_pair.getright()->toVector(), *this);
            } else {//内置过程
                ValuePtr proc = this->eval(v[0]);
                std::cout << 7 << std::endl;
                auto value = static_cast<PairValue&>(*expr);
                std::vector<ValuePtr> args = this->evalList(value.getright());  //将列表的剩余元素都求值一遍然后收集起来
                return this->apply(proc,args);  // 最后用 EvalEnv::apply 实现调用
            }
        }
    } else if(auto name = expr->asSymbol()) {
        auto value = this->lookupBinding(*name);
        if (!value) {
            throw LispError("Unbound variable " + *name);
        }
        return value;
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

ValuePtr EvalEnv::apply(ValuePtr proc, const std::vector<ValuePtr>& args) {
    if (!proc->isProcedure()) {
        throw LispError("Not a procedure " + proc->toString());
    }
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        // 调用内置过程
        auto value = static_cast<BuiltinProcValue&>(*proc);
        auto temp = value.getfunc();
        return (*temp)(args);
    } 
    auto value = static_cast<LambdaValue&>(*proc);
    return value.apply(args);

}

void EvalEnv::defineBinding(const std::string& name, ValuePtr value) {
    symbolPair[name] = std::move(value);
}

ValuePtr EvalEnv::lookupBinding(const std::string& name) const {
    auto it = symbolPair.find(name);
    if (it == symbolPair.end()) {
        return parent ? parent->lookupBinding(name) : nullptr;
    }
    return it->second;
}


