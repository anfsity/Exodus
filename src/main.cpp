#include "../3rd-party/fmt/core.h"
#include "../include/helper/log.hpp"
#include "../include/high/ast.hpp"
#include "../include/high/ir_builder.hpp"
#include "../include/high/ir_printer.hpp"
#include "../include/type.hpp"
#include <memory>

using namespace exodus;
using namespace exodus::ast;
using namespace exodus::high_ir;

extern FILE *yyin;
extern int yyparse(CompUnitAST &ast);

int main(int argc, char **argv) {
  if (argc > 1) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      fmt::print(stderr, "Error: Could not open file {}\n", argv[1]);
      return 1;
    }
  }

  CompUnitAST ast;
  if (yyparse(ast) != 0) {
    fmt::print(stderr, "Error: Parsing failed.\n");
    return 1;
  }

  IRBuilder builder(nullptr);
  auto module = builder.build(ast);

  IRPrinter printer;
  fmt::print("{}\n", printer.dump(*module));

  return 0;
}

