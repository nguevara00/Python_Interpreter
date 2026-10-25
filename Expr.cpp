#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <cmath>

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

    bool bothBooleans = leftBoolean && rightBoolean;
    bool bothInts = leftInt && rightInt;
    bool bothDoubles = leftDouble && rightDouble;
    bool bothStrings = leftString && rightString;

    const bool leftIntRightDouble = leftInt && rightDouble;
    const bool leftDoubleRightInt = leftDouble && rightInt;
    const bool leftIntRightString = leftInt && rightString;
    const bool rightIntLeftString = leftString && rightInt;
    const bool leftDoubleRightString = leftDouble && rightString;
    const bool rightDoubleLeftString = leftString && rightDouble;

    int64_t leftNum = 0;
    if (leftInt){
        leftNum = static_cast<const IntegerValueDescriptor*>(leftValue)->value();
    }

    int64_t rightNum = 0;
    if (rightInt){
        rightNum = static_cast<const IntegerValueDescriptor*>(rightValue)->value();
    }

    double leftDoubleNum = 0;
    if (leftDouble){
        leftDoubleNum = static_cast<const DoubleValueDescriptor*>(leftValue)->value();
    }

    double rightDoubleNum = 0;
    if (rightDouble){
        rightDoubleNum = static_cast<const DoubleValueDescriptor*>(rightValue)->value();
    }
    
    std::string leftStringActual = "";
    if (leftString){
        leftStringActual = static_cast<const StringValueDescriptor*>(leftValue)->toString();
    }

    std::string rightStringActual = "";
    if (rightString){
        rightStringActual = static_cast<const StringValueDescriptor*>(rightValue)->toString();
    }

    bool leftBoolValue = false;
    if (leftBoolean){
        leftBoolValue = static_cast<const BooleanValueDescriptor*>(leftValue)->value();
    }

    bool rightBoolValue = false;
    if (rightBoolean){
        leftBoolValue = static_cast<const BooleanValueDescriptor*>(rightValue)->value();
    }

    //both will be doubles
    if (leftIntRightDouble){
        leftDoubleNum = static_cast<const IntegerValueDescriptor*>(leftValue)->value();
        bothDoubles = true;
    }

    //both will be doubles
    if (leftDoubleRightInt){
        rightDoubleNum = static_cast<const IntegerValueDescriptor*>(rightValue)->value();
        bothDoubles = true;
    }

    if (token().isAdditionOperator()) {
        if (bothInts){
            return new IntegerValueDescriptor(leftNum + rightNum);
        } else if (bothDoubles){
            return new DoubleValueDescriptor(leftDoubleNum + rightDoubleNum);
        } else if (bothStrings){
            return new StringValueDescriptor(leftStringActual + rightStringActual);
        } else if (leftString || rightString){
            throw std::runtime_error("add number and string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber())); 
        }
    }

    if (token().isSubtractionOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum - rightNum);
        } else if (bothDoubles){
            return new DoubleValueDescriptor(leftDouble - rightDouble);
        }
    }

    if (token().isMultiplicationOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum * rightNum);
        } else if (bothDoubles) {
            return new DoubleValueDescriptor(leftDouble * rightDouble);
        }
    }

    if (token().isDivisionOperator() && !bothStrings) {
        if (leftInt){
            leftDoubleNum = static_cast<const IntegerValueDescriptor*>(leftValue)->value();
        }

        if (rightInt){
            rightDoubleNum = static_cast<const IntegerValueDescriptor*>(rightValue)->value();
        }
 
        if (rightValue == 0){
            throw std::runtime_error("division by zero at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));   
        }

        return new DoubleValueDescriptor(leftDoubleNum / rightDoubleNum);
    }


    if (token().isModuloOperator()){
        if (!bothInts) {
            throw std::runtime_error("modulo with non-integer operands at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));   
        }
        if (rightNum == 0){
            throw std::runtime_error("modulo by zero at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));  
        }
        return new IntegerValueDescriptor(leftNum % rightNum);

    }

    if (token().isFloorDivisionOperator()) {
        if (!bothInts) {
            throw std::runtime_error("floor division with non-integer operands at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));   
        }
        return new IntegerValueDescriptor(std::floor(static_cast<double>(leftInt) / rightInt));
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
            booleanResult = leftBoolean == rightBoolean;
        }

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean compared to non-boolean at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        return new BooleanValueDescriptor(booleanResult);
    }

    if (token().isNotEqualOperator()) {
        bool booleanResult = false;

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

        else if (leftString || rightString) {
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared at " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
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
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared at " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
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
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared at " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
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
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared at " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
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
            throw std::runtime_error("string compared to non-string at line " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }

        else if (leftBoolean || rightBoolean) {
            throw std::runtime_error("boolean cannot be compared at " + std::to_string(token().lineNumber()) + ", column " + std::to_string(token().columnNumber()));
        }
        return new BooleanValueDescriptor(booleanResult);
    }
    delete leftOperand;
    delete rightOperand;
    
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
    
    if(value->typeName() == "integer"){
        int64_t num = static_cast<IntegerValueDescriptor*>(value)->value();
        delete value;

        if (token().isAdditionOperator())  {
            return new IntegerValueDescriptor(num);
        }

        if (token().isSubtractionOperator()) {
            return new IntegerValueDescriptor(-num);
        }
    } 

    if(value->typeName() == "double"){
        double num = static_cast<DoubleValueDescriptor*>(value)->value();
        delete value;

        if (token().isAdditionOperator()) {
            return new DoubleValueDescriptor(num);
        }

        if (token().isSubtractionOperator()) {
            return new DoubleValueDescriptor(-num);
        }
     
    }
    throw std::logic_error("unsupported prefix operator");
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
    // creates a new descriptor 
    return new IntegerValueDescriptor(token().integerValue());
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

ValueDescriptor* Variable::evaluate(const SymbolTable &symbolTable) const {
   return (symbolTable.getValueFor(token().identifier()))->clone();
}
