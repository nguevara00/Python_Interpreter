#include "ValueDescriptor.hpp"
#include <cstdint>
#include <string>
#include <sstream>

    //IntegerValueDescriptor
    IntegerValueDescriptor::IntegerValueDescriptor(std::int64_t value){
        integerValue = value;
    }

    ValueDescriptor::Type IntegerValueDescriptor::type() const{
        return ValueDescriptor::Type::integer;
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
        return ValueDescriptor::Type::doubleValue;
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
        return ValueDescriptor::Type::boolean;
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
        return ValueDescriptor::Type::string;
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
