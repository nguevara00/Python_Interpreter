CXX = c++
CXXFLAGS = -g -std=c++20 -Wall -Wextra -Wpedantic
TARGET = interpreter.x
OBJECTS = Token.o Tokenizer.o Parser.o Expr.o SymbolTable.o Statements.o main.o ValueDescriptor.o

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)
	rm -f $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Token.o: Token.cpp Token.hpp
Tokenizer.o: Tokenizer.cpp Tokenizer.hpp Token.hpp
Expr.o: Expr.cpp Expr.hpp Token.hpp SymbolTable.hpp
SymbolTable.o: SymbolTable.cpp SymbolTable.hpp
ValueDescriptor.o: ValueDescriptor.cpp ValueDescriptor.hpp
Parser.o: Parser.cpp Parser.hpp Token.hpp Tokenizer.hpp Expr.hpp Statements.hpp
Statements.o: Statements.cpp Statements.hpp Expr.hpp SymbolTable.hpp
main.o: main.cpp Parser.hpp Tokenizer.hpp SymbolTable.hpp

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
