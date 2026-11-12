#include "ValueDescriptor.hpp"
#include <sstream>

//IntegerValueDescriptor
IntegerValueDescriptor::IntegerValueDescriptor(std::int64_t value){
    integerValue = value;
}

ValueDescriptor::Type IntegerValueDescriptor::type() const{
    return Type::integer;
}

std::string IntegerValueDescriptor::typeName() const{
    return "integer";
}

std::string IntegerValueDescriptor::toString() const{
    return std::to_string(integerValue);
}

ValueDescriptor* IntegerValueDescriptor::clone() const{
    return new IntegerValueDescriptor(integerValue);
}

std::int64_t IntegerValueDescriptor::value() const{
    return integerValue;
}

//DoubleValueDescriptor
DoubleValueDescriptor::DoubleValueDescriptor(double value){
    doubleValue = value;
}

ValueDescriptor::Type DoubleValueDescriptor::type() const{
    return Type::doubleValue;
}

std::string DoubleValueDescriptor::typeName() const{
    return "double";
}

std::string DoubleValueDescriptor::toString() const{
    std::ostringstream stringOutput;
    stringOutput << doubleValue;
    return stringOutput.str();
}

ValueDescriptor* DoubleValueDescriptor::clone() const{
    return new DoubleValueDescriptor(doubleValue);
}

double DoubleValueDescriptor::value() const{
    return doubleValue;
}


//BooleanValueDescriptor
BooleanValueDescriptor::BooleanValueDescriptor(bool value){
    booleanValue = value;
}

ValueDescriptor::Type BooleanValueDescriptor::type() const {
    return Type::boolean;
}

std::string BooleanValueDescriptor::typeName() const{
    return "boolean";
}

std::string BooleanValueDescriptor::toString() const{
    return booleanValue ? "True" : "False";
}

ValueDescriptor* BooleanValueDescriptor::clone() const{
    return new BooleanValueDescriptor(booleanValue);
}
bool BooleanValueDescriptor::value() const{
    return booleanValue;
}


//StringValueDescriptor
StringValueDescriptor::StringValueDescriptor(std::string value){
    stringValue = value;
}

ValueDescriptor::Type StringValueDescriptor::type() const{
    return Type::string;
}

std::string StringValueDescriptor::typeName() const{
    return "string";
}

std::string StringValueDescriptor::toString() const{
    return stringValue;
}

ValueDescriptor* StringValueDescriptor::clone() const{
    return new StringValueDescriptor(stringValue);
}

const std::string& StringValueDescriptor::value() const{
    return stringValue;
}


ListValueDescriptor::ListValueDescriptor(ListObject *object)
    : listObject{object} {
    listObject->retain();
}

ListValueDescriptor::~ListValueDescriptor() {
    if (listObject->release()) {
        delete listObject;
    }
}

Type ListValueDescriptor::type() const {
    return Type::list;
}

std::string ListValueDescriptor::typeName() const {
    return "list";
}

// [0, 0, 0, ] []
std::string ListValueDescriptor::toString() const {
    std::string output = "[";
    for (size_t i = 0; i < listObject->size(); i++) {
        ValueDescriptor* element = listObject->cloneElementAt(i);

        if (i != listObject->size() - 1) {
            output += element;
            output += ", ";
        } else {
            output += element;
        }

        delete element;
    }
    output += "]";

    return output;
}

ValueDescriptor* ListValueDescriptor::clone() const {
    return new ListValueDescriptor(listObject);
}

ListObject* ListValueDescriptor::object() {
    return listObject;
}

const ListObject* ListValueDescriptor::object() const {
    return listObject;
}