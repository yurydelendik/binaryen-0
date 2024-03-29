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
// wasm2asm console tool
//

#include "support/colors.h"
#include "support/command-line.h"
#include "support/file.h"
#include "wasm-binary.h"
#include "wasm-printing.h"

using namespace cashew;
using namespace wasm;

int main(int argc, const char *argv[]) {
  bool printAsWas = false;

  Options options("wasm-dis", "Un-assemble a .wasm (WebAssembly binary format) into a .wast (WebAssembly text format)");
  options.add("--output", "-o", "Output file (stdout if not specified)",
              Options::Arguments::One,
              [](Options *o, const std::string &argument) {
                o->extra["output"] = argument;
                Colors::disable();
              })
      .add("--was", "", "Print as was",
           Options::Arguments::Zero,
           [&printAsWas](Options *, const std::string &) {
             printAsWas = true;
           })
      .add_positional("INFILE", Options::Arguments::One,
                      [](Options *o, const std::string &argument) {
                        o->extra["infile"] = argument;
                      });
  options.parse(argc, argv);

  auto input(read_file<std::vector<char>>(options.extra["infile"], options.debug));

  if (options.debug) std::cerr << "parsing binary..." << std::endl;
  AllocatingModule wasm;
  WasmBinaryBuilder parser(wasm, input, options.debug);
  parser.read();

  if (options.debug) std::cerr << "Printing..." << std::endl;
  Output output(options.extra["output"], options.debug);
  if (printAsWas) {
    printWas(&wasm, output.getStream());
  } else {
    printWasm(&wasm, output.getStream());
  }
  output << '\n';

  if (options.debug) std::cerr << "Done." << std::endl;
}
