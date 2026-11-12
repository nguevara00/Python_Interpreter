#ifndef EXPRINTER_EXPR_HPP
#define EXPRINTER_EXPR_HPP

#include "SymbolTable.hpp"
#include "Token.hpp"
#include "ValueDescriptor.hpp"

class ExprNode {
public:
    explicit ExprNode(Token token);
    virtual ~ExprNode() = default;

    [[nodiscard]] const Token &token() const;
    virtual void print() const = 0;
    [[nodiscard]] virtual ValueDescriptor* evaluate(const SymbolTable &symbolTable) const = 0;

private:
    Token expressionToken;
};

class BinaryExprNode final : public ExprNode {
public:
    BinaryExprNode(Token token, ExprNode *left, ExprNode *right);
    ~BinaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *leftOperand;
    ExprNode *rightOperand;
};

class UnaryExprNode final : public ExprNode {
public:
    UnaryExprNode(Token token, ExprNode *operand);
    ~UnaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class IntegerLiteral final : public ExprNode {
public:
    explicit IntegerLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
};

class DoubleLiteral final : public ExprNode {
public:
    explicit DoubleLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
};

class StringLiteral final : public ExprNode {
public:
    explicit StringLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
};

class BooleanLiteral final : public ExprNode {
public:
    explicit BooleanLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
};

class Variable final : public ExprNode {
public:
    explicit Variable(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
};

class StringConversionExpression final : public ExprNode {
public:
    StringConversionExpression(Token token, ExprNode* expression);
    ~StringConversionExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};

class IntConversionExpression final : public ExprNode {
public:
    IntConversionExpression(Token token, ExprNode* expression);
    ~IntConversionExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};

class FloatConversionExpression final : public ExprNode {
public:
    FloatConversionExpression(Token token, ExprNode* expression);
    ~FloatConversionExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};

class InputExpression final : public ExprNode {
public:
    InputExpression(Token token);
    ~InputExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
};

class ListLiteral final : public ExprNode {
public:
    // StringConversionExpression::StringConversionExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}
    ListLiteral(Token token, std::vector<ExprNode*> expressions) : ExprNode{std::move(token)}, _expressions{std::move(expressions)}{}
    ~ListLiteral() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    std::vector<ExprNode*> _expressions;
};

/*
class LenExpression final : public ExprNode {
public:
    LenExpression(Token token, ExprNode* expression);
    ~LenExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};

class SplitExpression final : public ExprNode {
public:
    SplitExpression(Token token, ExprNode* expression);
    ~SplitExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};

class PopExpression final : public ExprNode {
public:
    PopExpression(Token token, ExprNode* expression);
    ~PopExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor* evaluate(const SymbolTable &symbolTable) const override;
private:
    ExprNode* _expression;
};
*/

#endif