#include <iostream>

#include "Token.hpp"

void Token::print(std::ostream &output) const {
    if (isNewline())
        output << "NEWLINE";
    else if (isEof())
        output << "EOF";
    else if (isIndent())
        output << "INDENT";
    else if (isDedent())
        output << "DEDENT";
    else if (isForKeyword())
        output << "for";
    else if (isPrintKeyword())
        output << "print";
    else if (isInKeyword())
        output << "in";
    else if (isRangeKeyword())
        output << "range";
    else if (isBoolean())
        output << (_boolValue ? "True" : "False");
    else if (isStrKeyword())
        output << " STR ";  
    else if (isAppendKeyword())
        output << "append";
    else if (isIfKeyword())
        output << "if";
    else if (isWhileKeyword())
        output << "while";  
    else if (isElifKeyword())
        output << "elif";
    else if (isElseKeyword())
        output << "else";
    else if (isInputKeyword())
        output << "input";
    else if (isIntKeyword())
        output << "int";
    else if (isFloatKeyword())
        output << "float";
    else if (isLenKeyword())
        output << "len";
    else if (isSplitKeyword())
        output << "split";
    else if (isPopKeyword())
        output << "pop";
    else if (isOpenParen())
        output << '(';
    else if (isClosedParen())
        output << ')';
    else if (isAssignmentOperator())
        output << " = ";
    else if (isSemicolon())
        output << ';';
    else if (isMultiplicationOperator())
        output << " * ";
    else if (isAdditionOperator())
        output << " + ";
    else if (isSubtractionOperator())
        output << " - ";
    else if (isFloorDivisionOperator())
        output << " // ";
    else if (isModuloOperator())
        output << " % ";
    else if (isDivisionOperator())
        output << " / ";
    else if (isIdentifier())
        output << identifier();
    else if (isInteger())
        output << integerValue();
    else if (isEqualityOperator())
        output << " == ";
    else if (isNotEqualOperator())
        output << " != ";
    else if (isGreaterThanOperator())
        output << " > ";
    else if (isGreaterThanOrEqualOperator())
        output << " >= ";
    else if (isLessThanOperator())
        output << " < ";
    else if (isLessThanOrEqualOperator())
        output << " <= ";
    else if (isOpenBrace())
        output << " { ";
    else if (isClosedBrace())
        output << " } ";
    else if (isOpenBracket())
    else if (isColon())
        output << ':';
    else if (isComma())
        output << ',';
    else if (isDouble())
        output << _doubleValue;
    else if (isString())
        output << _stringValue;
    else
        output << "uninitialized token";
}
