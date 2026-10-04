#include <iostream>

#include "Token.hpp"

void Token::print(std::ostream &output) const {
    if (isNewline())
        output << "NEWLINE";
    else if (isEof())
        output << "EOF";
    else if (isForKeyword())
        output << "for";
    else if (isPrintKeyword())
        output << "print";
    else if (isOpenParen())
        output << '(';
    else if (isCloseParen())
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
    else if (isOpenBracket())
        output << " { ";
    else if (isClosedBracket())
        output << " } ";
    else if (isColon())
        output << " : ";
    else if (isComma())
        output << " , ";
    else
        output << "uninitialized token";
}
