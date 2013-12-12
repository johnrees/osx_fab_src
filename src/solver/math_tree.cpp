#include <fstream>

#include "math_tree.hpp"
#include "node.hpp"
#include "switches.hpp"

// using namespace std;

typedef boost::lock_guard<boost::mutex> lock_guard;

MathTree::MathTree()
    : root(NULL), levels(NULL), active_nodes(NULL), dNodes(NULL), parent(NULL)
{
    // Nothing to do here
}

MathTree::~MathTree()
{
    // If this tree is a clone, then notify the parent (because the parent will be
    // waiting for this tree to be deleted before it can delete itself).
    if (parent) {
        lock_guard lock(parent->mutex);
        parent->done = true;
        parent->condition.notify_one();
    }

    // Make sure that all children have been deleted.
    wait_for_clones();

    // Delete all nodes stored in the tree, from top to bottom (to prevent
    // issues due to reference subtraction)
    if (levels)
        for (int i = num_levels - 1; i >= 0; --i) {
            for (int j = 0; j < active_nodes[i]; ++j)
                delete levels[i][j];
            delete [] levels[i];
        }

    for (MathTreeIter it = constants.begin(); it != constants.end(); ++it)
        delete *it;

    delete [] levels;
    delete [] active_nodes;
    delete [] dNodes;
}

MathTree* MathTree::clone()
{
    MathTree* m = new MathTree();
//    cout << "MathTree::clone() is cloning " << this << " to " << m << endl;
    m->num_levels = num_levels;
    m->levels = new Node**[num_levels];
    m->active_nodes = new int[num_levels];
    m->dNodes = new std::list<int>[num_levels];

    // Clone all of the active nodes.
    // Since constants aren't changing, we don't need to clone them.
    for (int i = 0; i < num_levels; ++i) {
        m->active_nodes[i] = active_nodes[i];
        m->levels[i] = new Node*[active_nodes[i]];
        for (int j = 0; j < active_nodes[i]; ++j)
            m->levels[i][j] = levels[i][j]->clone();
    }

    // If the entire tree is constant, then point back to this
    // original tree's root.
    if (num_levels)
        m->root = m->levels[num_levels - 1][0];
    else
        m->root = root;

    clones.push_back(new ThreadComm());
    m->parent = clones.back();

    return m;
}

void MathTree::wait_for_clones()
{

    // Go through the list, waiting for each of the children.
    std::list<ThreadComm*>::iterator it = clones.begin();
    while (it != clones.end())
    {
        {
            boost::unique_lock<boost::mutex> lock((**it).mutex);
            while (!(**it).done)
                (**it).condition.wait(lock);
        }
        delete *it;
        it = clones.erase(it);
    }
}

void MathTree::add(Node* n)
{
    if (!n)
        return;
    else if (n->marked) {
        constants.push_back(n);
    } else {
        unsigned int L = n->get_weight();
        if (L + 1 > level_list.size())
            level_list.resize(L + 1);
        level_list[L].push_back(n);
    }
}

// Converts the friendly, easy-to use C++ STL data structures into
// fast C-style arrays of pointers.
void MathTree::pack()
{
    num_levels = level_list.size();
    levels = new Node**[num_levels];
    active_nodes = new int[num_levels];
    dNodes = new std::list<int>[num_levels];

    for (int i = 0; i < num_levels; ++i)
    {
        active_nodes[i] = level_list[i].size();
        levels[i] = new Node*[active_nodes[i]];
        int j = 0;
        while (level_list[i].size()) {
            levels[i][j++] = level_list[i].front();
            level_list[i].pop_front();
        }
    }
}

void MathTree::set_root(Node* r)
{
    root = r;
}

int MathTree::node_count() const
{
    int total = 0;
    for(int i = 0; i < num_levels; ++i)
        total += active_nodes[i];
    return total;
}

int MathTree::constant_count() const
{
    return constants.size();
}


// Evaluate a single point
void MathTree::eval(const float X,
                    const float Y,
                    const float Z)
{
    for (int i = 0; i < num_levels; ++i)
        for (int j = 0; j < active_nodes[i]; ++j)
            levels[i][j]->eval(X, Y, Z);
}

// Evaluate an interval region
void MathTree::eval(const FabInterval& X,
                    const FabInterval& Y,
                    const FabInterval& Z)
{
    for (int i = 0; i < num_levels; ++i)
        for (int j = 0; j < active_nodes[i]; ++j)
            levels[i][j]->eval(X, Y, Z);
}

void MathTree::push()
{
    // Pass downwards through the tree.  If a node is cached
    // (meaning it won't change upon further recursion), then
    // tell its children that they are no longer being watched.

    // If all of a node's parents are no longer watching it, then
    // remove that node from the tree as well (and inform its children)

    // Keep track of how many nodes were removed from the tree
    // in the dNodes[i] stack.

    for (int i = num_levels - 2; i >= 0; --i) {
        dNodes[i].push_back(0);
        for (int j = 0; j < active_nodes[i]; ++j) {
            if (levels[i][j]->cacheable())
            {
                levels[i][j]->deactivate();
                std::swap(levels[i][j--], levels[i][--active_nodes[i]]);
                dNodes[i].back()++;
            }
        }
    }
}

void MathTree::pop()
{
    // Increase the number of active nodes in the arrays so that
    // previous cached nodes are now evaluated.

    for (int i = 0; i < num_levels - 1; ++i) {

        // Activate each of the previously disabled nodes.
        for (int n = 0; n < dNodes[i].back(); ++n) {
            levels[i][active_nodes[i]++]->activate();
        }
        dNodes[i].pop_back();
    }
}

std::ostream& operator<<(std::ostream& o, const MathTree& t)
{
    o << "Constants (" << t.constants.size() << " items)\n";
    MathTreeConstIter it;
    for (it = t.constants.begin(); it != t.constants.end(); ++it)
        o << '\t' << **it << '\n';
    o << std::endl;

    for (int i = 0; i < t.num_levels; ++i)
    {
        o << "Level " << i << " (" << t.active_nodes[i] << " items)\n";
        for (int j = 0; j < t.active_nodes[i]; ++j)
            o << '\t' << *t.levels[i][j] << '\n';
        o << std::endl;
    }
    return o;
}

void MathTree::export_dot(std::string filename) const
{
    std::ofstream out;
    out.open(filename.c_str());
    out << "digraph math {\n"
        << "node [rank = min, fontsize = 14, fontname = Arial]\n";
    MathTreeConstIter it;
    for (it = constants.begin(); it != constants.end(); ++it)
       (**it).dot(out);

    for (int i = 0; i < num_levels; ++i)
        for (int j = 0; j < active_nodes[i]; ++j)
            levels[i][j]->dot(out);

    out << "}\n";
    out.close();
}