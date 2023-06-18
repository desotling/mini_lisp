#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <unordered_map>
#include <string>
#include "./value.h"

class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
private:
    std::shared_ptr<EvalEnv> parent;
    std::unordered_map<std::string, ValuePtr> symbolPair;
    EvalEnv() = default;
public:
    //EvalEnv(const EvalEnv&) = delete;
    static std::shared_ptr<EvalEnv> createGlobal();
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, const std::vector<ValuePtr>& args); 
    ValuePtr lookupBinding(const std::string& name) const;
    void defineBinding(const std::string& name, ValuePtr value);
};

#endif