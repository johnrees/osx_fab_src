#include <iostream>

#include "fabvars.hpp"
#include "math_tree.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "thread_manager.hpp"
#include "trisolver.hpp"

#include "switches.hpp"

using namespace std;


void print_help()
{
    cout << "command line: math_stl in.math out.stl [resolution [quality]]\n"
         << "   in.math = input math string file\n"
         << "   out.png = output PNG image\n"
         << "   resolution = voxels per mm (optional, default 10)\n"
         << "   quality = voxel interpolation level (default 8)\n";
}


int main(int argc, char** argv)
{
    if (argc < 3) {
        print_help();
        exit(1);
    }

    argv++; argc--; // Remove executable name from argv
    FabVars v(OUTPUT_STL, argc, argv);

    if (v.mode != SOLVE_BOOL && v.mode != SOLVE_REAL) {
        cerr << "Error:  math_stl only works on Boolean or Real math strings." << endl;
        exit(4);
    }

    Parser p;
    MathTree* tree = p.parse(v.math_string, v.mode);
    if (!tree)
        return 1;

    cout << "Nodes in tree: " << tree->node_count() << endl
         << "Tree depth: " << tree->root->get_weight() << endl
         << "Evaluating (region size = " << v.ni << " x " << v.nj
                                         << " x " << v.nk << ")"
                                         << endl;
#if MULTITHREADED
    ThreadManager<TriSolver> tm(v);
    tm.evaluate(tree);
#else
    TriSolver s(tree, v);
    s.evaluate_region(Region(v));
    s.save();
#endif

    cout << "\nWriting STL..." << endl;
    v.write_stl();
    
    delete tree;
    return 0;
}