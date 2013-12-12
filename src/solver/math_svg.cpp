#include <iostream>

#include "fabvars.hpp"
#include "edgesolver.hpp"
#include "math_tree.hpp"
#include "parser.hpp"
#include "node.hpp"
#include "thread_manager.hpp"

#include "switches.hpp"

using namespace std;

void print_help()
{
    cout << "command line: math_svg in.math out.svg [resolution [slices [error [quality]]]]\n"
         << "   in.math = input math string file\n"
         << "   out.png = output PNG image\n"
         << "   resolution = voxels per mm (default: 10)\n"
         << "   slices = z slices (defaults: 1 for 2D models, 10 for 3D models)\n"
         << "   error = maximum decimation error (in mm^2)\n"
         << "   quality = voxel interpolation level (default: 8)\n"
         << "Note: output svgs are at 72 dpi." << endl;
}


int main(int argc, char** argv)
{
    if (argc < 3) {
        print_help();
        exit(1);
    }

    argv++; argc--; // Remove executable name from argv
    FabVars v(OUTPUT_SVG, argc, argv);

    if (v.mode != SOLVE_BOOL && v.mode != SOLVE_REAL) {
        cerr << "Error:  math_svg only works on Boolean or Real math strings." << endl;
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
    ThreadManager<EdgeSolver> tm(v);
    tm.evaluate(tree);
#else
    EdgeSolver s(tree, v);
    s.evaluate_region(Region(v));
    s.save();
#endif

    cout << "\nWriting SVG." << endl;
    v.write_svg();
    
    delete tree;
    return 0;
}