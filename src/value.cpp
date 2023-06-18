#include "./value.h"
#include "./error.h"
#include "./eval_env.h"
#include <typeinfo>
#include <string>
#include <iomanip>

using namespace std;

bool Value::isSelfEvaluating() {
    if(typeid(*this) == typeid(BooleanValue)
     ||typeid(*this) == typeid(StringValue)
     ||typeid(*this) == typeid(NumericValue)) {
        return true;
     }
    else return false;
}

bool Value::isNil() {
    if(typeid(*this) == typeid(NilValue)) {
        return true;
     }
    else return false;
}

bool Value::isPair() {
    if(typeid(*this) == typeid(PairValue)) {
        return true;
     }
    else return false;
}

bool Value::isNumber() {
    if(typeid(*this) == typeid(NumericValue)) {
        return true;
     }
    else return false;
}

bool Value::isString() {
    if(typeid(*this) == typeid(StringValue)) {
        return true;
     }
    else return false;
}

bool Value::isSymbol() {
    if(typeid(*this) == typeid(SymbolValue)) {
        return true;
     }
    else return false;
}

bool Value::isBoolean() {
    return typeid(*this) == typeid(BooleanValue);
}

bool Value::isList() {
    auto current = this;
    while (!current->isNil()) {
        if (!current->isPair()) {
            return false;
        }
        auto& tmp = static_cast<const PairValue&>(*current);
        current = tmp.getright().get();
    }
    return true;
}

bool Value::isProcedure(){
    if(typeid(*this) == typeid(BuiltinProcValue)||typeid(*this) == typeid(LambdaValue)){
        return true;
    } else {
        return false;
    }
}

bool Value::isTrue() {
    if (!isBoolean()) return true;
    return static_cast<const BooleanValue*>(this)->getValue();
}

double Value::asNumber() {
    return static_cast<const NumericValue*>(this)->getValue();
}

std::vector<ValuePtr> Value::toVector() {
    if(this->isNil()) {
        return {};
    } else if(typeid(*this) == typeid(PairValue)) {
        std::vector<ValuePtr> c;
        auto& v = static_cast<const PairValue&>(*this);
        c.push_back(v.getleft());
        auto remain = v.getright();
        while(this->isNil()||this->isPair()) {
            if(remain->isNil()) { 
                return c;
            } else {
                auto& rv = static_cast<const PairValue&>(*remain);
                c.push_back(rv.getleft());
                remain = rv.getright();
            }
        }
        c.push_back(remain);
        return c;
    }
    else{
        throw LispError("Not a pair.");
    }
}

std::optional<std::string> Value::asSymbol() {
    if(typeid(*this)==typeid(SymbolValue)){
        std::optional<std::string> v(this->toString());
        return v;
    }
    else{
        return std::nullopt;
    }
}

std::string BooleanValue::toString() const {
    if(value){
        return "#t";
    }
    else{
        return "#f";
    }
}
std::string NumericValue::toString() const {
    int v=value;
    double v1=double(v);
    if(v1==value){
        return to_string(v);
    }
    else{
        return to_string(value);
    }
}

std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return value;
}

std::string PairValue::toString() const {
    return "(" + (*this).getit() + ")";
}

std::string PairValue::getit() const {
    if (typeid(*right) == typeid(PairValue)) {
        auto& pair = static_cast<const PairValue&>(*right);
        return (*left).toString() + " " + (pair).getit();
    }
    else if(typeid(*right) == typeid(NilValue)) {
        return (*left).toString();
    }
    else return (*left).toString() + " ." + (*right).toString();
}

ValuePtr PairValue::getleft() const {
    return this->left;
}

ValuePtr PairValue::getright() const {
    return this->right;
}

std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    std::cout << 2 << std::endl;
    auto childEnv = env->createChild(params, args);
    auto result = childEnv->eval(body[0]);
    for (int i = 1; i < body.size(); i++) {
        result = childEnv->eval(body[i]);
    }
    return result;
}
