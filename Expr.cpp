#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Expr.hpp"

ExprNode::ExprNode(Token token) : expressionToken{std::move(token)} {}

const Token &ExprNode::token() const {
    return expressionToken;
}

BinaryExprNode::BinaryExprNode(Token token, ExprNode *left, ExprNode *right)
    : ExprNode{std::move(token)},
      leftOperand{left},
      rightOperand{right} {}

BinaryExprNode::~BinaryExprNode() {
    delete leftOperand;
    delete rightOperand;
}

ValueDescriptor* BinaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const ValueDescriptor* leftValue = leftOperand->evaluate(symbolTable);
    const ValueDescriptor* rightValue = rightOperand->evaluate(symbolTable);

    const bool leftInt = leftValue->type() == ValueDescriptor::Type::integer;
    const bool rightInt = rightValue->type() == ValueDescriptor::Type::integer;
    const bool leftDouble = leftValue->type() == ValueDescriptor::Type::doubleValue;
    const bool rightDouble = rightValue->type() == ValueDescriptor::Type::doubleValue;
    const bool leftString = leftValue->type() == ValueDescriptor::Type::string;
    const bool rightString = rightValue->type() == ValueDescriptor::Type::string;
    const bool leftBoolean = leftValue->type() == ValueDescriptor::Type::boolean;
    const bool rightBoolean = rightValue->type() == ValueDescriptor::Type::boolean;

    const bool bothBooleans = leftBoolean && rightBoolean;
    const bool bothInts = leftInt && rightInt;
    const bool bothStrings = leftString && rightString;
    const bool bothDoubles = (leftDouble && rightDouble) || (leftInt && rightDouble) || (leftDouble && rightInt);

    int64_t leftNum = 0;    
    int64_t rightNum = 0;
    double leftDoubleNum = 0;
    double rightDoubleNum = 0;
    std::string leftStringActual = "";
    std::string rightStringActual = "";
    bool leftBoolValue = false;
    bool rightBoolValue = false;

    const std::string leftTypeName = leftValue->typeName();
    const std::string rightTypeName = rightValue->typeName();
    const std::string location = " at line " + std::to_string(token().lineNumber()) +", column " + std::to_string(token().columnNumber());

    if (leftInt){
        leftNum = static_cast<const IntegerValueDescriptor*>(leftValue)->value();
        leftDoubleNum = static_cast<double>(leftNum);
    } else if (leftDouble) {
        leftDoubleNum = static_cast<const DoubleValueDescriptor*>(leftValue)->value();
    }

    if (rightInt){
        rightNum = static_cast<const IntegerValueDescriptor*>(rightValue)->value();
        rightDoubleNum = static_cast<double>(rightNum);
    } else if (rightDouble){
        rightDoubleNum = static_cast<const DoubleValueDescriptor*>(rightValue)->value();
    }
    
    
    if (leftString){
        leftStringActual = static_cast<const StringValueDescriptor*>(leftValue)->toString();
    }

    if (rightString){
        rightStringActual = static_cast<const StringValueDescriptor*>(rightValue)->toString();
    }

    if (leftBoolean){
        leftBoolValue = static_cast<const BooleanValueDescriptor*>(leftValue)->value();
    }

    if (rightBoolean){
        rightBoolValue = static_cast<const BooleanValueDescriptor*>(rightValue)->value();
    }

    //deletes here -leftValue and rightValue have all their values extracted and are no longer needed
    delete leftValue;
    delete rightValue;

    if (token().isAdditionOperator()) {
        if (bothInts){
            return new IntegerValueDescriptor(leftNum + rightNum);
        }
        if (bothDoubles){
            return new DoubleValueDescriptor(leftDoubleNum + rightDoubleNum);
        }
        if (bothStrings){
            return new StringValueDescriptor(leftStringActual + rightStringActual);
        }

        throw std::runtime_error("operator + cannot be applied to " + leftTypeName + "and " + rightTypeName + "at " + location);
    }

    if (token().isSubtractionOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum - rightNum);
        }
        if (bothDoubles){
            return new DoubleValueDescriptor(leftDoubleNum - rightDoubleNum);
        }
        throw std::runtime_error("operator - cannot be applied to " + leftTypeName + "and " + rightTypeName + "at " + location);
    }

    if (token().isMultiplicationOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum * rightNum);
        } 
        if (bothDoubles) {
            return new DoubleValueDescriptor(leftDoubleNum * rightDoubleNum);
        }
        throw std::runtime_error("operator * cannot be applied to " + leftTypeName + "and " + rightTypeName + "at " + location);

    }

    if (token().isDivisionOperator()) {
        if (!bothInts && !bothDoubles){
            throw std::runtime_error("division with non-numeric operands " + leftTypeName + "and " + rightTypeName + location);  
        }
        if (rightDoubleNum == 0){
            throw std::runtime_error("division by zero at line " + location);   
        }
        return new DoubleValueDescriptor(leftDoubleNum / rightDoubleNum);
    }


    if (token().isModuloOperator()){
        if (!bothInts) {
            throw std::runtime_error("modulo with non-integer operands" + leftTypeName + "and " + rightTypeName + location);   
        }
        if (rightNum == 0){
            throw std::runtime_error("modulo by zero " + location);  
        }
        //fixing modulo for python 
        int64_t remainder = leftNum % rightNum;
        if (remainder != 0 && ((leftNum < 0) != (rightNum < 0))){
            remainder += rightNum;
        }

        return new IntegerValueDescriptor(remainder);

    }

    if (token().isFloorDivisionOperator()) {
        if (!bothInts) {
            throw std::runtime_error("floor division with non-integer operands " + leftTypeName +", " + rightTypeName + location);   
        }
        if (rightNum == 0){
            throw std::runtime_error("floor division by zero " + location);  
        }
        //fixing python style floor division
        int64_t quotient = leftNum / rightNum;
        int64_t remainder = leftNum % rightNum;
        if (remainder != 0 && ((leftNum < 0) != (rightNum < 0))){
            --quotient;
        }
        return new IntegerValueDescriptor(quotient);
    }

    if (token().isEqualityOperator()) {
        bool booleanResult = false;

        if (bothInts) {
            booleanResult = leftNum == rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum == rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual == rightStringActual;
        }

        else if (bothBooleans) {
            booleanResult = leftBoolValue == rightBoolValue;
        }

        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isNotEqualOperator()) {
        bool booleanResult = true;

        if (bothInts) {
            booleanResult = leftNum != rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum != rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual != rightStringActual;
        }

        else if (bothBooleans) {
            booleanResult = leftBoolValue != rightBoolValue;
        }

        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isGreaterThanOperator()) {
        bool booleanResult = false;

        if (bothInts) {
            booleanResult = leftNum > rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum > rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual > rightStringActual;
        }

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string " + location);
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared " + location);
        }
        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isGreaterThanOrEqualOperator()) {
        bool booleanResult = false;

        if (bothInts) {
            booleanResult = leftNum >= rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum >= rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual >= rightStringActual;
        }

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string" + location);
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared" + location);
        }
        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isLessThanOperator()) {
        bool booleanResult = false;

        if (bothInts) {
            booleanResult = leftNum < rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum < rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual < rightStringActual;
        }

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string" + location);
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared" + location);
        }
        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isLessThanOrEqualOperator()) {
        bool booleanResult = false;

        if (bothInts) {
            booleanResult = leftNum <= rightNum;
        }

        else if (bothDoubles){
            booleanResult = leftDoubleNum <= rightDoubleNum;
        }

        else if (bothStrings) {
            booleanResult = leftStringActual <= rightStringActual;
        }

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string" + location);
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared" + location);
        }
        return new BooleanValueDescriptor(booleanResult);
    }

    throw std::logic_error("unsupported infix operator");
}

void BinaryExprNode::print() const {
    std::cout << '(';
    leftOperand->print();
    token().print(std::cout);
    rightOperand->print();
    std::cout << ')';
}

UnaryExprNode::UnaryExprNode(Token token, ExprNode *operand)
    : ExprNode{std::move(token)}, expression{operand} {}

UnaryExprNode::~UnaryExprNode() {
    delete expression;
}

ValueDescriptor* UnaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = expression->evaluate(symbolTable);
    bool isInteger = value->type() == ValueDescriptor::Type::integer;
    bool isDouble = value->type() == ValueDescriptor::Type::doubleValue;
    int64_t num = 0;
    double dblNum = 0.0;
    std::string valueTypeName = value->typeName();

    if(isInteger){
        num = static_cast<IntegerValueDescriptor*>(value)->value();
    }else if (isDouble){
        dblNum = static_cast<DoubleValueDescriptor*>(value)->value();
    }
    
    delete value;

    if (isInteger) {
        if (token().isAdditionOperator())  {
            return new IntegerValueDescriptor(num);
        }

        if (token().isSubtractionOperator()) {
            return new IntegerValueDescriptor(-num);
        }
    } 

    if(isDouble) {
        if (token().isAdditionOperator()) {
            return new DoubleValueDescriptor(dblNum);
        }

        if (token().isSubtractionOperator()) {
            return new DoubleValueDescriptor(-dblNum);
        }
    }
    throw std::runtime_error("unary operator cannot be applied to " + valueTypeName
);
}

void UnaryExprNode::print() const {
    std::cout << token().symbol();
    expression->print();
}

IntegerLiteral::IntegerLiteral(Token token) : ExprNode{std::move(token)} {}

void IntegerLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor* IntegerLiteral::evaluate(const SymbolTable &) const {
    return new IntegerValueDescriptor(token().integerValue());
}

//double
DoubleLiteral::DoubleLiteral(Token token) : ExprNode{std::move(token)} {}

void DoubleLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor* DoubleLiteral::evaluate(const SymbolTable &) const {

    return new DoubleValueDescriptor(token().doubleValue());
}

//string
StringLiteral::StringLiteral(Token token) : ExprNode{std::move(token)} {}

void StringLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor* StringLiteral::evaluate(const SymbolTable &) const {
    return new StringValueDescriptor(token().stringValue());
}

//boolean
BooleanLiteral::BooleanLiteral(Token token) : ExprNode{std::move(token)} {}

void BooleanLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor* BooleanLiteral::evaluate(const SymbolTable &) const {
    return new BooleanValueDescriptor(token().booleanValue());
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

ValueDescriptor* Variable::evaluate(const SymbolTable &symbolTable) const {
   return (symbolTable.getValueFor(token().identifier()))->clone();
}

StringConversionExpression::StringConversionExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

StringConversionExpression::~StringConversionExpression(){
    delete _expression;
}

//added back in formatting
void StringConversionExpression::print() const {
    std::cout << "str(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* StringConversionExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = _expression->evaluate(symbolTable);
    std::string result = value->toString();
    
    delete value;
    return new StringValueDescriptor(result);
}
