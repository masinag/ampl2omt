# AMPL to OMT Converter

Convert NonLinear Programming problems in AMPL format (.nl) to OMT problems in SMT-LIBv2 with optimization extensions.

Reference for AMPL .nl format: https://ampl.github.io/nlwrite.pdf

Reference for SMT-LIBv2 with optimization extensions: https://optimathsat.disi.unitn.it/pages/smt2reference.html

## Installation

The project can be built using CMake. To build the project, run the following commands:

```bash
mkdir build
cd build
cmake ..
make
```

This will generate the executable `build/ampl2omt`.

## Usage

First, write your .nl file.

You can convert AMPL model (.mod) files to .nl format using the AMPL software.
See [the AMPL documentation](https://ampl.github.io/nlwrite.pdf).

Then, run the converter with the following command:

```bash
./ampl2omt <input_file> <output_file>
```

where `<input_file>` is the path to the .nl file and `<output_file>` is the path to the output .smt2 file that will be
generated.

## Implementation

The converter is implemented in C++ and uses the [MP library](https://github.com/ampl/mp) to parse .nl files and navigate the expression graph.