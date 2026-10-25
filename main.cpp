#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>

#include "Parser.hpp"
#include "SymbolTable.hpp"
#include "Tokenizer.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " nameOfAnInputFile\n";
        return EXIT_FAILURE;
    }

    std::ifstream inputStream{argv[1]};
    if (!inputStream.is_open()) {
        std::cerr << "Unable to open " << argv[1] << ".\n";
        return EXIT_FAILURE;
    }

    Tokenizer tokenizer{inputStream};
    Parser parser{tokenizer};
    Statements *statements = parser.program();

    statements->print();
    std::cout.flush();

    SymbolTable symbolTable;
    try {
        statements->evaluate(symbolTable);
    } catch (const std::exception &error) {
        std::cerr << "Runtime error: " << error.what() << '\n';
        delete statements;
        return EXIT_FAILURE;
    }

    std::cout << "\nSymbol table contains the following variables:\n";
    symbolTable.print();
    delete statements;
    return EXIT_SUCCESS;
}