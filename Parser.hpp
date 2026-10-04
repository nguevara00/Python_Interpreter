#ifndef EXPRINTER_PARSER_HPP
#define EXPRINTER_PARSER_HPP

#include <string>

#include "Expr.hpp"
#include "Statements.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

class Parser {
public:
    explicit Parser(Tokenizer &tokenizer) : tokenizer{tokenizer} {}

    Statements *program();
    Statements *statements();
    Statement *statement();
    Statement* simpleStatement();
    AssignmentStatement *assignmentStatement();
    PrintStatement *printStatement();
    ForStatement* forStatement();
    Statement* compoundStatement();
    Statements* suite();

    ExprNode *rangeExpr();
    ExprNode *relExpr();
    ExprNode *relTerm();
    ExprNode *relPrimary();
    ExprNode *arithExpr();
    ExprNode *arithTerm();
    ExprNode *arithPrimary();
    ExprNode *arithAtom();

private:
    Tokenizer &tokenizer;

    [[noreturn]] void die(const std::string &where,
                          const std::string &message,
                          const Token &token) const;
};

#endif // EXPRINTER_PARSER_HPP
