#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <optional>
#include <memory>
#include <ostream>
#include <deque>
#include <vector>

class Value;
using ValuePtr = std::shared_ptr<Value>;

class Value{
public:
    Value(){};
    bool isNil();
    bool isSelfEvaluating();
    bool isPair();
    bool isNumber();
    bool isBoolean() const;
    bool isTrue() const;
    virtual double asNumber() const;
    std::vector<ValuePtr> toVector();
    std::optional<std::string> asSymbol();
    virtual std::string toString() const=0;
};

class BooleanValue: public Value{
    bool value;
public:
    BooleanValue(){};
    BooleanValue(bool v):value{v}{};
    BooleanValue(const BooleanValue& m):value{m.value}{};
    std::string toString() const override;
    bool getValue() const {return value;};
};
class NumericValue: public Value{
    double value;
public:
    NumericValue(){};
    NumericValue(double v):value{v}{};
    NumericValue(const NumericValue& m):value{m.value}{};
    std::string toString() const override;
    double getValue() const {return value;};
};
class StringValue: public Value{
    std::string value;
public:
    StringValue(){};
    StringValue(std::string v):value{v}{};
    StringValue(const StringValue& m):value{m.value}{};
    std::string toString() const override;
};
class NilValue: public Value{
public:
    NilValue(){};
    NilValue(const NilValue& m){};
    std::string toString() const override;
};
class SymbolValue: public Value{
    std::string value;
public:
    SymbolValue(){};
    SymbolValue(std::string v):value{v}{};
    SymbolValue(const SymbolValue& m):value{m.value}{};
    std::string toString() const override;
};

class PairValue: public Value{
    ValuePtr left;
    ValuePtr right;
public:
    PairValue(){};
    PairValue(ValuePtr a, ValuePtr b):left{a},right{b}{};
    PairValue(const PairValue& v):left{v.left},right{v.right}{};
    std::string toString() const override;
    std::string getit() const;
    ValuePtr getleft() const;
    ValuePtr getright() const;
};

using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);

class BuiltinProcValue : public Value {
    BuiltinFuncType* func;
public:
    BuiltinProcValue(){};
    BuiltinProcValue(BuiltinFuncType* func):func{func}{};
    BuiltinFuncType* getfunc(){return func;};
    std::string toString() const override;
};

class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    // [...]
public:
    std::string toString() const override; // 如前所述，返回 #<procedure> 即可
};
#endif