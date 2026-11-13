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

    if (token.isForKeyword() || token.isIfKeyword() || token.isWhileKeyword()) {
        tokenizer.ungetToken();
        return compoundStatement();
    }

    tokenizer.ungetToken();
    Statement *simple = simpleStatement();
    Token newline = tokenizer.getToken();

    if (!newline.isNewline()) {
        delete simple;
        die("Parser::statement", "expected NEWLINE after statement", newline);
    }

    return simple;
}

Statement *Parser::simpleStatement() {
    // <simple-statement> -> <assignment-statement> | <print-statement> | <append-statement>
    Token token = tokenizer.getToken();

    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        return assignmentStatement();
    }
    
    if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        return printStatement();
    }

    if (token.isAppendKeyword()){
        tokenizer.ungetToken();
        return appendStatement();
    }

    die("Parser::statement", "expected a statement", token);
}

Statement *Parser::compoundStatement(){
    // <compound-statement> -> <for-statement> | <if-statement> | <while-statement>

    Token token = tokenizer.getToken();
    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return forStatement();
    }
    if (token.isIfKeyword()){
        tokenizer.ungetToken();
        return ifStatement();
    }
    if (token.isWhileKeyword()){
        tokenizer.ungetToken();
        return whileStatement();
    }

    die("Parser::statement", "expected a for statement", token);
}

Statements *Parser::suite(){
    // <suite> -> NEWLINE INDENT <statements> DEDENT
    Token newLineToken = tokenizer.getToken();
    if (!newLineToken.isNewline()){
        die("Parser::suite", "expected a NEWLINE", newLineToken);
    }

    Token indentToken = tokenizer.getToken();
    if (!indentToken.isIndent()){
        die("Parser::suite", "expected an INDENT", indentToken);
    }

    Statements *parsedStatements = statements();
    
    Token dedentToken = tokenizer.getToken();
    if (!dedentToken.isDedent()) {
        delete parsedStatements;
        die("Parser::suite", "expected an DEDENT", dedentToken);
    }
    // check for dedent, if no dedent die. 
    return parsedStatements;

}


AssignmentStatement *Parser::assignmentStatement() {
    // <assignment-statement> -> <assignment-target> = <rel-expr>
    AssignmentTarget* target = assignmentTarget();

    Token assignmentOperator = tokenizer.getToken();
    if (!assignmentOperator.isAssignmentOperator())
        die("Parser::assignmentStatement", "expected '='", assignmentOperator);
    
    ExprNode* expression = relExpr();

    return new AssignmentStatement(target, expression);
}

AssignmentTarget* Parser::assignmentTarget() {
    // <assignment-target> -> <id> [ "[" <rel-expr> "]" ]
    
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier()) {
        die("Parser::assignmentTarget", "expected an identifier", variable);
    }

    ExprNode* expression = nullptr;
    
    Token openBracket = tokenizer.getToken();
    if (openBracket.isOpenBracket()){
        expression = relExpr();
        
        Token closedBracket = tokenizer.getToken();
        if (!closedBracket.isClosedBracket()) {
            die("Parser::assignmentTarget", "expected ']'", closedBracket);
        }
    } else {
        tokenizer.ungetToken();
    }
    
    return new AssignmentTarget(variable.identifier(), expression);
}

PrintStatement *Parser::printStatement() {
    // <print-statement> -> "print" <rel-expr>
    Token keyword = tokenizer.getToken();
    if (!keyword.isPrintKeyword()) {
        die("Parser::printStatement", "expected 'print'", keyword);
    }

    return new PrintStatement(relExpr());
}

AppendStatement* Parser::appendStatement(){
    //<append-statement> -> "append" "(" <rel-expr> "," <rel-expr> ")"
    Token keyword = tokenizer.getToken();
    if (!keyword.isAppendKeyword()) {
        die("Parser::appendStatement", "expected 'append'", keyword);
    }

    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen()){
        die("Parser::appendStatement", "expected '('", openParen);
    }

    ExprNode* first = relExpr();

    Token comma = tokenizer.getToken();
    if (!comma.isComma()) {
        tokenizer.ungetToken();
        die("Parser:appendStatement", "expected ','", comma);
    }

    ExprNode* second = relExpr();

    Token closedParen = tokenizer.getToken();
    if (!closedParen.isClosedParen()) {
        die("Parser::rangeExpression", "expected ')'", closedParen);
    }

    return new AppendStatement(first, second);
}
RangeExpression *Parser::rangeExpression() {
    // <range> -> "range" "(" <range-arguments> ")"
    Token keyword = tokenizer.getToken();
    if (!keyword.isRangeKeyword()) {
        die("Parser::rangeExpression", "expected 'range'", keyword);
    }

    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen()) {
        die("Parser::rangeExpression", "expected '('", openParen);
    }

    RangeExpression *range = rangeArguments();

    Token closedParen = tokenizer.getToken();
    if (!closedParen.isClosedParen()) {
        die("Parser::rangeExpression", "expected ')'", closedParen);
    }

    return range;
}

RangeExpression *Parser::rangeArguments(){
    // <range-arguments> -> <rel-expr> | <rel-expr> "," <rel-expr> | <rel-expr> "," <rel-expr> "," <rel-expr>
    ExprNode *first = relExpr();

    Token comma = tokenizer.getToken();
    if (!comma.isComma()) {
        tokenizer.ungetToken();
        return new RangeExpression(first);
    }

    ExprNode *second = relExpr();

    Token secondComma = tokenizer.getToken();
    if (!secondComma.isComma()) {
        tokenizer.ungetToken();
        return new RangeExpression(first, second);
    }

    ExprNode *third = relExpr();
    return new RangeExpression(first, second, third);
}

ForStatement *Parser::forStatement() {
    // <for-statement> -> "for" <id> "in" <range> : <suite>

    Token forToken = tokenizer.getToken();
    if (!forToken.isForKeyword()) {
        die("Parser::forStatement", "expected 'for'", forToken);
    }

    Token id = tokenizer.getToken();
    if (!id.isIdentifier()) {
        die("Parser::forStatement", "expected an identifier", id);
    }

    Token inToken = tokenizer.getToken();
    if (!inToken.isInKeyword()) {
        die("Parser::forStatement", "expected 'in'", inToken);
    }

    RangeExpression *range = rangeExpression();

    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()) {
        die("Parser::forStatement", "expected ':' ", colonToken);
    }

    Statements *statementsSuite = suite();

    return new ForStatement(id.identifier(), range, statementsSuite);
}

IfStatement* Parser::ifStatement(){
    //<if-statement> -> "if" <rel-expr> ":" <suite> { "elif" <rel-expr> ":" <suite> } [ "else" ":" <suite> ]
    
    Token ifToken = tokenizer.getToken();
    if (!ifToken.isIfKeyword()){
        die("Parser::ifStatement", "expected 'if'", ifToken);
    }

    std::vector<ExprNode*> conditions;
    std::vector<Statements*> suites;

    conditions.push_back(relExpr());

    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()){
        die("Parser::ifStatement", "expected ':'", ifToken);
    }
    
    suites.push_back(suite());

    Token elifToken = tokenizer.getToken();
    while (elifToken.isElifKeyword()){
        conditions.push_back(relExpr());
        colonToken = tokenizer.getToken();
        if (!colonToken.isColon()){
            die("Parser::ifStatement", "expected ':'", colonToken);
        }
        suites.push_back(suite());
        elifToken = tokenizer.getToken();
    }

    Statements* elseSuite = nullptr;
    if (elifToken.isElseKeyword()){
        colonToken = tokenizer.getToken();
        if (!colonToken.isColon()){
            die("Parser::ifStatement", "expected ':'", colonToken);
        }
        elseSuite = suite();
    } else {
        tokenizer.ungetToken();
    }

    return new IfStatement(conditions, suites, elseSuite);
}

WhileStatement* Parser::whileStatement(){
    //<while-statement> -> "while" <rel-expr> ":" <suite>
    Token whileToken = tokenizer.getToken();
    if (!whileToken.isWhileKeyword()){
        die("Parser::whileStatement", "expected 'while'", whileToken);
    }
    ExprNode* whileCondition = relExpr();

    Token colonToken = tokenizer.getToken();
    if (!colonToken.isColon()){
        die("Parser::whileStatement", "expected ':'", colonToken);
    }
    Statements* whileSuite = suite();
    return new WhileStatement(whileCondition,whileSuite);
}

ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]

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
           token.isModuloOperator() ||
           token.isFloorDivisionOperator()) {
        ExprNode *right = arithPrimary();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}


ExprNode *Parser::arithPrimary() {
    // <arith-primary> -> [ <sign> ] <postfix-expression>

    Token token = tokenizer.getToken();
    if (token.isAdditionOperator() || token.isSubtractionOperator()) {
        return new UnaryExprNode(token, postfixExpression());
    }

    tokenizer.ungetToken();
    return postfixExpression();
}

ExprNode* Parser::postfixExpression() {
    // <postfix-expression> -> <atom> [ "[" <rel-expr> "]" ]
    ExprNode* atom = arithAtom();

    Token openBracket = tokenizer.getToken();
    if (!openBracket.isOpenBracket()){
        tokenizer.ungetToken();
        return new PostfixExpression(atom->token(), atom, nullptr);
    }

    ExprNode* indexExpression = relExpr();

    Token closedBracket = tokenizer.getToken();
    if (!closedBracket.isClosedBracket()) {
        die("Parser::postfixExpression", "expected ']'", closedBracket);
    }

    return new PostfixExpression(atom->token(), atom, indexExpression);

}

// <atom> -> <id> | <literal> | <list-literal> | <input-expression> | <one-argument-call> | "(" <rel-expr> ")"
ExprNode *Parser::arithAtom() {
    Token token = tokenizer.getToken();
    
    if (token.isIdentifier()) {
        return new Variable(token);
    }

    //literals
    if (token.isInteger() || token.isDouble() || token.isString() || token.isBoolean()) {
        tokenizer.ungetToken();
        return literal();
    }

    //list literals
    if (token.isOpenBracket()){
        tokenizer.ungetToken();
        return listLiteral();
    } 

    //input
    if (token.isInputKeyword()){
        tokenizer.ungetToken();
        return input();
    } 

    //one argument calls
    if (token.isIntKeyword() || token.isFloatKeyword() || token.isLenKeyword() || token.isSplitKeyword() || token.isPopKeyword() || token.isStrKeyword()) {
        tokenizer.ungetToken();
        return oneArgumentCall();
    }
    
    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen()) {
            die("Parser::arithAtom", "expected ')'", closedParen);
        }
        return expression;
    }

    die("Parser::arithAtom", "expected an identifier, literal, [, input, oneArgumentCall, or (", token);
}

ExprNode *Parser::literal(){
    // <literal> -> <integer> | <double> | <boolean> | <string>

    Token token = tokenizer.getToken();
    if (token.isInteger())
        return new IntegerLiteral(token);

    if (token.isDouble()) {
        return new DoubleLiteral(token);
    }

    if (token.isBoolean()) {
        return new BooleanLiteral(token);
       
    }
    if (token.isString()) {
        return new StringLiteral(token);
    }

    die("Parser::literal", "expected a literal", token);
}

ExprNode* Parser::listLiteral(){
    //<list-literal> -> [ "[" <rel-expr> { "," <rel-expr> } "]" ]
    Token openBracket = tokenizer.getToken();
    if (!openBracket.isOpenBracket()) {
        die("Parser::listLiteral", "expected a open bracket", openBracket);
    }

    std::vector<ExprNode*> expressions;

    Token closedBracket = tokenizer.getToken();
    if (closedBracket.isClosedBracket()) {
        return new ListLiteral(openBracket, expressions);
    }
    tokenizer.ungetToken();

    expressions.push_back(relExpr());

    Token commaToken = tokenizer.getToken();
    while (commaToken.isComma()) {
        expressions.push_back(relExpr());
        commaToken = tokenizer.getToken();
    }
    tokenizer.ungetToken();

    closedBracket = tokenizer.getToken();
    if (!closedBracket.isClosedBracket()) {
        die("Parser::listLiteral", "expected a closed bracket", closedBracket);
    }

    return new ListLiteral(openBracket, expressions);
}

ExprNode *Parser::stringConversion(){
    // <string-converstion> -> "str" '(' <rel-expr> ')'
    Token strToken = tokenizer.getToken();
    
    if (!strToken.isStrKeyword()) {
        die("Parser::strConversion", "expected 'str'", strToken);
    }

    Token openToken = tokenizer.getToken(); 
    if (openToken.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen())
            die("Parser::strConversion", "expected ')'", closedParen);

        return new StringConversionExpression(strToken, expression);
    }
    tokenizer.ungetToken();
    die("Parser::strConversion", "expected '('", openToken);
}

ExprNode* Parser::intConversion(){
//<int-converstion> -> "int" "(" <rel-expr> ")"
    Token intToken = tokenizer.getToken();
        
    if (!intToken.isIntKeyword()) {
        die("Parser::intConversion", "expected 'int'", intToken);
    }

    Token openToken = tokenizer.getToken(); 
    if (openToken.isOpenParen()) {
        //check if the expression is a string or it is a int
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen())
            die("Parser::intConversion", "expected ')'", closedParen);

        return new IntConversionExpression(intToken, expression);
    }
    tokenizer.ungetToken();
    die("Parser::intConversion", "expected '('", openToken);
    }

ExprNode* Parser::floatConversion(){
//<float-converstion> -> "float" "(" <rel-expr> ")"
    Token floatToken = tokenizer.getToken();
    
    if (!floatToken.isFloatKeyword()) {
        die("Parser::floatConversion", "expected 'float'", floatToken);
    }

    Token openToken = tokenizer.getToken(); 
    if (openToken.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen())
            die("Parser::strConversion", "expected ')'", closedParen);

        return new FloatConversionExpression(floatToken, expression);
    }
    tokenizer.ungetToken();
    die("Parser::strConversion", "expected '('", openToken);
}

ExprNode* Parser::input(){
    //<input-expression> -> "input" "(" ")"
    Token inputToken = tokenizer.getToken();
        
    if (!inputToken.isInputKeyword()) {
        die("Parser::inputConversion", "expected 'input'", inputToken);
    }

    Token openToken = tokenizer.getToken(); 
    if (!openToken.isOpenParen())
        die("Parser::strConversion", "expected '('", openToken);
        
    Token closedParen = tokenizer.getToken();
    if (!closedParen.isClosedParen())
        die("Parser::strConversion", "expected ')'", closedParen);

    return new InputExpression(inputToken);
    }

ExprNode* Parser::len(){
//<len> -> "len" "(" <rel-expr> ")"
    Token lenToken = tokenizer.getToken();

    if (!lenToken.isLenKeyword()) {
        die("Parser::len", "expected 'len'", lenToken);
    }

    Token openToken = tokenizer.getToken();
    if (openToken.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen())
            die("Parser::len", "expected ')'", closedParen);

        return new LenExpression(lenToken, expression);
    }
    tokenizer.ungetToken();
    die("Parser::len", "expected '('", openToken);
}

ExprNode* Parser::split(){
//<split> -> "split" "(" <rel-expr> ")"
    Token splitToken = tokenizer.getToken();

    if (!splitToken.isSplitKeyword()) {
        die("Parser::split", "expected 'split'", splitToken);
    }

    Token openToken = tokenizer.getToken();
    if (openToken.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closedParen = tokenizer.getToken();
        if (!closedParen.isClosedParen())
            die("Parser::split", "expected ')'", closedParen);

        return new SplitExpression(splitToken, expression);
    }
    tokenizer.ungetToken();
    die("Parser::split", "expected '('", openToken);

}

ExprNode* Parser::pop(){
//<pop> -> "pop" "(" <rel-expr> ")"
    Token popToken = tokenizer.getToken();

    if (!popToken.isPopKeyword()) {
        die("Parser::pop", "expected 'pop'", popToken);
    }

    Token openToken = tokenizer.getToken();
    if (!openToken.isOpenParen()) {
        die("Parser::pop", "expected '('", openToken);
    }

    ExprNode* expression = relExpr();

    Token closedParen = tokenizer.getToken();
    if (!closedParen.isClosedParen()) {
        die("Parser::pop", "expected ')'", closedParen);
    }

    return new PopExpression(popToken, expression);
}



// <one-argument-name> -> "int" | "float" | "str" | "len" | "split" | "pop"
Token Parser::oneArgumentName(){
    Token name = tokenizer.getToken();

    if (name.isIntKeyword() || name.isFloatKeyword() || name.isStrKeyword() || name.isLenKeyword() || name.isSplitKeyword() || name.isPopKeyword()) {
        return name;
    }

    die("Parser::oneArgumentName", "expected a one-argument name", name);
}

// <one-argument-call> -> <one-argument-name> "(" <rel-expr> ")"
ExprNode* Parser::oneArgumentCall(){
    Token name = oneArgumentName();

    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen()) {
        die("Parser::oneArgumentCall", "expected '('", openParen);
    }

    ExprNode *expression = relExpr();

    Token closedParen = tokenizer.getToken();
    if (!closedParen.isClosedParen()) {
        die("Parser::oneArgumentCall", "expected ')'", closedParen);
    }

    if (name.isIntKeyword())
        return new IntConversionExpression(name, expression);

    if (name.isFloatKeyword())
        return new FloatConversionExpression(name, expression);

    if (name.isStrKeyword())
        return new StringConversionExpression(name, expression);

    if (name.isLenKeyword())
        return new LenExpression(name, expression);

    if (name.isSplitKeyword())
        return new SplitExpression(name, expression);

    if (name.isPopKeyword())
        return new PopExpression(name, expression);

    die("Parser::oneArgumentCall", "invalid one-argument name", name);
}