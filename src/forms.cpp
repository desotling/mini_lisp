#include "./forms.h"
#include "./error.h"
#include "./eval_env.h"
#include <memory>
#include <unordered_set>
#include <limits>
#include <typeinfo>

std::vector<ValuePtr> cutFirst(const std::vector<ValuePtr>& args) {
    std::vector<ValuePtr> temp;
    for(int i = 1; i < args.size(); i++) {
        temp.push_back(args[i]);
    }
    return temp;
}

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (auto name = args[0]->asSymbol()) {
        env.symbolPair.insert_or_assign(*name, args[1]);
        return ValuePtr(new NilValue());
    } else if(typeid(*args[0] == typeid(PairValue))) {
        auto expr = static_cast<PairValue&>(*args[0]);
        if(auto name = expr.getleft()->asSymbol()){
            auto temp = ValuePtr(new LambdaValue(expr.getleft(), cutFirst(args)));
            env.addVariable(*name, temp);
            return ValuePtr(new NilValue());
        }
        return ValuePtr(new NilValue());
    } else {
        throw LispError("Unimplemented");
    }
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    return ValuePtr(new LambdaValue((args[0]), cutFirst(args)));
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (env.eval(std::move(args[0]))->isTrue()) {
        return env.eval(std::move(args[1]));
    } else if (args.size() == 3) {
        return env.eval(std::move(args[2]));
    } else {
        return std::make_shared<NilValue>();
    }
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    int size = args.size();
    if (size == 0) {
        return std::make_shared<BooleanValue>(1);
    }
    ValuePtr tmp = env.eval(args[0]);
    for (int i = 0; i < size; i++) {
        if (!tmp->isTrue())
            return std::make_shared<BooleanValue>(0);
        if (i<size-1)
            tmp = env.eval(args[i+1]);
    }
    return tmp;
}
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    int size = args.size();
    if (size == 0) {
        return std::make_shared<BooleanValue>(0);
    }
    for (int i = 0; i < size; i++) {
        ValuePtr tmp = env.eval(args[i]);
        if (tmp->isTrue()) return tmp;
    }
    return std::make_shared<BooleanValue>(0);
} 

const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS {
    {"define", defineForm},
    {"quote", quoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm}
};
