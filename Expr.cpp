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

    //added functionality for comparison for list equality
    if (token().isEqualityOperator() || token().isNotEqualOperator()){
        bool equal = false;

        if (bothInts) {
            equal = static_cast<const IntegerValueDescriptor*>(leftValue)->value() == static_cast<const IntegerValueDescriptor*>(rightValue)->value();
        } else if (bothDoubles){
            double leftNumber;
            double rightNumber;

            if (leftInt){
                leftNumber = static_cast<double>(static_cast<const IntegerValueDescriptor*>(leftValue)->value());
            } else{
                leftNumber = static_cast<const DoubleValueDescriptor*>(leftValue)->value();
            }

            if (rightInt){
                rightNumber = static_cast<double>(static_cast<const IntegerValueDescriptor*>(rightValue)->value());
            } else{
                rightNumber = static_cast<const DoubleValueDescriptor*>(rightValue)->value();
            }

            equal = leftNumber == rightNumber;
        } else if (bothStrings){
            equal = static_cast<const StringValueDescriptor *>(leftValue)->value() == static_cast<const StringValueDescriptor *>(rightValue)->value();
        } else if (bothBooleans){
            equal = static_cast<const BooleanValueDescriptor *>(leftValue)->value() == static_cast<const BooleanValueDescriptor *>(rightValue)->value();
        } else if (leftValue->type() == ValueDescriptor::Type::list && rightValue->type() == ValueDescriptor::Type::list) {
            const ListObject *leftList = static_cast<const ListValueDescriptor *>(leftValue)->object();
            const ListObject *rightList = static_cast<const ListValueDescriptor *>(rightValue)->object();

            if (leftList->size() == rightList->size()) {
                if (leftList->size() == 0) {
                    equal = true;
                } else if (leftList->elementType() == rightList->elementType()) {
                    equal = true;

                    for (std::size_t i = 0; i < leftList->size(); ++i) {
                        ValueDescriptor *leftElement = leftList->cloneElementAt(i);
                        ValueDescriptor *rightElement = rightList->cloneElementAt(i);
                        bool elementsEqual = false;

                        bool leftElementInteger = leftElement->type() == ValueDescriptor::Type::integer;
                        bool rightElementInteger = rightElement->type() == ValueDescriptor::Type::integer;
                        bool leftElementDouble = leftElement->type() == ValueDescriptor::Type::doubleValue;
                        bool rightElementDouble = rightElement->type() == ValueDescriptor::Type::doubleValue;

                        if (leftElementInteger && rightElementInteger) {
                            elementsEqual = static_cast<IntegerValueDescriptor *>(leftElement)->value() == static_cast<IntegerValueDescriptor *>(rightElement)->value();
                        } else if ((leftElementInteger || leftElementDouble) && (rightElementInteger || rightElementDouble)) {
                            double leftNumber;
                            double rightNumber;

                            if (leftElementInteger){
                                leftNumber = static_cast<double>(static_cast<IntegerValueDescriptor *>(leftElement)->value());
                            } else {
                                leftNumber = static_cast<DoubleValueDescriptor *>(leftElement)->value();
                            }

                            if (rightElementInteger) {
                                rightNumber = static_cast<double>(static_cast<IntegerValueDescriptor *>(rightElement)->value());
                            } else {
                            rightNumber = static_cast<DoubleValueDescriptor *>(rightElement)->value();
                            }

                            elementsEqual = leftNumber == rightNumber;

                        } else if (leftElement->type() == ValueDescriptor::Type::string && rightElement->type() == ValueDescriptor::Type::string) {
                            elementsEqual = static_cast<StringValueDescriptor *>(leftElement)->value() == static_cast<StringValueDescriptor *>(rightElement)->value();
                        } else if (leftElement->type() == ValueDescriptor::Type::boolean && rightElement->type() == ValueDescriptor::Type::boolean) {
                            elementsEqual = static_cast<BooleanValueDescriptor *>(leftElement)->value() == static_cast<BooleanValueDescriptor *>(rightElement)->value();
                        }

                        delete leftElement;
                        delete rightElement;

                        if (!elementsEqual) {
                            equal = false;
                            break;
                        }
                    }
                }
            }
        }
        delete leftValue;
        delete rightValue;

        if (token().isNotEqualOperator()){
            equal = !equal;
        }

        return new BooleanValueDescriptor(equal);
    } 

    int64_t leftNum = 0;    
    int64_t rightNum = 0;
    double leftDoubleNum = 0;
    double rightDoubleNum = 0;
    std::string leftStringActual = "";
    std::string rightStringActual = "";

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

        throw std::runtime_error("operator + cannot be applied to " + leftTypeName + " and " + rightTypeName + "at " + location);
    }

    if (token().isSubtractionOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum - rightNum);
        }
        if (bothDoubles){
            return new DoubleValueDescriptor(leftDoubleNum - rightDoubleNum);
        }
        throw std::runtime_error("operator - cannot be applied to " + leftTypeName + " and " + rightTypeName + "at " + location);
    }

    if (token().isMultiplicationOperator() && !bothStrings){
        if (bothInts){
            return new IntegerValueDescriptor(leftNum * rightNum);
        } 
        if (bothDoubles) {
            return new DoubleValueDescriptor(leftDoubleNum * rightDoubleNum);
        }
        throw std::runtime_error("operator * cannot be applied to " + leftTypeName + " and " + rightTypeName + "at " + location);

    }

    if (token().isDivisionOperator()) {
        if (!bothInts && !bothDoubles){
            throw std::runtime_error("division with non-numeric operands " + leftTypeName + " and " + rightTypeName + location);  
        }
        if (rightDoubleNum == 0){
            throw std::runtime_error("division by zero at line " + location);   
        }
        return new DoubleValueDescriptor(leftDoubleNum / rightDoubleNum);
    }


    if (token().isModuloOperator()){
        if (!bothInts) {
            throw std::runtime_error("modulo with non-integer operands" + leftTypeName + " and " + rightTypeName + location);   
        }
        if (rightNum == 0){
            throw std::runtime_error("modulo by zero " + location);  
        }
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
    throw std::runtime_error("unary operator cannot be applied to " + valueTypeName);
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

//intConversionExpression
IntConversionExpression::IntConversionExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

IntConversionExpression::~IntConversionExpression(){
    delete _expression;
}

void IntConversionExpression::print() const {
    std::cout << "int(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* IntConversionExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = _expression->evaluate(symbolTable);
    //what if value int?
    if (value->type() == ValueDescriptor::Type::integer){
        
        IntegerValueDescriptor* intValue = static_cast<IntegerValueDescriptor*>(value);
        int result = intValue->value();
        delete value;
        return new IntegerValueDescriptor(result);
    }

    //what if valuetype is string?
    if (value->type() == ValueDescriptor::Type::string){
        StringValueDescriptor* stringValue = static_cast<StringValueDescriptor*>(value);
        int64_t result = std::stoi(stringValue->value());
        delete value;
        return new IntegerValueDescriptor(result);
    }
    
    std::string type = value->typeName();
    delete value;
    throw std::runtime_error("IntConversion cannot convert " + type + " to integer");
}

//FloatConversionExpression
FloatConversionExpression::FloatConversionExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

FloatConversionExpression::~FloatConversionExpression(){
    delete _expression;
}

void FloatConversionExpression::print() const {
    std::cout << "float(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* FloatConversionExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = _expression->evaluate(symbolTable);

    //what if double?
    if (value->type() == ValueDescriptor::Type::doubleValue){
        
        DoubleValueDescriptor* doubleValue = static_cast<DoubleValueDescriptor*>(value);
        double result = doubleValue->value();
        delete value;
        return new DoubleValueDescriptor(result);
    }

    //what if value int?
    if (value->type() == ValueDescriptor::Type::integer){
        
        DoubleValueDescriptor* doubleValue = static_cast<DoubleValueDescriptor*>(value);
        double result = doubleValue->value();
        delete value;
        return new DoubleValueDescriptor(result);
    }

    //what if valuetype is string?
    if (value->type() == ValueDescriptor::Type::string){
        StringValueDescriptor* stringValue = static_cast<StringValueDescriptor*>(value);
        double result = std::stof(stringValue->value());
        delete value;
        return new DoubleValueDescriptor(result);
    }
    
    std::string type = value->typeName();
    delete value;
    throw std::runtime_error("DoubleConversion cannot convert " + type + " to integer");
}

//InputExpression
InputExpression::InputExpression(Token token) : ExprNode {std::move(token)}{}

InputExpression::~InputExpression(){
}

void InputExpression::print() const {
    token().print(std::cout);
}

ValueDescriptor* InputExpression::evaluate(const SymbolTable &) const {
//idk

    std::string string;
    //if line is bad, runtime error
    if (!std::getline(std::cin,string)){
        throw std::runtime_error("Reached end of file while waiting for input on line number " + std::to_string(token().lineNumber()) + ", column number " + std::to_string(token().columnNumber()));
    }
    return new StringValueDescriptor(string);
}

ListLiteral::~ListLiteral() {
    for (ExprNode* expression : _expressions) {
        delete expression;
    }
}

void ListLiteral::print() const {
    std::cout << "[";
    for (size_t i = 0; i < _expressions.size(); i++) {
        if (i != _expressions.size() - 1) {
            _expressions[i]->print();
            std::cout << ", ";
        } else {
            _expressions[i]->print();
        }
    }
    std::cout << "]";
}

//added error handling for lists
ValueDescriptor* ListLiteral::evaluate(const SymbolTable &symbolTable) const {
    ListObject* list = new ListObject();

    for (ExprNode* expression : _expressions) {
        ValueDescriptor* listElement = expression->evaluate(symbolTable);
        ListElementType elementType;

        switch (listElement->type()) {
            case ValueDescriptor::Type::integer:
            case ValueDescriptor::Type::doubleValue:
                elementType = ListElementType::numeric;
                break;
            case ValueDescriptor::Type::boolean:
                elementType = ListElementType::boolean;
                break;
            case ValueDescriptor::Type::string:
                elementType = ListElementType::string;
                break;
            case ValueDescriptor::Type::list:
                delete listElement;
                delete list;
                throw std::runtime_error("lists cannot contain lists");
        }
        if (list->elementType() != ListElementType::unset && list->elementType() != elementType){
            delete listElement;
            delete list;
            throw std::runtime_error("list elements must have compatible type");
        }

        list->append(listElement);
    }
    return new ListValueDescriptor(list);
}

//LenExpression
LenExpression::LenExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

LenExpression::~LenExpression(){
    delete _expression;
}

void LenExpression::print() const {
    std::cout << "len(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* LenExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = _expression->evaluate(symbolTable);
    int64_t result = 0;

    if (value->type() == ValueDescriptor::Type::string) {
        result = static_cast<StringValueDescriptor*>(value)->value().size();
    } else if (value->type() == ValueDescriptor::Type::list) {
        result =  static_cast<std::int64_t>(static_cast<ListValueDescriptor*>(value)->object()->size());
    } else {
        std::string type = value->typeName();
        delete value;
        throw std::runtime_error("len() requires a string or a list, not a " + type);
    }
    
    delete value;
    return new IntegerValueDescriptor(result);
}

//SplitExpression
SplitExpression::SplitExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

SplitExpression::~SplitExpression(){
    delete _expression;
}

void SplitExpression::print() const {
    std::cout << "split(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* SplitExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor *value = _expression->evaluate(symbolTable);
    if (value->type() != ValueDescriptor::Type::string) {
        std::string valueType = value->typeName();
        delete value;
        throw std::runtime_error("split requires a string not a " + valueType);
    }
    std::string line = static_cast<StringValueDescriptor*>(value)->value();
    delete value;

    ListObject* list = new ListObject(ListElementType::string);
    std::string word;
    // the deer     jumped  over the      moon
    for (char character : line) {
        if (character == ' ' || character == '\t') {
            if (!word.empty()) {
                list->append(new StringValueDescriptor(word));
                word.clear();
            }
        } else {
            word += character;
        }
    }

    if (!word.empty()){
        list->append(new StringValueDescriptor(word));
    }

    return new ListValueDescriptor(list);
}
//popExpression
PopExpression::PopExpression(Token token, ExprNode* expression) : ExprNode{std::move(token)}, _expression{expression}{}

PopExpression::~PopExpression(){
    delete _expression;
}

void PopExpression::print() const {
    std::cout << "pop(";
    _expression->print();
    std::cout << ")";
}

ValueDescriptor* PopExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = _expression->evaluate(symbolTable);

    if (value->type() != ValueDescriptor::Type::list) {
        std::string valueType = value->typeName();
        delete value;
        throw std::runtime_error("pop requires a list not a " + valueType);
    }

    ListObject* list = static_cast<ListValueDescriptor*>(value)->object();

    if (list->size() == 0) {
        delete value;
        throw std::runtime_error("Pop called on an empty list");
    }

    ValueDescriptor* lastElement = list->pop();
    delete value;

    return lastElement;
}

PostfixExpression::PostfixExpression(Token token, ExprNode* atom, ExprNode* expression) : ExprNode{std::move(token)}, atom_{atom}, expression_{expression} {}
PostfixExpression::~PostfixExpression(){
    delete atom_;
    delete expression_;
}

void PostfixExpression::print() const {
    atom_->print();
    if (expression_ != nullptr){
        std::cout << '[';
        expression_->print();
        std::cout << ']';
    }
}

ValueDescriptor* PostfixExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor* value = atom_->evaluate(symbolTable);

    if (expression_ == nullptr){
        return value;
    }

    ValueDescriptor* indexValue = expression_->evaluate(symbolTable);

    if (indexValue->type() != ValueDescriptor::Type::integer){
        std::string type = indexValue->typeName();
        delete indexValue;
        delete value;
        throw std::runtime_error("subscript index must be an integer, got " + type);
    }

    std::int64_t index = static_cast<IntegerValueDescriptor*>(indexValue)->value();
    delete indexValue;

    if (value->type() == ValueDescriptor::Type::string) {
        StringValueDescriptor* stringValue = static_cast<StringValueDescriptor*>(value);
        const std::string &string = stringValue->value();

        if (index < 0 || static_cast<std::size_t>(index) >= string.size()){
            delete value;
            throw std::runtime_error("string index out of range");
        }

        std::string character(1, string[static_cast<std::size_t>(index)]);
        delete value;
        return new StringValueDescriptor(character);
    }

    if (value->type() == ValueDescriptor::Type::list){
        ListValueDescriptor* listValue = static_cast<ListValueDescriptor*>(value);
        ListObject* list = listValue->object();

        if (index < 0 || static_cast<std::size_t>(index) >= list->size()){
            delete value;
            throw std::runtime_error("list index out of range");
        }

        ValueDescriptor* element = list->cloneElementAt(static_cast<std::size_t>(index));
        delete value;
        return element;
    }

    std::string type = value->typeName();
    delete value;
    throw std::runtime_error("cannot subscript " + type);
}