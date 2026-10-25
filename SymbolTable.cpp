#include <iostream>
#include <stdexcept>

#include "SymbolTable.hpp"

void SymbolTable::setValueFor(const std::string &variableName, ValueDescriptor* value) {
    auto found = values.find(variableName);

    if (found != values.end()){
        delete found->second;
    }
    values[variableName] = value;
}

bool SymbolTable::isDefined(const std::string &variableName) const {
    return values.contains(variableName);
}

ValueDescriptor* SymbolTable::getValueFor(const std::string &variableName) const {
    const auto entry = values.find(variableName);
    if (entry == values.end())
        throw std::runtime_error("use of undefined variable '" + variableName + "'");
    return entry->second;
}

void SymbolTable::print() const {
    for (const auto &[variable, value] : values)
        std::cout << variable << " = " << value->toString() << '\n';
}

SymbolTable::~SymbolTable(){
    for (const auto&[variable, value] : values) {
        delete value;
    }
}