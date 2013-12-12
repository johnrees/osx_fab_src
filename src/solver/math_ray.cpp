#include <iostream>
#include <cstring>

#include "fabvars.hpp"
#include "raycaster.hpp"
#include "math_tree.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "thread_manager.hpp"

#include "switches.hpp"

using namespace std;


void print_help()
{
    cout << "command line: math_ray in.math out.png [resolution]\n"
         << "   in.math = input math string file\n"
         << "   out.png = output PNG image\n"
         << "   resolution = pixels per mm (optional, default 10)\n";
}


int main(int argc, char** argv)
{
    if (argc < 3) {
        print_help();
        exit(1);
    }
 
    argv++; argc--; // Remove executable name from argv
    FabVars v(OUTPUT_PNG, argc, argv);
    v.projection = true;
    
    if (v.mode != SOLVE_REAL) {
        cerr << "Error: math_ray only works on Real math strings." << endl;
        exit(4);
    }

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
    ThreadManager<Raycaster> tm(v);
    tm.evaluate(tree);
#else
    Raycaster s(tree, v);
    s.evaluate_region(Region(v));
#endif

    cout << "\n";
    v.write_png();

#if MULTITHREADED
    delete tree;
#endif

    return 0;
}