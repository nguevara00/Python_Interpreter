#ifndef EXPRINTER_SYMBOLTABLE_HPP
#define EXPRINTER_SYMBOLTABLE_HPP

#include <map>
#include <string>
#include "ValueDescriptor.hpp"

class SymbolTable {
public:
    void setValueFor(const std::string &variableName, ValueDescriptor* value);
    [[nodiscard]] bool isDefined(const std::string &variableName) const;
    [[nodiscard]] ValueDescriptor* getValueFor(const std::string &variableName) const;
    void print() const;

    SymbolTable() = default;
    ~SymbolTable();
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable &operator=(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;
    SymbolTable &operator=(SymbolTable &&) = delete;

private:
    std::map<std::string, ValueDescriptor*> values; // please check to make sure this needs to be a pointer and not an object. "descriptor remains owned by the symboltable"
};

#endif // EXPRINTER_SYMBOLTABLE_HPP