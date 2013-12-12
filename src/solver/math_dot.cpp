#include <iostream>

#include "fabvars.hpp"
#include "parser.hpp"
#include "math_tree.hpp"

using namespace std;

void print_help()
{
    cout << "command line: math_dot in.math out.dot\n"
         << "   in.math = input math string file\n"
         << "   out.dot = output dot file\n";
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        print_help();
        exit(1);
    }

    argv++; argc--; // Remove executable name from argv
    FabVars v(OUTPUT_NONE, argc, argv);

    Parser p;
    MathTree* tree = p.parse(v.math_string, v.mode);
    if (!tree)
        return 1;

    tree->export_dot(v.outfile_name);

    delete tree;
    return 0;
}