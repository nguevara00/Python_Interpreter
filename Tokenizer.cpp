#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "Tokenizer.hpp"

bool Tokenizer::isDigit(char character) {
    return std::isdigit(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::isIdentifierStart(char character) {
    return std::isalpha(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isIdentifierPart(char character) {
    return std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isDiscardedWhitespace(char character) {
    return character != '\n' && std::isspace(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::getCharacter(char &character) {
    if (!inputStream.get(character))
        return false;

    if (character == '\n') {
        ++lineNumber;
        columnNumber = 1;
    } else {
        ++columnNumber;
    }
    return true;
}

std::string Tokenizer::readIdentifier(char firstCharacter) {
    std::string identifier{firstCharacter};
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isIdentifierPart(character))
            break;
        getCharacter(character);
        identifier += character;
    }
    return identifier;
}

int Tokenizer::readInteger(char firstDigit) {
    int value = firstDigit - '0';
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isDigit(character))
            break;

        const int digit = character - '0';
        if (value > (std::numeric_limits<int>::max() - digit) / 10) {
            std::cerr << "Integer literal is too large at line " << lineNumber
                      << ", column " << columnNumber << ".\n";
            std::exit(EXIT_FAILURE);
        }

        getCharacter(character);
        value = value * 10 + digit;
    }
    return value;
}

Tokenizer::Tokenizer(std::ifstream &stream) : inputStream{stream} { indentStack.push(0); }

/*
    * Make a std::stack<int>
 * push 0 onto the stack
 * Look at the spaces in a newline, count spaces until we reach a character
 *      if the number of spaces is greater than the number on top of the stack
 *          push the #spaces onto the stack
 *          generate indent token
 *      if the number of spaces is the same as the top of the stack
 *          continue
 *      if number of spaces is less than the top of the stack
 *              pop the stack
 *              create a dedent token
 *              if the number of spaces is equal to the top of the stack
 *                  continue
 *              else : exit, program error
 * **this doesnt work for lines with multiple dedents, still need to figure that out**
 *          

 */

Token Tokenizer::getToken() {
    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());

        if (lineStart) { 
            int spaces = 0;

            //leading whitespace detection will now also detect tab characters in addition to spaces.
            while (inputStream.peek() == ' ' || inputStream.peek() == '\t') {
                if (inputStream.peek() == '\t') {
                    std::cerr << "Indentation error at line " << lineNumber << ": tab found in leading indentation\n";
                    std::exit(EXIT_FAILURE);
                }
                getCharacter(character);
                ++spaces;
            }

            //this was added to fix a bug. if a line had blank spaces and nothing else, the program was treating it as an indentation level
            if (inputStream.peek() == '\n' || inputStream.peek() == std::char_traits<char>::eof())  {
                lineStart = false;
                continue;
            }

            lineStart = false;
            currentIndent = spaces;
            checkIndent = true;
            continue;
        }

        if (checkIndent) {
            if (currentIndent > indentStack.top()) {
                indentStack.push(currentIndent);
                checkIndent = false;
                Token token;
                token.setLocation(lineNumber, columnNumber);
                token.markAsIndent();
                tokens.push_back(token);
                return lastToken = token;
            }

            if (currentIndent < indentStack.top()) {
                indentStack.pop();

                if (currentIndent > indentStack.top()) {
                    std::cerr << "Indentation error at line " << lineNumber << ": dedent does not match any outer indentation level.\n";
                    std::exit(EXIT_FAILURE);
                }

                Token token;
                token.setLocation(lineNumber, columnNumber);
                token.markAsDedent();
                tokens.push_back(token);
                return lastToken = token;
            }

            checkIndent = false;
        }

        if (isDiscardedWhitespace(character)) {
            getCharacter(character);
            continue;
        }

        if (character == '\n') {
            const auto newlineLine = lineNumber;
            const auto newlineColumn = columnNumber;
            getCharacter(character);
            lineStart = true;

            if (lineContainsToken) {
                Token token;
                token.setLocation(newlineLine, newlineColumn);
                token.markAsNewline();
                lineContainsToken = false;
                tokens.push_back(token);
                return lastToken = token;
            }

            continue;
        }

        break;
    }

    Token token;
    token.setLocation(lineNumber, columnNumber);

    if (inputStream.peek() == std::char_traits<char>::eof()) {
        if (inputStream.bad()) {
            std::cerr << "Error while reading the input stream in Tokenizer.\n";
            std::exit(EXIT_FAILURE);
        }

       if (lineContainsToken) {
            lineContainsToken = false;
            token.markAsNewline();
        } else if (indentStack.size() > 1) {
            indentStack.pop();
            token.markAsDedent();
        } else {
            token.markAsEof();
        }

    } else {
        char character;
        std::string multiCharString = "\0";
        getCharacter(character);

        if (isDigit(character)) {
            token.setIntegerValue(readInteger(character));
        } else if (character == '!' || character == '=' || character == '+' || character == '-' || character == '*' || character == '/' || character == '%' || character == ';' || character == ':' || character == '(' || character == ')' || character == '{' || character == '}' || character == '>' || character == '<' || character == ':' || character == ',') {
                    if ((character == '=' || character == '<' || character == '>' || character == '!') && inputStream.peek() == '=') {
                        multiCharString += character;
                        getCharacter(character);
                        multiCharString += character;
                        token.setMultiCharSymbol(multiCharString);
                    } else {
                        token.setSymbol(character);
                    }
        } else if (isIdentifierStart(character)) {
            std::string identifier = readIdentifier(character);
            if (identifier == "for")
                token.setKeyword(Keyword::forKeyword);
            else if (identifier == "print")
                token.setKeyword(Keyword::printKeyword);
            else if (identifier == "range")
                token.setKeyword(Keyword::rangeKeyword);
            else if (identifier == "in")
                token.setKeyword(Keyword::inKeyword);
            else
                token.setIdentifier(std::move(identifier));
        } else {
            std::cerr << "Unknown character in input at line " << token.lineNumber()
                      << ", column " << token.columnNumber() << ": '"
                      << character << "'.\n";
            std::exit(EXIT_FAILURE);
        }

        lineContainsToken = true;
    }

    tokens.push_back(token);
    return lastToken = token;
}

void Tokenizer::ungetToken() {
    if (ungottenToken) {
        std::cerr << "Tokenizer supports only one ungotten token.\n";
        std::exit(EXIT_FAILURE);
    }
    ungottenToken = true;
}

void Tokenizer::printProcessedTokens(std::ostream &output) const {
    for (const auto &token : tokens) {
        token.print(output);
        output << '\n';
    }
}
