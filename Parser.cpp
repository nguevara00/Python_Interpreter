#include <cstdlib>
#include <iostream>
#include <string>

#include "Parser.hpp"

void Parser::die(const std::string &where,
                 const std::string &message,
                 const Token &token) const {
    std::cerr << where << ": " << message << " at line " << token.lineNumber()
              << ", column " << token.columnNumber() << ". Got: ";
    token.print(std::cerr);
    std::cerr << "\n\nTokens identified up to this point:\n";
    tokenizer.printProcessedTokens(std::cerr);
    std::exit(EXIT_FAILURE);
}

Statements *Parser::program() {
    // <program> -> <statements> EOF
    Statements *parsedStatements = statements();
    Token eof = tokenizer.getToken();
    if (!eof.isEof()) {
        delete parsedStatements;
        die("Parser::program", "expected EOF", eof);
    }
    return parsedStatements;
}

Statements *Parser::statements() {
    // <statements> -> <statement> { <statement> }
    auto *parsedStatements = new Statements();
    parsedStatements->addStatement(statement());
    Token next = tokenizer.getToken();

    while (next.isIdentifier() || next.isKeyword()) {
        tokenizer.ungetToken();
        parsedStatements->addStatement(statement());
        next = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return parsedStatements;
}

Statement *Parser::statement() {
    // statement -> simple statement newline | compound statement
    Token token = tokenizer.getToken();

    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return compoundStatement();
    }

    Statement *simple = simpleStatement();
    Token newline = tokenizer.getToken();

    if (!newline.isNewline()) {
        delete simple;
        die("Parser::statement", "expected NEWLINE after statement", newline);
    }

    return simple;
}

Statement *Parser::simpleStatement() {
    Token token = tokenizer.getToken();

    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        return assignmentStatement();
    }
    
    if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        return printStatement();
    }

    die("Parser::statement", "expected a statement", token);
}

Statement *Parser::compoundStatement(){
    Token token = tokenizer.getToken();
    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return forStatement();
    }

    die("Parser::statement", "expected a for statement", token);
}

Statements *Parser::suite(){
    // <suite> -> NEWLINE INDENT <statements> DEDENT
}

AssignmentStatement *Parser::assignmentStatement() {
    // <assignment-statement> -> <id> = <rel-expr>
    // The caller consumes the context-dependent terminator: NEWLINE in a
    // statement list or ';' in a future for-loop header.
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier())
        die("Parser::assignmentStatement", "expected an identifier", variable);

    Token assignmentOperator = tokenizer.getToken();
    if (!assignmentOperator.isAssignmentOperator())
        die("Parser::assignmentStatement", "expected '='", assignmentOperator);

    return new AssignmentStatement(variable.identifier(), relExpr());
}

PrintStatement *Parser::printStatement() {
    // <print-statement> -> "print" <rel-expr>
    Token keyword = tokenizer.getToken();
    if (!keyword.isPrintKeyword()) {
        die("Parser::printStatement", "expected 'print'", keyword);
    }

    return new PrintStatement(relExpr());
}

ForStatement *Parser::forStatement() {
    // <for-statement> -> "for" ( <assign-statement> ; <rel-expr> ; <assign-statement> ) { NEWLINE <statements> }
    Token keyword = tokenizer.getToken();
    if (!keyword.isForKeyword()) {
        die("Parser::printStatement", "expected 'for'", keyword);
    }
    Token parenthesis = tokenizer.getToken();
    if (!parenthesis.isOpenParen()){
        die("Parser::forStatement", "expected '('", parenthesis);
    }

    AssignmentStatement *initializer = assignmentStatement();

    Token semiColon = tokenizer.getToken();
    if (!semiColon.isSemicolon()){
        die("Parser::forStatement", "expected ';'", semiColon);
    }

    ExprNode *forStatementCompare = relExpr();

    Token semiColon2 = tokenizer.getToken();
    if (!semiColon2.isSemicolon()){
        die("Parser::forStatement", "expected ';'", semiColon2);
    }

    AssignmentStatement *forStatementIncr = assignmentStatement();

    Token closedParenthesis = tokenizer.getToken();
    if (!closedParenthesis.isCloseParen()){
        die("Parser::forStatement", "expected ')'", closedParenthesis);
    }

    Token openBracket = tokenizer.getToken();
    if (!openBracket.isOpenBracket()) {
        die("Parser::forStatement", "expected '{'", openBracket);
    }

    Token newLine = tokenizer.getToken();
    if (!newLine.isNewline()) {
        die("Parser::forStatement", "expected 'NEWLINE'", newLine);
    }

    Statements *forloopStatements = statements();

    Token closedBracket = tokenizer.getToken();
    if (!closedBracket.isClosedBracket()) {
        die("Parser::forStatement", "expected '}'", closedBracket);
    }

    // for ( int a = 0; i < 10 ; i++ ) { statements }


    return new ForStatement(initializer, forStatementCompare, forStatementIncr, forloopStatements);
}

ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]
    // The optional equality operation is left for students to implement.

    ExprNode *left = relTerm();
    Token token = tokenizer.getToken();

    while (token.isEqualityOperator() || token.isNotEqualOperator()) {
        ExprNode *right = relTerm();
        return new BinaryExprNode(token, left, right);
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::relTerm() {
    // <rel-term> -> <rel-primary> [ <ordering-op> <rel-primary> ]
    // The optional ordering operation is left for students to implement.

    ExprNode *left = relPrimary();
    Token token = tokenizer.getToken();

    while (token.isLessThanOperator() || token.isLessThanOrEqualOperator() || token.isGreaterThanOperator() || token.isGreaterThanOrEqualOperator()) {
        ExprNode *right = relPrimary();
        return new BinaryExprNode(token, left, right);
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::relPrimary() {
    // <rel-primary> -> <arith-expr>
    return arithExpr();
}

ExprNode *Parser::arithExpr() {
    // <arith-expr> -> <arith-term> { <add-op> <arith-term> }
    ExprNode *left = arithTerm();
    Token token = tokenizer.getToken();

    while (token.isAdditionOperator() || token.isSubtractionOperator()) {
        ExprNode *right = arithTerm();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithTerm() {
    // <arith-term> -> <arith-primary> { <mult-op> <arith-primary> }
    ExprNode *left = arithPrimary();
    Token token = tokenizer.getToken();

    while (token.isMultiplicationOperator() ||
           token.isDivisionOperator() ||
           token.isModuloOperator()) {
        ExprNode *right = arithPrimary();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithPrimary() {
    // <arith-primary> -> [ <sign> ] <arith-atom>
    Token token = tokenizer.getToken();
    if (token.isAdditionOperator() || token.isSubtractionOperator())
        return new UnaryExprNode(token, arithAtom());

    tokenizer.ungetToken();
    return arithAtom();
}

ExprNode *Parser::arithAtom() {
    // <arith-atom> -> <id> | <integer> | '(' <rel-expr> ')'
    Token token = tokenizer.getToken();

    if (token.isInteger())
        return new IntegerLiteral(token);
    if (token.isIdentifier())
        return new Variable(token);
    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closeParen = tokenizer.getToken();
        if (!closeParen.isCloseParen())
            die("Parser::arithAtom", "expected ')'", closeParen);
        return expression;
    }

    die("Parser::arithAtom", "expected an identifier, integer, or '('", token);
}
