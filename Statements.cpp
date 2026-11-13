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

AssignmentTarget::AssignmentTarget(std::string variableName, ExprNode* expression) : variableName_{variableName}, expression_{expression}{}
AssignmentTarget::~AssignmentTarget(){
    delete expression_;
}
const std::string &AssignmentTarget::variableName() const {
    return variableName_;
}
ExprNode* AssignmentTarget::expression() const {
    return expression_;
}


AssignmentStatement::AssignmentStatement(AssignmentTarget* target, ExprNode *expression): target_{target}, expression_{expression} {}

AssignmentStatement::~AssignmentStatement() {
    delete target_;
    delete expression_;
}

void AssignmentStatement::evaluate(SymbolTable &symbolTable) const {
    if (target_->expression() == nullptr){
        symbolTable.setValueFor(target_->variableName(), expression_->evaluate(symbolTable));
        return;
    }
    
    ValueDescriptor* targetValue = symbolTable.getValueFor(target_->variableName());
    if (targetValue->type() != ValueDescriptor::Type::list){
        throw std::runtime_error("assignment target is not a list");
    }

    ListValueDescriptor *listValue = static_cast<ListValueDescriptor*>(targetValue);
    ListObject* list = listValue->object();

    ValueDescriptor* value = target_->expression()->evaluate(symbolTable);

    if (value->type() != ValueDescriptor::Type::integer){
        delete value;
        throw std::runtime_error("list index must be an integer");
    }

    std::int64_t index = static_cast<IntegerValueDescriptor*>(value)->value();
    delete value;

    if (index < 0 || static_cast<std::size_t>(index) >= list->size()){
        throw std::runtime_error("list index is out of range");
    }

    ValueDescriptor* newValue = expression_->evaluate(symbolTable);
    ListElementType valueType;

    switch (newValue->type()){
        case ValueDescriptor::Type::integer:
            valueType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::doubleValue:
            valueType = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::boolean:
            valueType = ListElementType::boolean;
            break;
        case ValueDescriptor::Type::string:
            valueType = ListElementType::string;
            break;
        case ValueDescriptor::Type::list:
            delete newValue;
            throw std::runtime_error("lists cannot contain lists");
    }

    if (list->elementType() != ListElementType::unset && list->elementType() != valueType) {
        delete newValue;
        throw std::runtime_error("list element type mismatch");
    }

    list->setElement(static_cast<std::size_t>(index), newValue);

}

void AssignmentStatement::print() const {
    std::cout << target_->variableName();

    if (target_->expression() != nullptr){
        std::cout << '[';
        target_->expression()->print();
        std::cout << ']';
    }
    std::cout << " = ";
    expression_->print();
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

IfStatement::IfStatement(std::vector<ExprNode*> conditions, std::vector<Statements*> suites, Statements* elseSuite) : conditions_{conditions}, suites_{suites}, elseSuite_{elseSuite}{}
IfStatement::~IfStatement(){
    for (ExprNode* condition: conditions_){
        delete condition;
    }
    for (Statements* suite : suites_){
        delete suite;
    }
    delete elseSuite_;
}
void IfStatement::evaluate(SymbolTable &symbolTable) const {
    for (std::size_t i = 0; i < conditions_.size(); ++i){
        ValueDescriptor *value = conditions_[i]->evaluate(symbolTable);

        if (value->type() != ValueDescriptor::Type::boolean){
            std::string type = value->typeName();
            delete value;
            throw std::runtime_error("if condition must be boolean, is" + type);
        }

        bool condition = static_cast<BooleanValueDescriptor*>(value)->value();
        delete value;
        if (condition) {
            suites_[i]->evaluate(symbolTable);
            return;
        }
    }
    if (elseSuite_ != nullptr){
        elseSuite_ -> evaluate(symbolTable);
    }
}
void IfStatement::print() const {
    for (std::size_t i = 0; i < conditions_.size(); ++i){
        if (i == 0){
            std::cout << "if ";
        }
        else{
            std::cout << "elif ";
        }

        conditions_[i]->print();
        std::cout << ":\n";
        suites_[i]->print();
    }
    if (elseSuite_ != nullptr){
        std::cout << "else:\n";
        elseSuite_->print();
    }
}

AppendStatement::AppendStatement(ExprNode* first, ExprNode* second) : _first{first}, _second{second}{}

AppendStatement::~AppendStatement() {
    delete _first;
    delete _second;
}

void AppendStatement::evaluate(SymbolTable &symbolTable) const {
    ValueDescriptor* first = _first->evaluate(symbolTable);
    if (first->type() != ValueDescriptor::Type::list) {
        std::string firstType = first->typeName();
        delete first;
        throw std::runtime_error("Append requires a list not a " + firstType);
    }

    ListObject *list = static_cast<ListValueDescriptor*>(first)->object();
    ValueDescriptor* second = _second->evaluate(symbolTable);
    if (second->type() == ValueDescriptor::Type::list) {
        delete second;
        delete first;
        throw std::runtime_error("lists cannot contain lists");
    }

    ListElementType secondCategory;
    switch (second->type()) {
        case ValueDescriptor::Type::integer:
            secondCategory = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::doubleValue:
            secondCategory = ListElementType::numeric;
            break;
        case ValueDescriptor::Type::boolean:
            secondCategory = ListElementType::boolean;
            break;
        case ValueDescriptor::Type::string:
            secondCategory = ListElementType::string;
            break;
        case ValueDescriptor::Type::list:
            delete second;
            delete first;
            throw std::runtime_error("lists cannot contain lists");
    }

    if (list->elementType() != ListElementType::unset && list->elementType() != secondCategory){
        delete second;
        delete first;
        throw std::runtime_error("list element type mismatch");
    }

    list->append(second);
    delete first;
}

void AppendStatement::print() const {
    std::cout << "append(";
    _first->print();
    std::cout << ", ";
    _second->print();
    std::cout << ")" << std::endl;
}

WhileStatement::WhileStatement(ExprNode* whileCondition, Statements* whileSuite) : whileCondition_{whileCondition}, suite_{whileSuite}{}

WhileStatement::~WhileStatement(){
    delete whileCondition_;
    delete suite_;
}

void WhileStatement::evaluate(SymbolTable &symbolTable) const {
    while (true){

        ValueDescriptor *value = whileCondition_->evaluate(symbolTable);
        if (value->type() != ValueDescriptor::Type::boolean){
            delete value;
            throw std::runtime_error("WhileStatement::evaluate While condition must be a boolean");
        }

        BooleanValueDescriptor* whileCondition = static_cast<BooleanValueDescriptor*>(value);
        if (!whileCondition->value()){
            delete value;
            break;
        }

        delete value;
        suite_->evaluate(symbolTable);   
    }
}

void WhileStatement::print() const {
    std::cout << "while "; 
    whileCondition_->print();
    std::cout << ":\n";
    suite_->print(); 
}
