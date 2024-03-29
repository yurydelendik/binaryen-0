/*
 * Copyright 2015 WebAssembly Community Group participants
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
// wasm.h: WebAssembly representation and processing library, in one
//         header file.
//
// This represents WebAssembly in an AST format, with a focus on making
// it easy to not just inspect but also to process. For example, some
// things that this enables are:
//
//  * Interpreting: See wasm-interpreter.h.
//  * Optimizing: See asm2wasm.h, which performs some optimizations
//                after code generation.
//  * Validation: See wasm-validator.h.
//  * Pretty-printing: See Print.cpp.
//

//
// wasm.js internal WebAssembly representation design:
//
//  * Unify where possible. Where size isn't a concern, combine
//    classes, so binary ops and relational ops are joined. This
//    simplifies that AST and makes traversals easier.
//  * Optimize for size? This might justify separating if and if_else
//    (so that if doesn't have an always-empty else; also it avoids
//    a branch).
//

#ifndef wasm_wasm_h
#define wasm_wasm_h

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "compiler-support.h"
#include "emscripten-optimizer/simple_ast.h"
#include "mixed_arena.h"
#include "pretty_printing.h"
#include "support/bits.h"
#include "support/utilities.h"

namespace wasm {

// We use a Name for all of the identifiers. These are IStrings, so they are
// all interned - comparisons etc are just pointer comparisons, so there is no
// perf loss. Having names everywhere makes using the AST much nicer.
// TODO: as an optimization, IString values < some threshold could be considered
//       numerical indices directly.

struct Name : public cashew::IString {
  Name() : cashew::IString() {}
  Name(const char *str) : cashew::IString(str, false) {}
  Name(cashew::IString str) : cashew::IString(str) {}
  Name(const std::string &str) : cashew::IString(str.c_str(), false) {}

  friend std::ostream& operator<<(std::ostream &o, Name name) {
    assert(name.str);
    return o << '$' << name.str; // reference interpreter requires we prefix all names
  }

  static Name fromInt(size_t i) {
    return cashew::IString(std::to_string(i).c_str(), false);
  }
};

// Types

enum WasmType {
  none,
  i32,
  i64,
  f32,
  f64,
  unreachable // none means no type, e.g. a block can have no return type. but
              // unreachable is different, as it can be "ignored" when doing
              // type checking across branches
};

inline const char* printWasmType(WasmType type) {
  switch (type) {
    case WasmType::none: return "none";
    case WasmType::i32: return "i32";
    case WasmType::i64: return "i64";
    case WasmType::f32: return "f32";
    case WasmType::f64: return "f64";
    case WasmType::unreachable: return "unreachable";
    default: WASM_UNREACHABLE();
  }
}

inline unsigned getWasmTypeSize(WasmType type) {
  switch (type) {
    case WasmType::none: abort();
    case WasmType::i32: return 4;
    case WasmType::i64: return 8;
    case WasmType::f32: return 4;
    case WasmType::f64: return 8;
    default: WASM_UNREACHABLE();
  }
}

inline bool isWasmTypeFloat(WasmType type) {
  switch (type) {
    case f32:
    case f64: return true;
    default: return false;
  }
}

inline WasmType getWasmType(unsigned size, bool float_) {
  if (size < 4) return WasmType::i32;
  if (size == 4) return float_ ? WasmType::f32 : WasmType::i32;
  if (size == 8) return float_ ? WasmType::f64 : WasmType::i64;
  abort();
}

inline WasmType getReachableWasmType(WasmType a, WasmType b) {
  return a != unreachable ? a : b;
}

inline bool isConcreteWasmType(WasmType type) {
  return type != none && type != unreachable;
}

// Literals

class Literal {
public:
  WasmType type;

private:
  // store only integers, whose bits are deterministic. floats
  // can have their signalling bit set, for example.
  union {
    int32_t i32;
    int64_t i64;
  };

public:
  Literal() : type(WasmType::none), i64(0) {}
  explicit Literal(WasmType type) : type(type), i64(0) {}
  explicit Literal(int32_t  init) : type(WasmType::i32), i32(init) {}
  explicit Literal(uint32_t init) : type(WasmType::i32), i32(init) {}
  explicit Literal(int64_t  init) : type(WasmType::i64), i64(init) {}
  explicit Literal(uint64_t init) : type(WasmType::i64), i64(init) {}
  explicit Literal(float    init) : type(WasmType::f32), i32(bit_cast<int32_t>(init)) {}
  explicit Literal(double   init) : type(WasmType::f64), i64(bit_cast<int64_t>(init)) {}

  Literal castToF32() {
    assert(type == WasmType::i32);
    Literal ret(i32);
    ret.type = WasmType::f32;
    return ret;
  }
  Literal castToF64() {
    assert(type == WasmType::i64);
    Literal ret(i64);
    ret.type = WasmType::f64;
    return ret;
  }
  Literal castToI32() {
    assert(type == WasmType::f32);
    Literal ret(i32);
    ret.type = WasmType::i32;
    return ret;
  }
  Literal castToI64() {
    assert(type == WasmType::f64);
    Literal ret(i64);
    ret.type = WasmType::i64;
    return ret;
  }

  int32_t geti32() const { assert(type == WasmType::i32); return i32; }
  int64_t geti64() const { assert(type == WasmType::i64); return i64; }
  float   getf32() const { assert(type == WasmType::f32); return bit_cast<float>(i32); }
  double  getf64() const { assert(type == WasmType::f64); return bit_cast<double>(i64); }

  int32_t* geti32Ptr() { assert(type == WasmType::i32); return &i32; } // careful!

  int32_t reinterpreti32() const { assert(type == WasmType::f32); return i32; }
  int64_t reinterpreti64() const { assert(type == WasmType::f64); return i64; }
  float   reinterpretf32() const { assert(type == WasmType::i32); return bit_cast<float>(i32); }
  double  reinterpretf64() const { assert(type == WasmType::i64); return bit_cast<double>(i64); }

  int64_t getInteger() {
    switch (type) {
      case WasmType::i32: return i32;
      case WasmType::i64: return i64;
      default: abort();
    }
  }

  double getFloat() {
    switch (type) {
      case WasmType::f32: return getf32();
      case WasmType::f64: return getf64();
      default: abort();
    }
  }

  bool operator==(const Literal& other) const {
    if (type != other.type) return false;
    switch (type) {
      case WasmType::none: return true;
      case WasmType::i32: return i32 == other.i32;
      case WasmType::f32: return getf32() == other.getf32();
      case WasmType::i64: return i64 == other.i64;
      case WasmType::f64: return getf64() == other.getf64();
      default: abort();
    }
  }

  static void printFloat(std::ostream &o, float f) {
    if (isnan(f)) {
      const char *sign = std::signbit(f) ? "-" : "";
      o << sign << "nan";
      if (uint32_t payload = ~0xff800000u & bit_cast<uint32_t>(f)) {
        o << ":0x" << std::hex << payload << std::dec;
      }
      return;
    }
    printDouble(o, f);
  }

  static void printDouble(std::ostream &o, double d) {
    if (d == 0 && std::signbit(d)) {
      o << "-0";
      return;
    }
    if (isnan(d)) {
      const char *sign = std::signbit(d) ? "-" : "";
      o << sign << "nan";
      if (uint64_t payload = ~0xfff0000000000000ull & bit_cast<uint64_t>(d)) {
        o << ":0x" << std::hex << payload << std::dec;
      }
      return;
    }
    if (!std::isfinite(d)) {
      o << (std::signbit(d) ? "-infinity" : "infinity");
      return;
    }
    const char *text = cashew::JSPrinter::numToString(d);
    // spec interpreter hates floats starting with '.'
    if (text[0] == '.') {
      o << '0';
    } else if (text[0] == '-' && text[1] == '.') {
      o << "-0";
      text++;
    }
    o << text;
  }

  friend std::ostream& operator<<(std::ostream &o, Literal literal) {
    o << '(';
    prepareMinorColor(o) << printWasmType(literal.type) << ".const ";
    switch (literal.type) {
      case none: o << "?"; break;
      case WasmType::i32: o << literal.i32; break;
      case WasmType::i64: o << literal.i64; break;
      case WasmType::f32: literal.printFloat(o, literal.getf32()); break;
      case WasmType::f64: literal.printDouble(o, literal.getf64()); break;
      default: WASM_UNREACHABLE();
    }
    restoreNormalColor(o);
    return o << ')';
  }

  Literal countLeadingZeroes() const {
    if (type == WasmType::i32) return Literal((int32_t)CountLeadingZeroes(i32));
    if (type == WasmType::i64) return Literal((int64_t)CountLeadingZeroes(i64));
    WASM_UNREACHABLE();
  }
  Literal countTrailingZeroes() const {
    if (type == WasmType::i32) return Literal((int32_t)CountTrailingZeroes(i32));
    if (type == WasmType::i64) return Literal((int64_t)CountTrailingZeroes(i64));
    WASM_UNREACHABLE();
  }
  Literal popCount() const {
    if (type == WasmType::i32) return Literal((int32_t)PopCount(i32));
    if (type == WasmType::i64) return Literal((int64_t)PopCount(i64));
    WASM_UNREACHABLE();
  }

  Literal extendToSI64() const {
    assert(type == WasmType::i32);
    return Literal((int64_t)i32);
  }
  Literal extendToUI64() const {
    assert(type == WasmType::i32);
    return Literal((uint64_t)(uint32_t)i32);
  }
  Literal extendToF64() const {
    assert(type == WasmType::f32);
    return Literal(double(getf32()));
  }
  Literal truncateToI32() const {
    assert(type == WasmType::i64);
    return Literal((int32_t)i64);
  }
  Literal truncateToF32() const {
    assert(type == WasmType::f64);
    return Literal(float(getf64()));
  }

  Literal convertSToF32() const {
    if (type == WasmType::i32) return Literal(float(i32));
    if (type == WasmType::i64) return Literal(float(i64));
    WASM_UNREACHABLE();
  }
  Literal convertUToF32() const {
    if (type == WasmType::i32) return Literal(float(uint32_t(i32)));
    if (type == WasmType::i64) return Literal(float(uint64_t(i64)));
    WASM_UNREACHABLE();
  }
  Literal convertSToF64() const {
    if (type == WasmType::i32) return Literal(double(i32));
    if (type == WasmType::i64) return Literal(double(i64));
    WASM_UNREACHABLE();
  }
  Literal convertUToF64() const {
    if (type == WasmType::i32) return Literal(double(uint32_t(i32)));
    if (type == WasmType::i64) return Literal(double(uint64_t(i64)));
    WASM_UNREACHABLE();
  }

  Literal neg() const {
    switch (type) {
      case WasmType::i32: return Literal(i32 ^ 0x80000000);
      case WasmType::i64: return Literal(int64_t(i64 ^ 0x8000000000000000ULL));
      case WasmType::f32: return Literal(i32 ^ 0x80000000).castToF32();
      case WasmType::f64: return Literal(int64_t(i64 ^ 0x8000000000000000ULL)).castToF64();
      default: WASM_UNREACHABLE();
    }
  }
  Literal abs() const {
    switch (type) {
      case WasmType::i32: return Literal(i32 & 0x7fffffff);
      case WasmType::i64: return Literal(int64_t(i64 & 0x7fffffffffffffffULL));
      case WasmType::f32: return Literal(i32 & 0x7fffffff).castToF32();
      case WasmType::f64: return Literal(int64_t(i64 & 0x7fffffffffffffffULL)).castToF64();
      default: WASM_UNREACHABLE();
    }
  }
  Literal ceil() const {
    switch (type) {
      case WasmType::f32: return Literal(std::ceil(getf32()));
      case WasmType::f64: return Literal(std::ceil(getf64()));
      default: WASM_UNREACHABLE();
    }
  }
  Literal floor() const {
    switch (type) {
      case WasmType::f32: return Literal(std::floor(getf32()));
      case WasmType::f64: return Literal(std::floor(getf64()));
      default: WASM_UNREACHABLE();
    }
  }
  Literal trunc() const {
    switch (type) {
      case WasmType::f32: return Literal(std::trunc(getf32()));
      case WasmType::f64: return Literal(std::trunc(getf64()));
      default: WASM_UNREACHABLE();
    }
  }
  Literal nearbyint() const {
    switch (type) {
      case WasmType::f32: return Literal(std::nearbyint(getf32()));
      case WasmType::f64: return Literal(std::nearbyint(getf64()));
      default: WASM_UNREACHABLE();
    }
  }
  Literal sqrt() const {
    switch (type) {
      case WasmType::f32: return Literal(std::sqrt(getf32()));
      case WasmType::f64: return Literal(std::sqrt(getf64()));
      default: WASM_UNREACHABLE();
    }
  }

  Literal add(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 + other.i32);
      case WasmType::i64: return Literal(i64 + other.i64);
      case WasmType::f32: return Literal(getf32() + other.getf32());
      case WasmType::f64: return Literal(getf64() + other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal sub(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 - other.i32);
      case WasmType::i64: return Literal(i64 - other.i64);
      case WasmType::f32: return Literal(getf32() - other.getf32());
      case WasmType::f64: return Literal(getf64() - other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal mul(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 * other.i32);
      case WasmType::i64: return Literal(i64 * other.i64);
      case WasmType::f32: return Literal(getf32() * other.getf32());
      case WasmType::f64: return Literal(getf64() * other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal div(const Literal& other) const {
    switch (type) {
      case WasmType::f32: return Literal(getf32() / other.getf32());
      case WasmType::f64: return Literal(getf64() / other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal divS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 / other.i32);
      case WasmType::i64: return Literal(i64 / other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal divU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) / uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) / uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal remS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 % other.i32);
      case WasmType::i64: return Literal(i64 % other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal remU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) % uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) % uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal and_(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 & other.i32);
      case WasmType::i64: return Literal(i64 & other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal or_(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 | other.i32);
      case WasmType::i64: return Literal(i64 | other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal xor_(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 ^ other.i32);
      case WasmType::i64: return Literal(i64 ^ other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal shl(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 << other.i32);
      case WasmType::i64: return Literal(i64 << other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal shrS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 >> other.i32);
      case WasmType::i64: return Literal(i64 >> other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal shrU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) >> uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) >> uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }

  Literal eq(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 == other.i32);
      case WasmType::i64: return Literal(i64 == other.i64);
      case WasmType::f32: return Literal(getf32() == other.getf32());
      case WasmType::f64: return Literal(getf64() == other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal ne(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 != other.i32);
      case WasmType::i64: return Literal(i64 != other.i64);
      case WasmType::f32: return Literal(getf32() != other.getf32());
      case WasmType::f64: return Literal(getf64() != other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal ltS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 < other.i32);
      case WasmType::i64: return Literal(i64 < other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal ltU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) < uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) < uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal lt(const Literal& other) const {
    switch (type) {
      case WasmType::f32: return Literal(getf32() < other.getf32());
      case WasmType::f64: return Literal(getf64() < other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal leS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 <= other.i32);
      case WasmType::i64: return Literal(i64 <= other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal leU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) <= uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) <= uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal le(const Literal& other) const {
    switch (type) {
      case WasmType::f32: return Literal(getf32() <= other.getf32());
      case WasmType::f64: return Literal(getf64() <= other.getf64());
      default: WASM_UNREACHABLE();
    }
  }

  Literal gtS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 > other.i32);
      case WasmType::i64: return Literal(i64 > other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal gtU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) > uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) > uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal gt(const Literal& other) const {
    switch (type) {
      case WasmType::f32: return Literal(getf32() > other.getf32());
      case WasmType::f64: return Literal(getf64() > other.getf64());
      default: WASM_UNREACHABLE();
    }
  }
  Literal geS(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(i32 >= other.i32);
      case WasmType::i64: return Literal(i64 >= other.i64);
      default: WASM_UNREACHABLE();
    }
  }
  Literal geU(const Literal& other) const {
    switch (type) {
      case WasmType::i32: return Literal(uint32_t(i32) >= uint32_t(other.i32));
      case WasmType::i64: return Literal(uint64_t(i64) >= uint64_t(other.i64));
      default: WASM_UNREACHABLE();
    }
  }
  Literal ge(const Literal& other) const {
    switch (type) {
      case WasmType::f32: return Literal(getf32() >= other.getf32());
      case WasmType::f64: return Literal(getf64() >= other.getf64());
      default: WASM_UNREACHABLE();
    }
  }

  Literal min(const Literal& other) const {
    switch (type) {
      case WasmType::f32: {
        auto l = getf32(), r = other.getf32();
        if (l == r && l == 0) return Literal(std::signbit(l) ? l : r);
        auto result = std::min(l, r);
        bool lnan = isnan(l), rnan = isnan(r);
        if (!isnan(result) && !lnan && !rnan) return Literal(result);
        if (!lnan && !rnan) return Literal((int32_t)0x7fc00000).castToF32();
        return Literal(lnan ? l : r).castToI32().or_(Literal(0xc00000)).castToF32();
      }
      case WasmType::f64: {
        auto l = getf64(), r = other.getf64();
        if (l == r && l == 0) return Literal(std::signbit(l) ? l : r);
        auto result = std::min(l, r);
        bool lnan = isnan(l), rnan = isnan(r);
        if (!isnan(result) && !lnan && !rnan) return Literal(result);
        if (!lnan && !rnan) return Literal((int64_t)0x7ff8000000000000LL).castToF64();
        return Literal(lnan ? l : r).castToI64().or_(Literal(int64_t(0x8000000000000LL))).castToF64();
      }
      default: WASM_UNREACHABLE();
    }
  }
  Literal max(const Literal& other) const {
    switch (type) {
      case WasmType::f32: {
        auto l = getf32(), r = other.getf32();
        if (l == r && l == 0) return Literal(std::signbit(l) ? r : l);
        auto result = std::max(l, r);
        bool lnan = isnan(l), rnan = isnan(r);
        if (!isnan(result) && !lnan && !rnan) return Literal(result);
        if (!lnan && !rnan) return Literal((int32_t)0x7fc00000).castToF32();
        return Literal(lnan ? l : r).castToI32().or_(Literal(0xc00000)).castToF32();
      }
      case WasmType::f64: {
        auto l = getf64(), r = other.getf64();
        if (l == r && l == 0) return Literal(std::signbit(l) ? r : l);
        auto result = std::max(l, r);
        bool lnan = isnan(l), rnan = isnan(r);
        if (!isnan(result) && !lnan && !rnan) return Literal(result);
        if (!lnan && !rnan) return Literal((int64_t)0x7ff8000000000000LL).castToF64();
        return Literal(lnan ? l : r).castToI64().or_(Literal(int64_t(0x8000000000000LL))).castToF64();
      }
      default: WASM_UNREACHABLE();
    }
  }
  Literal copysign(const Literal& other) const {
    // operate on bits directly, to avoid signalling bit being set on a float
    switch (type) {
      case WasmType::f32: return Literal((i32 & 0x7fffffff) | (other.i32 & 0x80000000)).castToF32(); break;
      case WasmType::f64: return Literal((i64 & 0x7fffffffffffffffUL) | (other.i64 & 0x8000000000000000UL)).castToF64(); break;
      default: WASM_UNREACHABLE();
    }
  }
};

// Operators

enum UnaryOp {
  Clz, Ctz, Popcnt, // int
  Neg, Abs, Ceil, Floor, Trunc, Nearest, Sqrt, // float
  // conversions
  ExtendSInt32, ExtendUInt32, WrapInt64, TruncSFloat32, TruncUFloat32, TruncSFloat64, TruncUFloat64, ReinterpretFloat, // int
  ConvertSInt32, ConvertUInt32, ConvertSInt64, ConvertUInt64, PromoteFloat32, DemoteFloat64, ReinterpretInt // float
};

enum BinaryOp {
  Add, Sub, Mul, // int or float
  DivS, DivU, RemS, RemU, And, Or, Xor, Shl, ShrU, ShrS, // int
  Div, CopySign, Min, Max, // float
  // relational ops
  Eq, Ne, // int or float
  LtS, LtU, LeS, LeU, GtS, GtU, GeS, GeU, // int
  Lt, Le, Gt, Ge // float
};

enum HostOp {
  PageSize, MemorySize, GrowMemory, HasFeature
};

//
// Expressions
//
// Note that little is provided in terms of constructors for these. The rationale
// is that writing  new Something(a, b, c, d, e)  is not the clearest, and it would
// be better to write   new Something(name=a, leftOperand=b...  etc., but C++
// lacks named operands, so in asm2wasm etc. you will see things like
//   auto x = new Something();
//   x->name = a;
//   x->leftOperand = b;
//   ..
// which is less compact but less ambiguous. But hopefully we can do better,
// suggestions for API improvements here are welcome.
//

class Expression {
public:
  enum Id {
    InvalidId = 0,
    BlockId,
    IfId,
    LoopId,
    BreakId,
    SwitchId,
    CallId,
    CallImportId,
    CallIndirectId,
    GetLocalId,
    SetLocalId,
    LoadId,
    StoreId,
    ConstId,
    UnaryId,
    BinaryId,
    SelectId,
    ReturnId,
    HostId,
    NopId,
    UnreachableId
  };
  Id _id;

  WasmType type; // the type of the expression: its *output*, not necessarily its input(s)

  Expression() : _id(InvalidId), type(none) {}
  Expression(Id id) : _id(id), type(none) {}

  template<class T>
  bool is() {
    return _id == T()._id;
  }

  template<class T>
  T* dyn_cast() {
    return _id == T()._id ? (T*)this : nullptr;
  }

  template<class T>
  T* cast() {
    assert(_id == T()._id);
    return (T*)this;
  }
};

inline const char *getExpressionName(Expression *curr) {
  switch (curr->_id) {
    case Expression::Id::InvalidId: abort();
    case Expression::Id::BlockId: return "block";
    case Expression::Id::IfId: return "if";
    case Expression::Id::LoopId: return "loop";
    case Expression::Id::BreakId: return "break";
    case Expression::Id::SwitchId: return "switch";
    case Expression::Id::CallId: return "call";
    case Expression::Id::CallImportId: return "call_import";
    case Expression::Id::CallIndirectId: return "call_indirect";
    case Expression::Id::GetLocalId: return "get_local";
    case Expression::Id::SetLocalId: return "set_local";
    case Expression::Id::LoadId: return "load";
    case Expression::Id::StoreId: return "store";
    case Expression::Id::ConstId: return "const";
    case Expression::Id::UnaryId: return "unary";
    case Expression::Id::BinaryId: return "binary";
    case Expression::Id::SelectId: return "select";
    case Expression::Id::ReturnId: return "return";
    case Expression::Id::HostId: return "host";
    case Expression::Id::NopId: return "nop";
    case Expression::Id::UnreachableId: return "unreachable";
    default: WASM_UNREACHABLE();
  }
}

typedef std::vector<Expression*> ExpressionList; // TODO: optimize?

class Nop : public Expression {
public:
  Nop() : Expression(NopId) {}
};

class Block : public Expression {
public:
  Block() : Expression(BlockId) {
    type = none; // blocks by default do not return, but if their last statement does, they might
  }

  Name name;
  ExpressionList list;
};

class If : public Expression {
public:
  If() : Expression(IfId), ifFalse(nullptr) {
    type = none; // by default none; if-else can have one, though
  }

  Expression *condition, *ifTrue, *ifFalse;

  void finalize() {
    if (condition) {
      type = getReachableWasmType(ifTrue->type, ifFalse->type);
    }
  }
};

class Loop : public Expression {
public:
  Loop() : Expression(LoopId) {}

  Name out, in;
  Expression *body;

  void finalize() {
    type = body->type; // loop might have a type, if the body ends in something that does not break
  }
};

class Break : public Expression {
public:
  Break() : Expression(BreakId), value(nullptr), condition(nullptr) {
    type = unreachable;
  }

  Name name;
  Expression *value;
  Expression *condition;
};

class Switch : public Expression {
public:
  Switch() : Expression(SwitchId) {}

  struct Case {
    Name name;
    Expression *body;
    Case() {}
    Case(Name name, Expression *body) : name(name), body(body) {}
  };

  Name name;
  Expression *value;
  std::vector<Name> targets;
  Name default_;
  std::vector<Case> cases;
};

class CallBase : public Expression {
public:
  CallBase(Id which) : Expression(which) {}

  ExpressionList operands;
};

class Call : public CallBase {
public:
  Call() : CallBase(CallId) {}

  Name target;
};

class CallImport : public Call {
public:
  CallImport() {
    _id = CallImportId;
  }
};

class FunctionType {
public:
  Name name;
  WasmType result;
  std::vector<WasmType> params;

  FunctionType() : result(none) {}

  bool operator==(FunctionType& b) {
    if (name != b.name) return false; // XXX
    if (result != b.result) return false;
    if (params.size() != b.params.size()) return false;
    for (size_t i = 0; i < params.size(); i++) {
      if (params[i] != b.params[i]) return false;
    }
    return true;
  }
  bool operator!=(FunctionType& b) {
    return !(*this == b);
  }
};

class CallIndirect : public CallBase {
public:
  CallIndirect() : CallBase(CallIndirectId) {}

  FunctionType *fullType;
  Expression *target;
};

class GetLocal : public Expression {
public:
  GetLocal() : Expression(GetLocalId) {}

  Name name;
};

class SetLocal : public Expression {
public:
  SetLocal() : Expression(SetLocalId) {}

  Name name;
  Expression *value;
};

class Load : public Expression {
public:
  Load() : Expression(LoadId) {}

  uint32_t bytes;
  bool signed_;
  uint32_t offset;
  uint32_t align;
  Expression *ptr;
};

class Store : public Expression {
public:
  Store() : Expression(StoreId) {}

  unsigned bytes;
  uint32_t offset;
  unsigned align;
  Expression *ptr, *value;
};

class Const : public Expression {
public:
  Const() : Expression(ConstId) {}

  Literal value;

  Const* set(Literal value_) {
    value = value_;
    type = value.type;
    return this;
  }
};

class Unary : public Expression {
public:
  Unary() : Expression(UnaryId) {}

  UnaryOp op;
  Expression *value;
};

class Binary : public Expression {
public:
  Binary() : Expression(BinaryId) {}

  BinaryOp op;
  Expression *left, *right;

  // the type is always the type of the operands,
  // except for relationals

  bool isRelational() { return op >= Eq; }

  void finalize() {
    if (isRelational()) {
      type = i32;
    } else {
      assert(left->type != unreachable && right->type != unreachable ? left->type == right->type : true);
      type = getReachableWasmType(left->type, right->type);
    }
  }
};

class Select : public Expression {
public:
  Select() : Expression(SelectId) {}

  Expression *ifTrue, *ifFalse, *condition;

  void finalize() {
    type = getReachableWasmType(ifTrue->type, ifFalse->type);
  }
};

class Return : public Expression {
public:
  Expression *value;

  Return() : Expression(ReturnId), value(nullptr) {
    type = unreachable;
  }
};

class Host : public Expression {
public:
  Host() : Expression(HostId) {}

  HostOp op;
  Name nameOperand;
  ExpressionList operands;

  void finalize() {
    switch (op) {
      case PageSize: case MemorySize: case HasFeature: {
        type = i32;
        break;
      }
      case GrowMemory: {
        type = i32;
        break;
      }
      default: abort();
    }
  }
};

class Unreachable : public Expression {
public:
  Unreachable() : Expression(UnreachableId) {
    type = unreachable;
  }
};

// Globals

struct NameType {
  Name name;
  WasmType type;
  NameType() : name(nullptr), type(none) {}
  NameType(Name name, WasmType type) : name(name), type(type) {}
};

class Function {
public:
  Name name;
  WasmType result;
  std::vector<NameType> params;
  std::vector<NameType> locals;
  Name type; // if null, it is implicit in params and result
  Expression *body;

  Function() : result(none) {}
};

class Import {
public:
  Name name, module, base; // name = module.base
  FunctionType* type;

  Import() : type(nullptr) {}
};

class Export {
public:
  Name name;  // exported name
  Name value; // internal name
};

class Table {
public:
  std::vector<Name> names;
};

class Memory {
public:
  struct Segment {
    size_t offset;
    const char* data;
    size_t size;
    Segment() {}
    Segment(size_t offset, const char *data, size_t size) : offset(offset), data(data), size(size) {}
  };

  size_t initial, max;
  std::vector<Segment> segments;

  Memory() : initial(0), max((uint32_t)-1) {}
};

class Module {
public:
  // internal wasm contents (don't access these from outside; use add*() and the *Map objects)
  std::vector<FunctionType*> functionTypes;
  std::vector<Import*> imports;
  std::vector<Export*> exports;
  std::vector<Function*> functions;

  // publicly-accessible content
  std::map<Name, FunctionType*> functionTypesMap;
  std::map<Name, Import*> importsMap;
  std::map<Name, Export*> exportsMap;
  std::map<Name, Function*> functionsMap;
  Table table;
  Memory memory;
  Name start;

  Module() : functionTypeIndex(0), importIndex(0), exportIndex(0), functionIndex(0) {}

  void addFunctionType(FunctionType* curr) {
    Name numericName = Name::fromInt(functionTypeIndex);
    if (curr->name.isNull()) {
      curr->name = numericName;
    }
    functionTypes.push_back(curr);
    functionTypesMap[curr->name] = curr;
    functionTypesMap[numericName] = curr;
    functionTypeIndex++;
  }
  void addImport(Import* curr) {
    Name numericName = Name::fromInt(importIndex);
    if (curr->name.isNull()) {
      curr->name = numericName;
    }
    imports.push_back(curr);
    importsMap[curr->name] = curr;
    importsMap[numericName] = curr;
    importIndex++;
  }
  void addExport(Export* curr) {
    Name numericName = Name::fromInt(exportIndex);
    if (curr->name.isNull()) {
      curr->name = numericName;
    }
    exports.push_back(curr);
    exportsMap[curr->name] = curr;
    exportsMap[numericName] = curr;
    exportIndex++;
  }
  void addFunction(Function* curr) {
    Name numericName = Name::fromInt(functionIndex);
    if (curr->name.isNull()) {
      curr->name = numericName;
    }
    functions.push_back(curr);
    functionsMap[curr->name] = curr;
    functionsMap[numericName] = curr;
    functionIndex++;
  }
  void addStart(const Name &s) {
    start = s;
  }

  void removeImport(Name name) {
    for (size_t i = 0; i < imports.size(); i++) {
      if (imports[i]->name == name) {
        imports.erase(imports.begin() + i);
        break;
      }
    }
    importsMap.erase(name);
  }

private:
  size_t functionTypeIndex, importIndex, exportIndex, functionIndex;
};

class AllocatingModule : public Module {
 public:
  MixedArena allocator;
};

//
// WebAssembly AST visitor. Useful for anything that wants to do something
// different for each AST node type, like printing, interpreting, etc.
//
// This class is specifically designed as a template to avoid virtual function
// call overhead. To write a visitor, derive from this class as follows:
//
//   struct MyVisitor : public WasmVisitor<MyVisitor> { .. }
//

template<typename SubType, typename ReturnType>
struct WasmVisitor {
  virtual ~WasmVisitor() {}
  // should be pure virtual, but https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51048
  // Expression visitors
  ReturnType visitBlock(Block *curr) { abort(); }
  ReturnType visitIf(If *curr) { abort(); }
  ReturnType visitLoop(Loop *curr) { abort(); }
  ReturnType visitBreak(Break *curr) { abort(); }
  ReturnType visitSwitch(Switch *curr) { abort(); }
  ReturnType visitCall(Call *curr) { abort(); }
  ReturnType visitCallImport(CallImport *curr) { abort(); }
  ReturnType visitCallIndirect(CallIndirect *curr) { abort(); }
  ReturnType visitGetLocal(GetLocal *curr) { abort(); }
  ReturnType visitSetLocal(SetLocal *curr) { abort(); }
  ReturnType visitLoad(Load *curr) { abort(); }
  ReturnType visitStore(Store *curr) { abort(); }
  ReturnType visitConst(Const *curr) { abort(); }
  ReturnType visitUnary(Unary *curr) { abort(); }
  ReturnType visitBinary(Binary *curr) { abort(); }
  ReturnType visitSelect(Select *curr) { abort(); }
  ReturnType visitReturn(Return *curr) { abort(); }
  ReturnType visitHost(Host *curr) { abort(); }
  ReturnType visitNop(Nop *curr) { abort(); }
  ReturnType visitUnreachable(Unreachable *curr) { abort(); }
  // Module-level visitors
  ReturnType visitFunctionType(FunctionType *curr) { abort(); }
  ReturnType visitImport(Import *curr) { abort(); }
  ReturnType visitExport(Export *curr) { abort(); }
  ReturnType visitFunction(Function *curr) { abort(); }
  ReturnType visitTable(Table *curr) { abort(); }
  ReturnType visitMemory(Memory *curr) { abort(); }
  ReturnType visitModule(Module *curr) { abort(); }

#define DELEGATE(CLASS_TO_VISIT) \
  return static_cast<SubType*>(this)-> \
      visit##CLASS_TO_VISIT(static_cast<CLASS_TO_VISIT*>(curr))

  ReturnType visit(Expression *curr) {
    assert(curr);
    switch (curr->_id) {
      case Expression::Id::InvalidId: abort();
      case Expression::Id::BlockId: DELEGATE(Block);
      case Expression::Id::IfId: DELEGATE(If);
      case Expression::Id::LoopId: DELEGATE(Loop);
      case Expression::Id::BreakId: DELEGATE(Break);
      case Expression::Id::SwitchId: DELEGATE(Switch);
      case Expression::Id::CallId: DELEGATE(Call);
      case Expression::Id::CallImportId: DELEGATE(CallImport);
      case Expression::Id::CallIndirectId: DELEGATE(CallIndirect);
      case Expression::Id::GetLocalId: DELEGATE(GetLocal);
      case Expression::Id::SetLocalId: DELEGATE(SetLocal);
      case Expression::Id::LoadId: DELEGATE(Load);
      case Expression::Id::StoreId: DELEGATE(Store);
      case Expression::Id::ConstId: DELEGATE(Const);
      case Expression::Id::UnaryId: DELEGATE(Unary);
      case Expression::Id::BinaryId: DELEGATE(Binary);
      case Expression::Id::SelectId: DELEGATE(Select);
      case Expression::Id::ReturnId: DELEGATE(Return);
      case Expression::Id::HostId: DELEGATE(Host);
      case Expression::Id::NopId: DELEGATE(Nop);
      case Expression::Id::UnreachableId: DELEGATE(Unreachable);
      default: WASM_UNREACHABLE();
    }
  }
};

//
// Base class for all WasmWalkers
//
template<typename SubType, typename ReturnType = void>
struct WasmWalkerBase : public WasmVisitor<SubType, ReturnType> {
  virtual void walk(Expression*& curr) { abort(); }
  virtual void startWalk(Function *func) { abort(); }
  virtual void startWalk(Module *module) { abort(); }
};

template<typename ParentType>
struct ChildWalker : public WasmWalkerBase<ChildWalker<ParentType>> {
  ParentType& parent;

  ChildWalker(ParentType& parent) : parent(parent) {}

  void visitBlock(Block *curr) {
    ExpressionList& list = curr->list;
    for (size_t z = 0; z < list.size(); z++) {
      parent.walk(list[z]);
    }
  }
  void visitIf(If *curr) {
    parent.walk(curr->condition);
    parent.walk(curr->ifTrue);
    parent.walk(curr->ifFalse);
  }
  void visitLoop(Loop *curr) {
    parent.walk(curr->body);
  }
  void visitBreak(Break *curr) {
    parent.walk(curr->condition);
    parent.walk(curr->value);
  }
  void visitSwitch(Switch *curr) {
    parent.walk(curr->value);
    for (auto& case_ : curr->cases) {
      parent.walk(case_.body);
    }
  }
  void visitCall(Call *curr) {
    ExpressionList& list = curr->operands;
    for (size_t z = 0; z < list.size(); z++) {
      parent.walk(list[z]);
    }
  }
  void visitCallImport(CallImport *curr) {
    ExpressionList& list = curr->operands;
    for (size_t z = 0; z < list.size(); z++) {
      parent.walk(list[z]);
    }
  }
  void visitCallIndirect(CallIndirect *curr) {
    parent.walk(curr->target);
    ExpressionList& list = curr->operands;
    for (size_t z = 0; z < list.size(); z++) {
      parent.walk(list[z]);
    }
  }
  void visitGetLocal(GetLocal *curr) {}
  void visitSetLocal(SetLocal *curr) {
    parent.walk(curr->value);
  }
  void visitLoad(Load *curr) {
    parent.walk(curr->ptr);
  }
  void visitStore(Store *curr) {
    parent.walk(curr->ptr);
    parent.walk(curr->value);
  }
  void visitConst(Const *curr) {}
  void visitUnary(Unary *curr) {
    parent.walk(curr->value);
  }
  void visitBinary(Binary *curr) {
    parent.walk(curr->left);
    parent.walk(curr->right);
  }
  void visitSelect(Select *curr) {
    parent.walk(curr->ifTrue);
    parent.walk(curr->ifFalse);
    parent.walk(curr->condition);
  }
  void visitReturn(Return *curr) {
    parent.walk(curr->value);
  }
  void visitHost(Host *curr) {
    ExpressionList& list = curr->operands;
    for (size_t z = 0; z < list.size(); z++) {
      parent.walk(list[z]);
    }
  }
  void visitNop(Nop *curr) {}
  void visitUnreachable(Unreachable *curr) {}
};

//
// Simple WebAssembly children-first walking (i.e., post-order, if you look
// at the children as subtrees of the current node), with the ability to replace
// the current expression node. Useful for writing optimization passes.
//

template<typename SubType, typename ReturnType = void>
struct WasmWalker : public WasmWalkerBase<SubType, ReturnType> {
  Expression* replace;

  WasmWalker() : replace(nullptr) {}

  // the visit* methods can call this to replace the current node
  void replaceCurrent(Expression *expression) {
    replace = expression;
  }

  // By default, do nothing
  ReturnType visitBlock(Block *curr) {}
  ReturnType visitIf(If *curr) {}
  ReturnType visitLoop(Loop *curr) {}
  ReturnType visitBreak(Break *curr) {}
  ReturnType visitSwitch(Switch *curr) {}
  ReturnType visitCall(Call *curr) {}
  ReturnType visitCallImport(CallImport *curr) {}
  ReturnType visitCallIndirect(CallIndirect *curr) {}
  ReturnType visitGetLocal(GetLocal *curr) {}
  ReturnType visitSetLocal(SetLocal *curr) {}
  ReturnType visitLoad(Load *curr) {}
  ReturnType visitStore(Store *curr) {}
  ReturnType visitConst(Const *curr) {}
  ReturnType visitUnary(Unary *curr) {}
  ReturnType visitBinary(Binary *curr) {}
  ReturnType visitSelect(Select *curr) {}
  ReturnType visitReturn(Return *curr) {}
  ReturnType visitHost(Host *curr) {}
  ReturnType visitNop(Nop *curr) {}
  ReturnType visitUnreachable(Unreachable *curr) {}

  ReturnType visitFunctionType(FunctionType *curr) {}
  ReturnType visitImport(Import *curr) {}
  ReturnType visitExport(Export *curr) {}
  ReturnType visitFunction(Function *curr) {}
  ReturnType visitTable(Table *curr) {}
  ReturnType visitMemory(Memory *curr) {}
  ReturnType visitModule(Module *curr) {}

  // children-first
  void walk(Expression*& curr) override {
    if (!curr) return;

    ChildWalker<WasmWalker<SubType, ReturnType>>(*this).visit(curr);

    this->visit(curr);

    if (replace) {
      curr = replace;
      replace = nullptr;
    }
  }

  void startWalk(Function *func) override {
    walk(func->body);
  }

  void startWalk(Module *module) override {
    // Dispatch statically through the SubType.
    SubType* self = static_cast<SubType*>(this);
    for (auto curr : module->functionTypes) {
      self->visitFunctionType(curr);
      assert(!replace);
    }
    for (auto curr : module->imports) {
      self->visitImport(curr);
      assert(!replace);
    }
    for (auto curr : module->exports) {
      self->visitExport(curr);
      assert(!replace);
    }
    for (auto curr : module->functions) {
      startWalk(curr);
      self->visitFunction(curr);
      assert(!replace);
    }
    self->visitTable(&module->table);
    assert(!replace);
    self->visitMemory(&module->memory);
    assert(!replace);
    self->visitModule(module);
    assert(!replace);
  }
};

} // namespace wasm

#endif // wasm_wasm_h
