#include <iostream>
#include <string>
#include <utility>

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
    std::cout << relExpr->evaluate(symbolTable) << std::endl;
}

void PrintStatement::print() const {
    // relExpr->print();
    // std::cout << '\n';
}

// initializer, forStatementCompare, forStatementIncr, forloopStatements
// ForStatement::ForStatement(AssignmentStatement *initializer, ExprNode *forStatementCompare, AssignmentStatement *forStatementIncr, Statements *forLoopStatements) :
//     initializer{initializer}, forStatementCompare{forStatementCompare}, forStatementIncr{forStatementIncr}, forLoopStatements{forLoopStatements}
// {}

ForStatement::ForStatement(std::string variable, RangeExpression *range, Statements *suite) :
    variableName_{variable}, range_{range}, suite_{suite} {}

ForStatement::~ForStatement() {
    delete range_;
    delete suite_;
};



void ForStatement::evaluate(SymbolTable &symbolTable) const {

    EvaluatedRange evalRange = range_->evaluate(symbolTable);

    int nextValue = evalRange.start();

    if (evalRange.hasIteration()) {
        while (evalRange.shouldContinue(nextValue)) {
            symbolTable.setValueFor(variableName_, nextValue);
            suite_->evaluate(symbolTable);
            nextValue += evalRange.step();
        }
    }
}

void ForStatement::print() const {
    // relExpr->print();
    // std::cout << '\n';
}

EvaluatedRange::EvaluatedRange(int start, int stop, int step) {
    if (step == 0){
        std::cout << "EvaluatedRange::Constructor - step must be non-zero, step was: " << step << std::endl;
        std::exit(-1);
    }
    start_ = start;
    stop_ = stop;
    step_ = step;
}

int EvaluatedRange::start() const{
    return start_;
}

int EvaluatedRange::stop() const{
    return stop_;
}

int EvaluatedRange::step() const{
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

bool EvaluatedRange::shouldContinue(int nextValue) const{
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

[[nodiscard]] EvaluatedRange RangeExpression::evaluate(const SymbolTable& symbolTable) const{
    int start = 0;
    int stop = 0; 
    int step = 1;
    
    if (startExpression != nullptr) {
        start = startExpression->evaluate(symbolTable);
    }

    stop = stopExpression->evaluate(symbolTable);

    if (stepExpression != nullptr){
        step = stepExpression->evaluate(symbolTable);
    }

    EvaluatedRange range(start,stop,step);
    return range;
}

void RangeExpression::print(std::ostream& output) const{
    //stuff
}