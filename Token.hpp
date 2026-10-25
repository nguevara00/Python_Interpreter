#ifndef ALEXICALANALYZER_TOKEN_HPP
#define ALEXICALANALYZER_TOKEN_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <cstdint>

enum class Keyword {
    none,
    forKeyword,
    printKeyword,
    inKeyword,
    rangeKeyword,
    strKeyword
};

class Token {
public:
    [[nodiscard]] bool isEof() const { return _eof; }
    [[nodiscard]] bool isNewline() const { return _newline; }
    
    //new phase 2
    [[nodiscard]] bool isIndent() const { return _indent; }
    [[nodiscard]] bool isDedent() const { return _dedent; }
    
    void markAsEof() { _eof = true; }
    void markAsNewline() { _newline = true; }

    //new phase 2
    void markAsIndent() {_indent = true; }
    void markAsDedent() {_dedent = true; }

    void setLocation(std::size_t line, std::size_t column) {
        _lineNumber = line;
        _columnNumber = column;
    }
    [[nodiscard]] std::size_t lineNumber() const { return _lineNumber; }
    [[nodiscard]] std::size_t columnNumber() const { return _columnNumber; }

    void setSymbol(char symbol) { _symbol = symbol; }
    void setMultiCharSymbol(std::string& multiCharSymbol) { _multiCharSymbol = multiCharSymbol; }
    [[nodiscard]] char symbol() const { return _symbol; }

    [[nodiscard]] bool isOpenParen() const { return _symbol == '('; }
    [[nodiscard]] bool isClosedParen() const { return _symbol == ')'; }
    [[nodiscard]] bool isSemicolon() const { return _symbol == ';'; }
    [[nodiscard]] bool isAssignmentOperator() const { return _symbol == '='; }
    [[nodiscard]] bool isMultiplicationOperator() const { return _symbol == '*'; }
    [[nodiscard]] bool isAdditionOperator() const { return _symbol == '+'; }
    [[nodiscard]] bool isSubtractionOperator() const { return _symbol == '-'; }
    [[nodiscard]] bool isModuloOperator() const { return _symbol == '%'; }
    [[nodiscard]] bool isDivisionOperator() const { return _symbol == '/'; }
    [[nodiscard]] bool isFloorDivisionOperator() const { return _multiCharSymbol == "//"; }

    // New Relational Operators
    [[nodiscard]] bool isEqualityOperator() const { return _multiCharSymbol == "=="; }
    [[nodiscard]] bool isNotEqualOperator() const { return _multiCharSymbol == "!="; }
    [[nodiscard]] bool isGreaterThanOperator() const { return _symbol == '>'; }
    [[nodiscard]] bool isGreaterThanOrEqualOperator() const { return _multiCharSymbol == ">="; }
    [[nodiscard]] bool isLessThanOperator() const { return _symbol == '<'; }
    [[nodiscard]] bool isLessThanOrEqualOperator() const { return _multiCharSymbol == "<="; }
    [[nodiscard]] bool isOpenBracket() const { return _symbol == '{'; }
    [[nodiscard]] bool isClosedBracket() const { return _symbol == '}'; }
    
    //New phase 2 tokens
    [[nodiscard]] bool isColon() const { return _symbol == ':'; }
    [[nodiscard]] bool isComma() const { return _symbol == ','; }

    //New phase 3 tokens


    

    void setIdentifier(std::string identifier) { _identifier = std::move(identifier); }
    [[nodiscard]] bool isIdentifier() const { return !_identifier.empty(); }
    [[nodiscard]] const std::string &identifier() const { return _identifier; }

    void setKeyword(Keyword keyword) { _keyword = keyword; }
    [[nodiscard]] bool isKeyword() const { return _keyword != Keyword::none; }
    [[nodiscard]] bool isForKeyword() const { return _keyword == Keyword::forKeyword; }
    [[nodiscard]] bool isPrintKeyword() const { return _keyword == Keyword::printKeyword; }
    
    [[nodiscard]] bool isInKeyword() const { return _keyword == Keyword::inKeyword; }
    [[nodiscard]] bool isRangeKeyword() const { return _keyword == Keyword::rangeKeyword; }
    [[nodiscard]] bool isStrKeyword() const { return _keyword == Keyword::strKeyword; }
    [[nodiscard]] Keyword keyword() const { return _keyword; }

    void setIntegerValue(std::int64_t value) {
        _integerValue = value;
        _isInteger = true;
    }

    void setDoubleValue(double value){
        _doubleValue = value;
        _isDouble = true;
    }


    void setStringValue(std::string value){
        _stringValue = value;
        _isString = true;
    }

    void setBooleanValue(bool value){
        _boolValue = value;
        _isBoolean = true;
    }



    [[nodiscard]] bool isInteger() const { return _isInteger; }
    [[nodiscard]] bool isString() const { return _isString; }
    [[nodiscard]] bool isDouble() const { return _isDouble; }
    [[nodiscard]] bool isBoolean() const { return _isBoolean; }

    [[nodiscard]] int64_t integerValue() const { return _integerValue; }
    [[nodiscard]] double doubleValue() const { return _doubleValue; }
    [[nodiscard]] std::string stringValue() const { return _stringValue; }
    [[nodiscard]] bool booleanValue() const { return _boolValue; }

    [[nodiscard]] int getIndentSpaces() const { return _indentSpaces; }
    void markIndentSpaces(int& spaces) { _indentSpaces = spaces; }

    void print(std::ostream &output) const;

private:
    std::string _identifier{};
    Keyword _keyword{Keyword::none};
    bool _eof{false};
    bool _isBoolean{false};
    bool _newline{false};
    bool _indent{false};
    bool _dedent{false};
    bool _isInteger{false};
    bool _isString{false};
    bool _isDouble{false};

    char _symbol{'\0'};
    std::string _multiCharSymbol{'\0'};

    //types
    std::int64_t _integerValue{0};
    std::string _stringValue{'\0'};
    double _doubleValue{0};
    bool _boolValue{false};

    std::size_t _lineNumber{0};
    std::size_t _columnNumber{0};
    int _indentSpaces{0};
};

#endif // ALEXICALANALYZER_TOKEN_HPP
