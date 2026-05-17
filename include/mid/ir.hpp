#pragma once

#include "../high/ir.hpp"
#include <list>
#include <string>

namespace exodus::mid_ir {

struct Block {
  std::string name;
  std::list<high_ir::Op *> insts;

  std::vector<Block *> preds;
  std::vector<Block *> succs;

  Block(std::string n) : name(std::move(n)) {}
};

struct LinearFunction {
  std::string name;
  std::shared_ptr<Type> type;
  std::vector<high_ir::Argument *> args;

  std::list<std::unique_ptr<Block>> blocks;
  bool is_decl = false;
};

} // namespace exodus::mid_ir