#include <iostream>
#include "Parser.h"

int main() {
  Bish::Parser p;
  std::string test = "{ a = 0; b = 1; c = a + b; }";
  Bish::AST *ast = p.parse(test);
  //ast->print(std::cout);
  return 0;
}
