/*
 * Copyright 2016 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Print out text in text format
//

#include <wasm.h>
#include <pass.h>

namespace wasm {

struct PrintText : public WasmVisitor<PrintText, void> {
  std::ostream& o;
  unsigned indent;
  unsigned expr_depth;
  bool pulling_pushes;
  bool use_line_term;
  bool pull_pushes;

  PrintText(std::ostream& o) : o(o), indent(0),
                               expr_depth(0), pulling_pushes(false),
                               use_line_term(false), pull_pushes(true) {}

  void printFullLine(Expression *expression) {
    doIndent(o, indent);
    visit(expression);
    o << '\n';
  }

  void printEndOfPush() {
    if (use_line_term) o << ';';
    o << '\n';
  }

  void pullPushes(Expression *expression) {
    if (!pull_pushes) return;
    if (pulling_pushes) abort();
    pulling_pushes = true;
    visit(expression);
    if (!pulling_pushes) abort();
    pulling_pushes = false;
  }

  void visitBlock(Block *curr) {
    if (pulling_pushes) return;
    if (expr_depth++) o << '(';
    unsigned saved_expr_depth = expr_depth;
    expr_depth = 0;
    o << '{' << '\n';
    indent++;
    int useSeparator = curr->list.size();
    for (auto expression : curr->list) {
      pullPushes(expression);
      if (--useSeparator && use_line_term) {
        doIndent(o, indent);
        visit(expression);
        o << ';' << '\n';
      } else
        printFullLine(expression);
    }
    indent--;
    doIndent(o, indent);
    o << '}';
    if (curr->name.is()) {
      o << " : " << curr->name;
    }
    expr_depth = saved_expr_depth;
    if (--expr_depth) o << ')';
  }
  void visitIf(If *curr) {
    if (pulling_pushes) return;
    if (expr_depth++) o << '(';
    unsigned saved_expr_depth = expr_depth;
    expr_depth = 0;
    o << "if ";
    visit(curr->condition);
    o << " {" << '\n';
    indent++;
    pullPushes(curr->ifTrue);
    printFullLine(curr->ifTrue);
    indent--;
    if (curr->ifFalse) {
        doIndent(o, indent);
        o << "} else {" << '\n';
        indent++;
        pullPushes(curr->ifFalse);
        printFullLine(curr->ifFalse);
        indent--;
    }
    doIndent(o, indent);
    o << '}';
    expr_depth = saved_expr_depth;
    if (--expr_depth) o << ')';
  }
  void visitLoop(Loop *curr) {
    if (pulling_pushes) return;
    if (expr_depth++) o << '(';
    unsigned saved_expr_depth = expr_depth;
    expr_depth = 0;
    if (curr->in.is()) {
      o << curr->in;
    }
    o << ": {" << '\n';
    indent++;
    pullPushes(curr->body);
    printFullLine(curr->body);
    indent--;
    doIndent(o, indent);
    o << '}';
    if (curr->out.is()) {
      o << " : " << curr->out;
    }
    expr_depth = saved_expr_depth;
    if (--expr_depth) o << ')';
  }
  void visitBreak(Break *curr) {
    if (pulling_pushes) {
      if (curr->value && !curr->value->is<Nop>()) {
        pulling_pushes = false;
        pullPushes(curr->value);
        doIndent(o, indent);
        o << "push ";
        visit(curr->value);
        pulling_pushes = true;
        printEndOfPush();
      }
      if (curr->condition) visit(curr->condition);
      return;
    }

    if (curr->condition) {
      o << "br_if " << curr->name << ", ";
      visit(curr->condition);
    } else {
      o << "br " << curr->name;
    }
  }
  void visitSwitch(Switch *curr) {
    if (pulling_pushes) return;
    o << '{' << '\n';
    indent++;
    doIndent(o, indent);
    for (int i = curr->cases.size(); i > 0; i--) {
      o << '{' << '\n';
      indent++;
      doIndent(o, indent);
    }
    o << "br_table (";
    bool useSeparator = false;
    for (auto& t : curr->targets) {
      if (useSeparator) {
     o << ',';
      } else {
     useSeparator = true;
      }
      o << (t.is() ? t : curr->default_);
    }
    o << ")," << curr->default_ << ',';
    visit(curr->value);
    o << '\n';
    for (auto& c : curr->cases) {
      indent--;
      doIndent(o, indent);
      o << "} : " << c.name << '\n';
      pullPushes(c.body);
      printFullLine(c.body);
    }
    indent--;
    doIndent(o, indent);
    o << '}';
    if (curr->name.is()) {
      o << " : " << curr->name;
    }
  }

  void printCallParams(CallBase *curr) {
    o << '(';
    bool useSeparator = false;
    for (auto operand : curr->operands) {
      if (useSeparator)    {
        o << ',';
      } else {
        useSeparator = true;
      }
      visit(operand);
    }
    o << ')';
  }

  void printCallBody(Call* curr) {
    o << curr->target;
    printCallParams(curr);
  }

  void pullOperandPushes(CallBase* curr) {
    for (auto operand : curr->operands) {
      visit(operand);
    }
  }

  bool maybeVisitPush(bool &saved_pulling_pushes) {
    saved_pulling_pushes = pulling_pushes;
    if (pulling_pushes) {
      expr_depth++;
      return true;
    }
    return false;
  }

  bool maybePop() {
    if (!pull_pushes) return false;
    if (expr_depth > 0) {
      o << "pop";
      return true;
    }
    return false;
  }

  bool maybeInsertPush() {
    expr_depth--;
    if (expr_depth == 0) return false;
    pulling_pushes = false;
    doIndent(o, indent);
    o << "push ";
    return true;
  }

  void maybeFinishPush(bool saved_pulling_pushes) {
    pulling_pushes = saved_pulling_pushes;
    if (pulling_pushes) printEndOfPush();
  }

  void visitCall(Call *curr) {
    bool saved_pulling_pushes;
    if (maybeVisitPush(saved_pulling_pushes)) {
      pullOperandPushes(curr);
      if (!maybeInsertPush()) return;
    } else if (maybePop()) {
      return;
    }
    expr_depth++;
    o << "call ";
    printCallBody(curr);
    expr_depth--;
    maybeFinishPush(saved_pulling_pushes);
  }
  void visitCallImport(CallImport *curr) {
    bool saved_pulling_pushes;
    if (maybeVisitPush(saved_pulling_pushes)) {
      pullOperandPushes(curr);
      if (!maybeInsertPush()) return;
    } else if (maybePop()) {
      return;
    }
    expr_depth++;
    o << "call_import ";
    printCallBody(curr);
    expr_depth--;
    maybeFinishPush(saved_pulling_pushes);
  }
  void visitCallIndirect(CallIndirect *curr) {
    bool saved_pulling_pushes;
    if (maybeVisitPush(saved_pulling_pushes)) {
      visit(curr->target);
      if (!maybeInsertPush()) return;
    } else if (maybePop()) {
      return;
    }
    expr_depth++;
    o << "call_indirect " << curr->fullType->name;
    o << ',';
    visit(curr->target);
    printCallParams(curr);
    expr_depth--;
    maybeFinishPush(saved_pulling_pushes);
  }
  void visitGetLocal(GetLocal *curr) {
    if (pulling_pushes) return;
    o << curr->name;
  }
  void visitSetLocal(SetLocal *curr) {
    bool saved_pulling_pushes;
    if (maybeVisitPush(saved_pulling_pushes)) {
      visit(curr->value);
      if (!maybeInsertPush()) return;
    } else if (maybePop()) {
      return;
    }
    expr_depth++;
    o << curr->name << " = ";
    visit(curr->value);
    expr_depth--;
    maybeFinishPush(saved_pulling_pushes);
  }
  void visitLoad(Load *curr) {
    if (pulling_pushes) {
      expr_depth++;
      visit(curr->ptr);
      expr_depth--;
      return;
    }
    if (expr_depth++) o << '(';
    o << printWasmType(curr->type) << ".load";
    if (curr->bytes < 4 || (curr->type == i64 && curr->bytes < 8)) {
      if (curr->bytes == 1) {
        o << '8';
      } else if (curr->bytes == 2) {
        o << "16";
      } else if (curr->bytes == 4) {
        o << "32";
      } else {
        abort();
      }
      o << (curr->signed_ ? "_s" : "_u");
    }
    o << " [";
    visit(curr->ptr);
    o << "," << curr->offset << "]";
    if (curr->align != curr->bytes) {
      o << "/" << curr->align;
    }
    if (--expr_depth) o << ')';
  }
  void visitStore(Store *curr) {
    bool saved_pulling_pushes;
    if (maybeVisitPush(saved_pulling_pushes)) {
      visit(curr->ptr);
      visit(curr->value);
      if (!maybeInsertPush()) return;
    } else if (maybePop()) {
      return;
    }
    if (expr_depth++) o << '(';
    o << printWasmType(curr->type) << ".store";
    if (curr->bytes < 4 || (curr->type == i64 && curr->bytes < 8)) {
      if (curr->bytes == 1) {
        o << '8';
      } else if (curr->bytes == 2) {
        o << "16";
      } else if (curr->bytes == 4) {
        o << "32";
      } else {
        abort();
      }
    }
    o << " [";
    visit(curr->ptr);
    o << "," << curr->offset << "]";
    if (curr->align != curr->bytes) {
      o << "/" << curr->align;
    }
    o << ",";
    visit(curr->value);
    if (--expr_depth) o << ')';
    maybeFinishPush(saved_pulling_pushes);
  }
  void visitConst(Const *curr) {
    if (pulling_pushes) return;
    Literal literal = curr->value;
    switch (literal.type) {
      case none: o << "?"; break;
      case WasmType::i32: o << literal.geti32(); break;
      case WasmType::i64: o << literal.geti64(); break;
      case WasmType::f32: literal.printFloat(o, literal.getf32()); break;
      case WasmType::f64: literal.printDouble(o, literal.getf64()); break;
      default: WASM_UNREACHABLE();
    }
  }
  void visitUnary(Unary *curr) {
    if (pulling_pushes) {
        expr_depth++;
      visit(curr->value);
        expr_depth--;
      return;
    }
    if (expr_depth++) o << '(';
    bool symbolic = false;
    if (curr->op == Neg) {
      symbolic = true;
    }
    if (!symbolic) {
      o << printWasmType(curr->type) << '.';
    }
    switch (curr->op) {
      case Clz:              o << "clz";     break;
      case Ctz:              o << "ctz";     break;
      case Popcnt:           o << "popcnt";  break;
      case Neg:              o << "-";    break;
      case Abs:              o << "abs";    break;
      case Ceil:             o << "ceil";    break;
      case Floor:            o << "floor";   break;
      case Trunc:            o << "trunc";   break;
      case Nearest:          o << "nearest"; break;
      case Sqrt:             o << "sqrt";    break;
      case ExtendSInt32:     o << "extend_s/i32"; break;
      case ExtendUInt32:     o << "extend_u/i32"; break;
      case WrapInt64:        o << "wrap/i64"; break;
      case TruncSFloat32:    o << "trunc_s/f32"; break;
      case TruncUFloat32:    o << "trunc_u/f32"; break;
      case TruncSFloat64:    o << "trunc_s/f64"; break;
      case TruncUFloat64:    o << "trunc_u/f64"; break;
      case ReinterpretFloat: o << "reinterpret/" << (curr->type == i64 ? "f64" : "f32"); break;
      case ConvertUInt32:    o << "convert_u/i32"; break;
      case ConvertSInt32:    o << "convert_s/i32"; break;
      case ConvertUInt64:    o << "convert_u/i64"; break;
      case ConvertSInt64:    o << "convert_s/i64"; break;
      case PromoteFloat32:   o << "promote/f32"; break;
      case DemoteFloat64:    o << "demote/f64"; break;
      case ReinterpretInt:   o << "reinterpret/" << (curr->type == f64 ? "i64" : "i32"); break;
      default: abort();
    }
    if (!symbolic) o << ' ';
    visit(curr->value);
    if (--expr_depth) o << ')';
  }
  void visitBinary(Binary *curr) {
    if (pulling_pushes) {
      expr_depth++;
      visit(curr->left);
      visit(curr->right);
      expr_depth--;
      return;
    }
    if (expr_depth++) o << '(';
    bool fullForm = true;
    switch (curr->op) {
      case CopySign: o << "copysign"; break;
      case Min:      o << "min";      break;
      case Max:      o << "max";      break;
      default:       fullForm = false; break;
    }
    if (fullForm) {
      o << ' ';
      o << '(';
      visit(curr->left);
      o << ')';
      o << ',';
      o << '(';
      visit(curr->right);
      o << ')';
      return;
    }
    visit(curr->left);
    o << ' ';
    switch (curr->op) {
      case Add:      o << "+";      break;
      case Sub:      o << "-";      break;
      case Mul:      o << "*";      break;
      case DivS:     o << "/s";    break;
      case DivU:     o << "/u";    break;
      case RemS:     o << "%s";    break;
      case RemU:     o << "%u";    break;
      case And:      o << "&";      break;
      case Or:       o << "|";       break;
      case Xor:      o << "^";      break;
      case Shl:      o << "<<";      break;
      case ShrU:     o << ">>";    break;
      case ShrS:     o << ">>>";    break;
      case Div:      o << "/";      break;
      case Eq:       o << "==";       break;
      case Ne:       o << "!=";       break;
      case LtS:      o << "<s";     break;
      case LtU:      o << "<u";     break;
      case LeS:      o << ">=s";     break;
      case LeU:      o << ">=u";     break;
      case GtS:      o << "<s";     break;
      case GtU:      o << "<u";     break;
      case GeS:      o << "<=s";     break;
      case GeU:      o << "<=u";     break;
      case Lt:       o << ">";       break;
      case Le:       o << ">=";       break;
      case Gt:       o << "<";       break;
      case Ge:       o << "<=";       break;
      default:       abort();
    }
    o << ' ';
    visit(curr->right);
    if (--expr_depth) o << ')';
  }
  void visitSelect(Select *curr) {
    if (pulling_pushes) {
        expr_depth++;
      visit(curr->ifTrue);
      visit(curr->ifFalse);
      visit(curr->condition);
        expr_depth--;
      return;
    }
    if (expr_depth++) o << '(';
    o << "select ";
    visit(curr->ifTrue);
    o << ',';
    visit(curr->ifFalse);
    o << ',';
    visit(curr->condition);
    if (--expr_depth) o << ')';
  }
  void visitReturn(Return *curr) {
    if (pulling_pushes) {
      if (curr->value && !curr->value->is<Nop>()) {
        pulling_pushes = false;
        pullPushes(curr->value);
        doIndent(o, indent);
        o << "push ";
        visit(curr->value);
        pulling_pushes = true;
        printEndOfPush();
      }
      return;
    }
    o << "return";
  }
  void visitHost(Host *curr) {
    switch (curr->op) {
      case PageSize: o << "pagesize"; break;
      case MemorySize: o << "memory_size"; break;
      case GrowMemory: {
        o << "grow_memory ";
        visit(curr->operands[0]);
        break;
      }
      case HasFeature: o << "hasfeature " << curr->nameOperand; break;
      default: abort();
    }
  }
  void visitNop(Nop *curr) {
    o << "nop";
  }
  void visitUnreachable(Unreachable *curr) {
    o << "unreachable";
  }
  // Module-level visitors
  void visitFunctionType(FunctionType *curr, bool full=false) {
      /*
    if (full) {
      printOpening(o, "type") << ' ' << curr->name << " (func";
    }
    if (curr->params.size() > 0) {
      o << ' ';
      printMinorOpening(o, "param");
      for (auto& param : curr->params) {
        o << ' ' << printWasmType(param);
      }
      o << ')';
    }
    if (curr->result != none) {
      o << ' ';
      printMinorOpening(o, "result ") << printWasmType(curr->result) << ')';
    }
    if (full) {
      o << "))";;
    }
    */
  }
  void visitImport(Import *curr) {
    o << "import " << curr->name << ' ';
    printText(o, curr->module.str) << ' ';
    printText(o, curr->base.str);
    if (curr->type) visitFunctionType(curr->type);
  }
  void visitExport(Export *curr) {
    o << "export ";
    printText(o, curr->name.str) << ' ' << curr->value;
  }
  void visitFunction(Function *curr) {
    o << "func " << curr->name;
    if (curr->type.is()) {
      //o << " (type " << curr->type << ')';
    }
    o << '(';
    bool useSeparator = false;
    if (curr->params.size() > 0) {
      for (auto& param : curr->params) {
        if (useSeparator) {
          o << ',';
        } else {
          useSeparator = true;
        }
        o << param.name << ':' << printWasmType(param.type);
      }
    }
    o << ')';
    if (curr->result != none) {
      o << " : (";
      o << printWasmType(curr->result);
      o << ')';
    }
    o << " {" << '\n';
    indent++;
    for (auto& local : curr->locals) {
      doIndent(o, indent);
      o << "local " << local.name << ':' << printWasmType(local.type);
      if (use_line_term) o << ';';
      o << '\n';
    }
    printFullLine(curr->body);
    indent--;
    doIndent(o, indent);
    o << '}';
  }
  void visitTable(Table *curr) {
/*
      printOpening(o, "table");
    for (auto name : curr->names) {
      o << ' ' << name;
    }
    o << ')';
    */
  }
  void visitModule(Module *curr) {
    o << ";; new module" << '\n';
    o << "memory" << " " << curr->memory.initial;
    if (curr->memory.max && curr->memory.max != (uint32_t)-1) o << "," << curr->memory.max;
    o << " {" << '\n';
    indent++;
    for (auto segment : curr->memory.segments) {
      doIndent(o, indent);
      o << "segment " << segment.offset << ",\"";
      for (size_t i = 0; i < segment.size; i++) {
        unsigned char c = segment.data[i];
        switch (c) {
          case '\n': o << "\\n"; break;
          case '\r': o << "\\0d"; break;
          case '\t': o << "\\t"; break;
          case '\f': o << "\\0c"; break;
          case '\b': o << "\\08"; break;
          case '\\': o << "\\\\"; break;
          case '"' : o << "\\\""; break;
          case '\'' : o << "\\'"; break;
          default: {
            if (c >= 32 && c < 127) {
              o << c;
            } else {
              o << std::hex << '\\' << (c/16) << (c%16) << std::dec;
            }
          }
        }
      }
      o << "\"" << '\n';
    }
    indent--;
    o << "}\n";
    if (curr->start.is()) {
      doIndent(o, indent);
      o << "start" << " " << curr->start << '\n';
    }
    for (auto& child : curr->functionTypes) {
      doIndent(o, indent);
      visitFunctionType(child, true);
      o << '\n';
    }
    for (auto& child : curr->imports) {
      doIndent(o, indent);
      visitImport(child);
      o << '\n';
    }
    for (auto& child : curr->exports) {
      doIndent(o, indent);
      visitExport(child);
      o << '\n';
    }
    if (curr->table.names.size() > 0) {
      doIndent(o, indent);
      visitTable(&curr->table);
      o << '\n';
    }
    for (auto& child : curr->functions) {
      o << '\n';
      doIndent(o, indent);
      visitFunction(child);
      o << '\n';
    }
    o << '\n';
  }
};

// Pass entry point. Eventually this will direct printing to one of various options.

void TextPrinter::run(PassRunner* runner, Module* module) {
  PrintText print(o);
  print.visitModule(module);
}

static RegisterPass<TextPrinter> registerPass("print-was", "print in text format");

} // namespace wasm
