cmake_minimum_required(VERSION 3.12)
project(ExprInter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ExprInter
    main.cpp
    Token.cpp
    Tokenizer.cpp
    Parser.cpp
    Expr.cpp
    SymbolTable.cpp
    Statements.cpp
    listObject.cpp
    ValueDescriptor.cpp
)

target_compile_options(ExprInter PRIVATE -Wall -Wextra -Wpedantic)
