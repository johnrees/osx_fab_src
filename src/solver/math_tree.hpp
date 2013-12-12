#ifndef MATH_TREE_H
#define MATH_TREE_H

#include <list>
#include <vector>
#include <boost/thread.hpp>

#include "fab_interval.hpp"
#include "region.hpp"

// Forward declaration of node.
class Node;



// This class stores a math expression tree.
class MathTree
{
public:
    /* MathTree()
     *
     *  Simple constructor initializes pointers to NULL and nothing else.
     */
    MathTree();
    
    
    /* ~MathTree()
     *
     *  Destructor deletes all stored nodes.
     */    
    ~MathTree();
    
    
    /* MathTree* clone()
     *
     *  Clones the MathTree.  Constants and cached values are not cloned,
     *  so anything that references them will point back to this tree.
     *
     *  Creates a ThreadComm object so that the child can notify us on
     *  destruction.
     */
    MathTree* clone();
    
    
    /* void wait_for_clones()
     *
     *  Waits for all clones to be deleted before returning.
     */
    void wait_for_clones();
    
    
    /* void add(Node* n)
     *
     *  Adds a node to the tree, based on the node's weight.
     *  The node is added to the list<Node*> level_list if it is unmarked,
     *  otherwise it is added to the list<Node*> constants.
     *
     *  Note that pack() must be called before the tree can be evaluated.
     */
    void add(Node* n);

    /* void pack()
     *
     *  Converts the STL lists into C-style arrays for fast access.
     */
    void pack();
    
    /* void set_root(Node* r)
     *
     *  Sets the root of the tree.
     */
    void set_root(Node* r);
    
    /* int node_count() const
     *
     *  Counts the number of active nodes in the tree.
     */
    int node_count() const;

    /* int constant_count() const
     *
     *  Counts the number of constant nodes in the tree.
     */
    int constant_count() const;
    
    
    /* int depth() const
     *
     *  Returns the tree's depth.
     */
    int depth() const { return num_levels; }

    /* void eval(X, Y, Z)
     *
     *  Evaluates every active node in the tree, from leaves to root.
     *  After an evaluate, each node will have correct results stored in
     *  its result_{float,interval,bool,color} variables.  Each node may
     *  also be marked if it can be ignored upon further recursion.
     */
    void eval(const float X,
              const float Y,
              const float Z);
    void eval(const FabInterval& X,
              const FabInterval& Y,
              const FabInterval& Z);
    
    /* void push()
     *
     *  Traveling downward through the tree, deactivating nodes that are no
     *  longer relevant (either because they will not change values upon
     *  deeper recursion, or because no one is referencing them anymore).
     *
     *  Deactivated nodes are moved to the back of their list, and the number
     *  of active nodes for that level is decreased by one.
     *
     *  This function keeps track of how many nodes have been deactivated on
     *  each tree level in the variable dNodes.
     */
    void push();
    
    /* void pop()
     *
     *  Traverse through the tree, re-activating nodes that had been
     *  deactivated by an earlier push().
     */
    void pop();
    
    /* void export_dot(std::string filename) const
     *
     *  Exports the tree to a dot/graphviz file with a given filename.
     */
    void export_dot(std::string filename) const;
    
    // Output helper function
    friend std::ostream& operator<<(std::ostream& o, const MathTree& t);
    
    // Root of the tree (stores the overall tree's results)
    Node* root;
    
private:
    // Nice STL vectors and lists to store nodes while we're building the tree
    std::vector< std::list<Node*> > level_list;
    std::list<Node*> constants;

    // Fast C-style array for when we're actually using the tree.
    Node*** levels;
    
    // Keeps track of how many nodes are active on each level
    int* active_nodes;
    
    // Total number of levels in the tree
    int num_levels;
    
    // Array of stacks storing the number of nodes that have been deactivated
    // in each push() operation.  Each level has an independent stack
    // (implemented as a list<int>)
    std::list<int>* dNodes;
    
    
    typedef struct ThreadComm {
        ThreadComm() : done(false) {}
        bool done;
        boost::mutex mutex;
        boost::condition_variable condition;
    } ThreadData;
    
    std::list<ThreadComm*> clones;
    ThreadComm* parent;
    
};

// Helpful typedefs
typedef std::list<Node*>::iterator        MathTreeIter;
typedef std::list<Node*>::const_iterator  MathTreeConstIter;
    
std::ostream& operator<<(std::ostream& o, const MathTree& t);
#endif