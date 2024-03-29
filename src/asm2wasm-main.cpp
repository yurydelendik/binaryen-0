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
// asm2wasm console tool
//

#include "support/colors.h"
#include "support/command-line.h"
#include "support/file.h"
#include "wasm-printing.h"

#include "asm2wasm.h"

using namespace cashew;
using namespace wasm;

int main(int argc, const char *argv[]) {
  Options options("asm2wasm", "Translate asm.js files to .wast files");
  options
      .add("--output", "-o", "Output file (stdout if not specified)",
           Options::Arguments::One,
           [](Options *o, const std::string &argument) {
             o->extra["output"] = argument;
             Colors::disable();
           })
      .add("--mapped-globals", "-m", "Mapped globals", Options::Arguments::One,
           [](Options *o, const std::string &argument) {
             o->extra["mapped globals"] = argument;
           })
      .add_positional("INFILE", Options::Arguments::One,
                      [](Options *o, const std::string &argument) {
                        o->extra["infile"] = argument;
                      });
  options.parse(argc, argv);

  const auto &mg_it = options.extra.find("mapped globals");
  const char *mappedGlobals =
      mg_it == options.extra.end() ? nullptr : mg_it->second.c_str();

  Asm2WasmPreProcessor pre;
  auto input(
      read_file<std::vector<char>>(options.extra["infile"], options.debug));
  char *start = pre.process(input.data());

  if (options.debug) std::cerr << "parsing..." << std::endl;
  cashew::Parser<Ref, DotZeroValueBuilder> builder;
  Ref asmjs = builder.parseToplevel(start);
  if (options.debug) {
    std::cerr << "parsed:" << std::endl;
    asmjs->stringify(std::cerr, true);
    std::cerr << std::endl;
  }

  if (options.debug) std::cerr << "wasming..." << std::endl;
  AllocatingModule wasm;
  wasm.memory.initial = wasm.memory.max =
      16 * 1024 * 1024;  // we would normally receive this from the compiler
  Asm2WasmBuilder asm2wasm(wasm, pre.memoryGrowth, options.debug);
  asm2wasm.processAsm(asmjs);

  if (options.debug) std::cerr << "optimizing..." << std::endl;
  asm2wasm.optimize();

  if (options.debug) std::cerr << "printing..." << std::endl;
  Output output(options.extra["output"], options.debug);
  printWasm(&wasm, output.getStream());

  if (mappedGlobals) {
    if (options.debug)
      std::cerr << "serializing mapped globals..." << std::endl;
    asm2wasm.serializeMappedGlobals(mappedGlobals);
  }

  if (options.debug) std::cerr << "done." << std::endl;
}
