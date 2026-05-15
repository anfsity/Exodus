#pragma once

#include "../helper/log.hpp"
#include "../helper/overload.hpp"
#include "ast.hpp"
#include "ir.hpp"
#include "sym_tab.hpp"
#include <cassert>
#include <string>

namespace exodus::high_ir {

struct IRPrinter {
  auto dump(const Module &m) -> std::string {
    idt = 0;
    std::string res;
    for (auto &g : m.globals) {
      res += dump(*g);
    }
    for (auto &f : m.functions) {
      id_cnt = 0;
      res += dump(*f);
    }
    return res;
  }

private:
  int id_cnt;
  int idt;

  auto ident() -> std::string { return std::string(idt * 2, ' '); }
  auto get(const Value *v) -> std::string;

  auto dump(const Op &op) -> std::string;
  auto dump(const GlobalVar &v) -> std::string;
  auto dump(const Function &f) -> std::string;
  auto dump(const Region &r) -> std::string;
  auto dump(const InitVal &i) -> std::string;
  auto dump_call(const Op &op) -> std::string;
  auto dump_simple_op(const Op &op) -> std::string;
};

inline auto opcode_to_str(OpCode oc) -> std::string {
  // clang-format off
  switch (oc) {
  case OpCode::Add: return "add";
  case OpCode::Sub: return "sub";
  case OpCode::Mul: return "mul";
  case OpCode::Div: return "div";
  case OpCode::Mod: return "mod";
  case OpCode::FAdd: return "fadd";
  case OpCode::FSub: return "fsub";
  case OpCode::FMul: return "fmul";
  case OpCode::FDiv: return "fdiv";
  case OpCode::I2F: return "i2f";
  case OpCode::F2I: return "f2i";
  case OpCode::ZExt: return "zext";
  case OpCode::Eq: return "eq";
  case OpCode::Ne: return "ne";
  case OpCode::Lt: return "lt";
  case OpCode::Gt: return "gt";
  case OpCode::Le: return "le";
  case OpCode::Ge: return "ge";
  case OpCode::And: return "and";
  case OpCode::Or: return "or";
  case OpCode::Xor: return "xor";
  case OpCode::Shl: return "shl";
  case OpCode::Shr: return "shr";
  case OpCode::Alloca: return "alloca";
  case OpCode::Load: return "load";
  case OpCode::Store: return "store";
  case OpCode::GetPtr: return "getptr";
  case OpCode::Call: return "call";
  case OpCode::Ret: return "ret";
  case OpCode::If: return "if";
  case OpCode::While: return "while";
  case OpCode::Break: return "break";
  case OpCode::Continue: return "continue";
  case OpCode::Yield: return "yield";
  case OpCode::Condition: return "condition";
  default: return "unknown";
  }
  // clang-format on
}

inline auto is_compare_opcode(OpCode oc) -> bool {
  switch (oc) {
  case OpCode::Eq:
  case OpCode::Ne:
  case OpCode::Lt:
  case OpCode::Gt:
  case OpCode::Le:
  case OpCode::Ge:
    return true;
  default:
    return false;
  }
}

inline auto is_cast_opcode(OpCode oc) -> bool {
  switch (oc) {
  case OpCode::I2F:
  case OpCode::F2I:
  case OpCode::ZExt:
    return true;
  default:
    return false;
  }
}

template <typename DumpValue>
inline auto join_operands(
  const std::vector<Value *> &operands,
  DumpValue dump_value
) -> std::string {
  std::string res;
  for (size_t i = 0; i < operands.size(); ++i) {
    res += dump_value(operands[i]);
    if (i + 1 < operands.size()) {
      res += ", ";
    }
  }
  return res;
}

inline auto join_operand_types(const std::vector<Value *> &operands)
  -> std::string {
  return join_operands(operands, [](const Value *operand) {
    return operand->type->to_string();
  });
}

inline auto IRPrinter::get(const Value *v) -> std::string {
  if (v->kind == ValueKind::OpResult) {
    auto res = static_cast<const OpResult *>(v);
    if (res->id == -1)
      res->id = id_cnt++;
  }
  return v->dump();
}

inline auto IRPrinter::dump(const GlobalVar &v) -> std::string {
  return fmt::format(
    "{} = global {} : {}\n", v.addr->dump(), dump(v.init), v.type->to_string()
  );
}

inline auto IRPrinter::dump(const InitVal &i) -> std::string {
  return std::visit(
    overload{
      [](int i) -> std::string { return std::to_string(i); },
      [](float f) -> std::string { return std::to_string(f) + "f"; },
      [](ZeroInit) -> std::string { return std::string("zeroinit"); },
      [&](const InitList &list) -> std::string {
        std::string s = "{";
        for (auto &l : list.values) {
          s += dump(l) + (&l == &list.values.back() ? "" : ", ");
        }
        return s + "}";
      }
    },
    i.data
  );
}

inline auto IRPrinter::dump(const Function &f) -> std::string {
  std::string args_s;

  for (auto &arg : f.args) {
    args_s += arg->dump() + (&arg == &f.args.back() ? "" : ", ");
  }

  if (f.is_decl) {
    return fmt::format(
      "decl @{}() : {}\n", f.name, f.type->to_string()
    );
  }

  return fmt::format(
    "func @{}({}) : {} {{\n{}}}\n",
    f.name,
    args_s,
    f.type->to_string(),
    dump(f.body)
  );
}

inline auto IRPrinter::dump(const Region &r) -> std::string {
  std::string res;
  idt++;
  for (auto &op : r) {
    res += dump(*op);
  }
  idt--;
  return res;
}

inline auto IRPrinter::dump_call(const Op &op) -> std::string {
  auto &cp = std::get<CallPayload>(op.payload);
  std::string line;

  if (op.result && !op.result->type->is_void()) {
    line += get(op.result) + " = ";
  }

  line += fmt::format(
    "call @{}({}) : ({})",
    cp.func_name,
    join_operands(op.operands, [&](const Value *operand) {
      return get(operand);
    }),
    join_operand_types(op.operands)
  );

  if (op.result) {
    line += " -> " + op.result->type->to_string();
  }

  return line;
}

inline auto IRPrinter::dump_simple_op(const Op &op) -> std::string {
  std::string line;

  if (op.result) {
    line += get(op.result) + " = ";
  }

  line += opcode_to_str(op.code);

  if (!op.operands.empty()) {
    if (op.code == OpCode::Condition) {
      line += fmt::format(
        "({})",
        join_operands(op.operands, [&](const Value *operand) {
          return get(operand);
        })
      );
    } else {
      line += " " + join_operands(op.operands, [&](const Value *operand) {
        return get(operand);
      });
    }
  }

  switch (op.code) {
  case OpCode::Alloca: {
    assert(op.result);
    assert(op.result->type->is_ptr());
    auto ptr_type = std::static_pointer_cast<Ptr>(op.result->type);
    line += " : " + ptr_type->target->to_string();
    break;
  }
  case OpCode::Load:
  case OpCode::Store:
    line += " : " + join_operand_types(op.operands);
    break;
  case OpCode::GetPtr:
    assert(!op.operands.empty());
    line += " : " + op.operands.front()->type->to_string();
    break;
  default:
    if (is_compare_opcode(op.code)) {
      assert(!op.operands.empty());
      line += " : " + op.operands.front()->type->to_string();
    } else if (is_cast_opcode(op.code)) {
      assert(!op.operands.empty());
      line += " : " + op.operands.front()->type->to_string();
    }
    break;
  }

  if (op.result) {
    line += " -> " + op.result->type->to_string();
  }

  return line;
}

inline auto IRPrinter::dump(const Op &op) -> std::string {
  std::string prefix = ident();
  std::string line =
    op.code == OpCode::Call ? dump_call(op) : dump_simple_op(op);

  return std::visit(
    overload{
      [&](const IfPayload &ifp) -> std::string {
        std::string res = fmt::format(
          "{}{} {{\n{}{}}}", prefix, line, dump(*ifp.then_region), ident()
        );

        if (ifp.else_region.has_value()) {
          res +=
            fmt::format(" else {{\n{}{}}}\n", dump(*ifp.else_region), ident());
        } else {
          res += "\n";
        }

        return res;
      },
      [&](const WhilePayload &whp) -> std::string {
        std::string res = fmt::format(
          "{}{} {{\n{}{}}}", prefix, line, dump(*whp.cond_region), ident()
        );

        res += fmt::format(" {{\n{}{}}}\n", dump(*whp.loop_region), ident());
        return res;
      },
      [&](const auto &) -> std::string { return prefix + line + "\n"; }
    },
    op.payload
  );
}

} // namespace exodus::high_ir
