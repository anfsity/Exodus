#include "../include/helper/log.hpp"
#include "../include/high/ast.hpp"
#include "../include/type.hpp"
#include <memory>

using namespace exodus::ast;

extern FILE *yyin;
extern int yyparse(std::unique_ptr<CompUnitAST> &ast);

int main() { return 0; }