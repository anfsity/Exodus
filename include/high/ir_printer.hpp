#pragma once

#include "../helper/ir_printer_base.hpp"
#include "ir.hpp"
#include <string>

namespace exodus::high_ir {

struct IRPrinter : public IRPrinterBase {
  auto dump(const Module &m) -> std::string {
    std::string res;
    for (auto &g : m.globals) {
      res += dump_global(*g);
    }
    for (auto &f : m.functions) {
      reset_context();
      res += dump(*f);
    }
    return res;
  }

  auto dump(const Function &f) -> std::string {
    std::string args_s;
    for (auto &arg : f.args) {
      args_s += arg->dump() + (&arg == &f.args.back() ? "" : ", ");
    }

    if (f.is_decl) {
      return fmt::format("decl @{}({}) : {}\n", f.name, args_s, f.type->to_string());
    }

    return fmt::format(
      "func @{}({}) : {} {{\n{}}}\n",
      f.name,
      args_s,
      f.type->to_string(),
      dump(f.body)
    );
  }

  auto dump(const Region &r) -> std::string {
    std::string res;
    idt++;
    for (auto &op : r) {
      res += dump(*op);
    }
    idt--;
    return res;
  }

  auto dump(const Op &op) -> std::string {
    std::string prefix = ident();
    std::string line = dump_op_common(op);

    return std::visit(
      overload{
        [&](const IfPayload &ifp) -> std::string {
          std::string res = fmt::format(
            "{}{} {{\n{}{}}}", prefix, line, dump(*ifp.then_region), ident()
          );
          if (ifp.else_region.has_value()) {
            res += fmt::format(" else {{\n{}{}}}", dump(*ifp.else_region), ident());
          }
          return res + "\n";
        },
      
        [&](const WhilePayload &whp) -> std::string {
          std::string res = fmt::format(
            "{}{} {{\n{}{}}} {{\n{}{}}}",
            prefix,
            line,
            dump(*whp.cond_region),
            ident(),
            dump(*whp.loop_region),
            ident()
          );
          return res + "\n";
        },
        [&](const auto &) -> std::string { return prefix + line + "\n"; }
      },
      op.payload
    );
  }
};

} // namespace exodus::high_ir
