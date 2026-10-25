#include <iostream>
#include <string>
#include <utility>
#include <cstdint>
#include <stdexcept>

#include "Statements.hpp"

Statements::~Statements() {
    for (auto *statement : statements)
        delete statement;
}

void Statements::addStatement(Statement *statement) {
    statements.push_back(statement);
}

void Statements::print() const {
    for (const auto *statement : statements)
        statement->print();
}

void Statements::evaluate(SymbolTable &symbolTable) const {
    for (const auto *statement : statements)
        statement->evaluate(symbolTable);
}

AssignmentStatement::AssignmentStatement(std::string variableName, ExprNode *expression): variableName{std::move(variableName)}, expression{expression} {}

AssignmentStatement::~AssignmentStatement() {
    delete expression;
}

void AssignmentStatement::evaluate(SymbolTable &symbolTable) const {
    symbolTable.setValueFor(variableName, expression->evaluate(symbolTable));
}

void AssignmentStatement::print() const {
    std::cout << variableName << " = ";
    expression->print();
    std::cout << '\n';
}

PrintStatement::PrintStatement(ExprNode *expression) : relExpr{expression} {}

PrintStatement::~PrintStatement() {
    delete relExpr;
};

void PrintStatement::evaluate(SymbolTable &symbolTable) const {
    ValueDescriptor* value = (relExpr->evaluate(symbolTable));
    std::cout << value->toString() << std::endl;
    delete value;
}

//restored print function
void PrintStatement::print() const {
    std::cout << "print ";
    relExpr->print();
    std::cout << '\n';
}

ForStatement::ForStatement(std::string variable, RangeExpression *range, Statements *suite) :
    variableName_{variable}, range_{range}, suite_{suite} {}

ForStatement::~ForStatement() {
    delete range_;
    delete suite_;
};

void ForStatement::evaluate(SymbolTable &symbolTable) const {

    EvaluatedRange evalRange = range_->evaluate(symbolTable);

    int64_t nextValue = evalRange.start();

    if (evalRange.hasIteration()) {
        while (evalRange.shouldContinue(nextValue)) {
            IntegerValueDescriptor* nextValueDescriptor = new IntegerValueDescriptor(nextValue);
            symbolTable.setValueFor(variableName_, nextValueDescriptor);
            suite_->evaluate(symbolTable);
            nextValue += evalRange.step();
        }
    }
}

//fixed forstatementprint to match other print statements
void ForStatement::print() const {
    std::cout << "for " << variableName_ << " in ";
    range_->print(std::cout);
    std::cout << ":\n";
    suite_->print();
}

EvaluatedRange::EvaluatedRange(int64_t start, int64_t stop, int64_t step) {
    if (step == 0){
        std::cout << "EvaluatedRange::Constructor - step must be non-zero, step was: " << step << std::endl;
        throw std::runtime_error("range step cannot be zero");
    }
    start_ = start;
    stop_ = stop;
    step_ = step;
}

int64_t EvaluatedRange::start() const{
    return start_;
}

int64_t EvaluatedRange::stop() const{
    return stop_;
}

int64_t EvaluatedRange::step() const{
    return step_;
}

bool EvaluatedRange::hasIteration() const{
    //EvaluatedRange::hasIteration() determines whether the start value belongs to the range.
    //For a positive step, it returns whether start < stop. For a negative step, it returns whether start > stop.
    if (step_ > 0){
        return start_ < stop_;
    } else {
        return start_ > stop_;
    }
}

bool EvaluatedRange::shouldContinue(int64_t nextValue) const{
    //EvaluatedRange::shouldContinue(nextValue) applies the same directional boundary test to the value supplied by the ForStatement. 
    //The start(), stop(), and step() functions return the corresponding concrete values.
    if (step_ > 0) {
        return (nextValue < stop_);
    } else {
        return (nextValue > stop_);
    }

}

RangeExpression::RangeExpression(ExprNode* stop){
    startExpression = nullptr;
    stopExpression = stop;
    stepExpression = nullptr;
}

RangeExpression::RangeExpression(ExprNode* start, ExprNode* stop){
    startExpression = start;
    stopExpression = stop;
    stepExpression = nullptr;
}

RangeExpression::RangeExpression(ExprNode* start,ExprNode* stop,ExprNode* step){
    startExpression = start;
    stopExpression = stop;
    stepExpression = step;
}

RangeExpression::~RangeExpression(){
    delete startExpression;
    delete stopExpression;
    delete stepExpression;
}

EvaluatedRange RangeExpression::evaluate(const SymbolTable& symbolTable) const{
    int64_t start = 0;
    int64_t stop = 0; 
    int64_t step = 1;
    
    if (startExpression != nullptr) {
        ValueDescriptor* value = startExpression->evaluate(symbolTable);

        if (value->type() != ValueDescriptor::Type::integer){
            std::string type = value->typeName();
            delete value;
            throw std::runtime_error("range start must be integer, is " + type);
        }

        start = static_cast<IntegerValueDescriptor*>(value)->value();\
        delete value;
    }

    ValueDescriptor* value = stopExpression->evaluate(symbolTable);
    if (value->type() != ValueDescriptor::Type::integer){
        std::string type = value->typeName();
        delete value;
        throw std::runtime_error("range stop must be integer, is " + type);
    }

    stop = static_cast<IntegerValueDescriptor*>(value)->value();
    delete value;

    if (stepExpression != nullptr){
        ValueDescriptor* value = stepExpression->evaluate(symbolTable);
        if (value->type() != ValueDescriptor::Type::integer){
            std::string type = value->typeName();
            delete value;
            throw std::runtime_error("range step must be integer, is " + type);
        }
        step = static_cast<IntegerValueDescriptor*>(value)->value();
        delete value;
    }

    return EvaluatedRange(start,stop,step);
}

void RangeExpression::print(std::ostream& output) const{
    output << "range(";
    if (startExpression != nullptr) {
        startExpression->print();
        output << ", ";
    }
    stopExpression->print();
    if (stepExpression != nullptr) {
        output << ", ";
        stepExpression->print();
    }
    output << ')';
}