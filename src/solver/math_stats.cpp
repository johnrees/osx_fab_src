#include <iostream>
#include <cstring>
#include <cmath>

#include "fabvars.hpp"
#include "volsolver.hpp"
#include "math_tree.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "thread_manager.hpp"

#include "switches.hpp"

using namespace std;


void print_help()
{
    cout << "command line: math_stats in.math [resolution]\n"
         << "   in.math = input math string file\n"
         << "   resolution = pixels per mm (optional, default 10)\n";
}


int main(int argc, char** argv)
{
    if (argc < 1) {
        print_help();
        exit(1);
    }
 
    argv++; argc--; // Remove executable name from argv
    FabVars v(OUTPUT_STATS, argc, argv);

    Parser p;
    MathTree* tree = p.parse(v.math_string, v.mode);
    if (!tree)
        return 1;

    cout << "Nodes in tree: " << tree->node_count() << endl
         << "\t" << tree->constant_count() << " constants\n"
         << "Tree depth: " << tree->root->get_weight() << endl
         << "Evaluating (region size = " << v.ni << " x " << v.nj
                                         << " x " << v.nk << ")"
                                         << endl;

#if MULTITHREADED
    ThreadManager<VolSolver> tm(v);
    tm.evaluate(tree);
#else
    VolSolver s(tree, v);
    s.evaluate_region(Region(v));
#endif

    cout << "\n";
    cout << "Filled volume = " << v.volume/pow(v.pixels_per_mm,3) << " mm^3" << endl;

#if MULTITHREADED
    delete tree;
#endif

    return 0;
}