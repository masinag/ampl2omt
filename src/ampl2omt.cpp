#include "smtlib_converter.hpp"
#include "mp/nl-reader.h"
#include <string>
#include <filesystem>
#include <fstream>


int PrintUsage(const std::string &executable_name) {
    fmt::print("Usage:\n");
    fmt::print("{} <input> <output>\n", executable_name);
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        return PrintUsage(argv[0]);
    }

    std::string input_filename = argv[1];
    if (!std::filesystem::exists(input_filename)) {
        fmt::print("File {} does not exist\n", input_filename);
        return EXIT_FAILURE;
    }


    mp::Problem p;
    ReadNLFile(input_filename, p);

    fmt::MemoryWriter w;
    SmtLibConverter converter(w, p);

    for (const auto &v: p.vars()) {
        variable_decl_to_smtlib(v, w, p);
        w << "\n";
    }
    w << "\n";

    for (const auto &c: p.algebraic_cons()) {
        constraint_to_smtlib(c, p, converter);
        w << "\n";
    }
    w << "\n";

    for (const auto &obj: p.objs()) {
        objective_to_smtlib(obj, p, converter);
        w << "\n";
    }

    w << "(check-sat)\n(get-objectives)";

    std::string output_filename = argv[2];
    std::ofstream output_file(output_filename);
    output_file << w.str() << std::endl;
    output_file.close();
}