#ifndef EXPRINTER_STATEMENTS_HPP
#define EXPRINTER_STATEMENTS_HPP

#include <string>
#include <vector>

#include "Expr.hpp"
#include "SymbolTable.hpp"

class Statement {
public:
    virtual ~Statement() = default;

    virtual void print() const = 0;
    virtual void evaluate(SymbolTable &symbolTable) const = 0;
};

class Statements {
public:
    ~Statements();

    void addStatement(Statement *statement);
    void evaluate(SymbolTable &symbolTable) const;
    void print() const;

private:
    std::vector<Statement *> statements;
};

class AssignmentStatement final : public Statement {
public:
    AssignmentStatement(std::string variableName,
                        ExprNode *expression);
    ~AssignmentStatement() override;

    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;

private:
    std::string variableName;
    ExprNode *expression;
};

class PrintStatement final : public Statement {
public:
    PrintStatement(ExprNode* expression);
    ~PrintStatement() override;
    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;
private:
    ExprNode *relExpr;
};

// for i in range(3):
//     i = 100
//AssignmentStatement *initializer, ExprNode *forStatementCompare, AssignmentStatement *forStatementIncr, Statements *forLoopStatements
class ForStatement final : public Statement {
public:
    ForStatement();
    ~ForStatement() override;
    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;
private:
    AssignmentStatement *initializer;
    ExprNode *forStatementCompare;
    AssignmentStatement *forStatementIncr;
    Statements *forLoopStatements;
};

class EvaluatedRange {
public:
    EvaluatedRange(int start, int stop, int step);

    [[nodiscard]] int start() const;
    [[nodiscard]] int stop() const;
    [[nodiscard]] int step() const;

    [[nodiscard]] bool hasIteration() const;
    [[nodiscard]] bool shouldContinue(int nextValue) const;

private:
    int start_;
    int stop_;
    int step_;
};

class RangeExpression {
public:
    explicit RangeExpression(ExprNode* stop);
    RangeExpression(ExprNode* start, ExprNode* stop);
    RangeExpression(
        ExprNode* start,
        ExprNode* stop,
        ExprNode* step
    );
    ~RangeExpression();

    RangeExpression(const RangeExpression&) = delete;
    RangeExpression& operator=(const RangeExpression&) = delete;

    [[nodiscard]] EvaluatedRange evaluate(const SymbolTable& symbolTable) const;

    void print(std::ostream& output) const;

private:
    ExprNode* startExpression;
    ExprNode* stopExpression;
    ExprNode* stepExpression;
};
#endif // EXPRINTER_STATEMENTS_HPP
