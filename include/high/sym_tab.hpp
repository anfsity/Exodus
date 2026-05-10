#pragma once

#include "../type.hpp"
#include "ir.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace exodus::high_ir {

struct Symbol {
  std::shared_ptr<Type> type;
  Value *val;
  Function *func;
  bool is_const;
};

struct SymTab {
  SymTab() { enter_scope(); }

  auto enter_scope() -> void { scopes.emplace_back(); }

  auto exit_scope() -> void {
    if (scopes.size() > 1u) {
      scopes.pop_back();
    }
  }

  auto push(std::string &name, Symbol sym) -> bool {
    auto &cur = scopes.back();
    if (cur.find(name) != cur.end()) {
      return false;
    }
    cur[name] = std::move(sym);
    return true;
  }

  auto lookup(std::string &name) -> std::optional<Symbol> {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto res = it->find(name);
      if (res != it->end()) {
        return res->second;
      }
    }
    return std::nullopt;
  }

  auto is_global() const -> bool { return scopes.size() == 1u; }

  std::vector<std::map<std::string, Symbol>> scopes;
};

} // namespace exodus::high_ir