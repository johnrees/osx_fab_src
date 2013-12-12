#ifndef NODE_H
#define NODE_H

#include <iostream>

#include <boost/thread.hpp>

#include "opcodes.hpp"
#include "fab_interval.hpp"
#include "region.hpp"

class Node
{
public:
    // Constructor and destructor.  Nothing unusual here.
    Node(opcode operation);
    virtual ~Node();

    /* virtual Node* clone() = 0
     *
     *  Clones a node (non-recursively).
     *  Refers to the clone_address member of children, which
     *  must be populated with a valid value.
     */
    virtual Node* clone() = 0;
    
    
    /* static Node* make(opcode op)
     *
     *  Creates a new node with the provided opcode.
     *
     *  Used by clone() to duplicate an existing node.
     */
    static Node* make(opcode op);
    
    
    /* virtual void eval(X, Y, Z) = 0
     *
     *  Evaluates the node, storing results in the local
     *  result_{float,interval,bool,color} variables (depending on
     *  node output type and whether we're solving for intervals or
     *  floats).  Nodes may also become marked if they can be ignored
     *  upon deeper recursion.
     */
    virtual void eval(const float X,
                      const float Y,
                      const float Z) = 0;
    virtual void eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z) = 0;
    
    
    /* int add_ref()
     * int sub_ref()
     *
     *  Increments and decrements to the node's reference count, returning
     *  the updated value.
     */
    int add_ref() { return ++ref_count; }
    int sub_ref() { return --ref_count; }
    
    
    /* virtual void deactivate()
     * virtual void activate()
     *
     *  Turns a node on or off.  Turning a node off entails:
     *      Setting 'marked = true'
     *      Subtracting a reference from all children.
     *
     *  Turning a node on is the opposite.
     */
    virtual void deactivate();
    virtual void activate();
    
    
    /* int get_weight()
     *
     *  Return the node's weight (i.e. distance from the bottom of the
     *  tree).  Constants and variables have weight 0, everything else
     *  has weight of max(children's weights) + 1.
     */
    int get_weight() const { return weight;    }
    
    
    /* opcode op() const
     *
     *  Return this node's opcode.
     */
    opcode op() const      { return operation; }
    
    
    /* bool ignored() const
     *
     *  Returns true if no other nodes are watching this node
     *  (i.e. ref_count == 0).
     */
    bool ignored() const { return ref_count == 0; }


    /* bool cacheable() const
     *
     *  Returns true if no other nodes are watching this node
     *  (i.e. ref_count == 0) or if this node is marked.
     */
    bool cacheable() const { return marked || ref_count == 0; }
    
    /* Node* get_clone_address()
     *
     *  Returns the address of this node's youngest clone.
     */
    Node* get_clone_address() const { return clone_address; }
    
    
    /* virtual bool operator==(const Node& rhs) = 0
     *
     *  Recursively compare two nodes, going down the
     *  tree as far as needed to check equality.
     */
    virtual bool operator==(const Node& rhs) = 0;
    
    
    /* virtual void print(std::ostream& o) const
     *
     *  Function defined by derived classes to print a node
     *  and its children in human-readable (hah!) form.
     */
    virtual void print(std::ostream& o) const = 0;
    
    
    /* virtual void dot(std::ostream& o) const
     *
     *  Function defined by derived classes to print a node
     *  and its children in graphviz format.
     */
    virtual void dot(std::ostream& o) const;
    
    
    /* virtual bool null_children const
     *
     *  Returns true if the node's children are null, false otherwise.
     */
    virtual bool null_children() const=0;
    
    // Results are stored locally and then looked up by parents
    float result_float;
    FabInterval result_interval;
    boost::tribool result_bool;
    int result_color;
    
    // Nodes are marked if they should to be moved to the cache (with
    // a math_tree push operation).
    bool marked;
    
protected:
    const opcode operation;
    int ref_count;
    
    int weight;
    Node* clone_address;
};

std::ostream& operator<<(std::ostream& o, const Node& t);

////////////////////////////////////////////////////////////////////////////////
class UnaryNode : public Node
{
public:
    UnaryNode(opcode operation);
    virtual ~UnaryNode() {/*Nothing to do here*/};
    Node* clone();
        
    void deactivate();
    void activate();
    
    bool operator==(const Node& rhs);
    void print(std::ostream& o) const;
    void   dot(std::ostream& o) const;
    
    void set_child(Node* child);
    virtual bool null_children() const;
protected:
    Node* child;
};
////////////////////////////////////////////////////////////////////////////////
class BinaryNode : public Node
{
public:
    BinaryNode(opcode operation);
    virtual ~BinaryNode() {/*Nothing to do here*/};
    Node* clone();
    
    void deactivate();
    void activate();
    
    bool operator==(const Node& rhs);
    void print(std::ostream& o) const;
    void   dot(std::ostream& o) const;
    
    void set_children(Node* left, Node* right);
    virtual bool null_children() const;
protected:
    Node* left;
    Node* right;
};
////////////////////////////////////////////////////////////////////////////////
class NonaryNode : public Node
{
public:
    NonaryNode(opcode operation);
    virtual ~NonaryNode() {/*Nothing to do here*/};
    Node* clone();
    
    void deactivate();
    void activate();
    
    bool operator==(const Node& rhs);
    void print(std::ostream& o) const;
    virtual bool null_children() const;
};

#endif
