#include "node.hpp"
#include "switches.hpp"

#include "numeric_nodes.hpp"
#include "logic_nodes.hpp"
#include "translator_nodes.hpp"
#include "color_nodes.hpp"

using namespace std;
using boost::logic::indeterminate;

Node::Node(opcode operation)
    : result_float(0), result_interval(0),
      result_bool(indeterminate), result_color(0),
      marked(false), operation(operation), 
      ref_count(0), weight(0),
      clone_address(this)
{
    // Nothing to do here.
}

Node::~Node()
{
    // Nothing to do here
}

Node* Node::make(opcode operation)
{
    switch (operation)
    {
        case OP_AND:        return new LogicAnd();
        case OP_OR:         return new LogicOr();
        case OP_NEQ:        return new LogicNeq();
        case OP_NOT:        return new LogicNot();
        case OP_LT:         return new TransitionLt();
        case OP_LEQ:        return new TransitionLeq();
        case OP_GT:         return new TransitionGt();
        case OP_GEQ:        return new TransitionGeq();
        case OP_ABS:        return new NumericAbs();
        case OP_COS:        return new NumericCos();
        case OP_SIN:        return new NumericSin();
        case OP_ACOS:       return new NumericACos();
        case OP_ASIN:       return new NumericASin();
        case OP_ATAN:       return new NumericATan();
        case OP_SQRT:       return new NumericSqrt();
        case OP_NEGATIVE:   return new NumericNeg();
        case OP_EXP:        return new NumericExp();
        case OP_SGN:        return new NumericSgn();
        case OP_PLUS:       return new NumericPlus();
        case OP_MINUS:      return new NumericMinus();
        case OP_MULT:       return new NumericMult();
        case OP_DIV:        return new NumericDiv();
        case OP_ATAN2:      return new NumericATan2();
        case OP_POW:        return new NumericPow();
        case OP_MIN:        return new NumericMin();
        case OP_MAX:        return new NumericMax();
        case NUM_CONST:     return new NumericConst(0);
        case VAR_X:         return new VarX();
        case VAR_Y:         return new VarY();
        case VAR_Z:         return new VarZ();
        case COLOR_AND:     return new ColorAnd();
        case COLOR_OR:      return new ColorOr();
        case COLOR_NOT:     return new ColorNot();
        case NUM2BOOL:      return new NumToBool();
        case BOOL2NUM:      return new BoolToNum();
        case NUM2COLOR:     return new NumToColor();
        case BOOL2COLOR:    return new BoolToColor();
        default:
            cerr << "Unknown clone target " << operation << endl;
            exit(1);
    }
}

void Node::deactivate()
{
    // If we clone this node, then we want to be pointing back here
    // (not to a previous clone address).
    clone_address = this;
}

void Node::activate()
{
    // Nothing to do here
}

void Node::dot(ostream& o) const
{
    o << "\"p" << this << "\" [shape = " << dot_shape(operation)
      << ", color = " << dot_color(operation) << ", fontsize = 24, label = \""
      << dot_label(operation) << "\"";
    if (marked)
        o << ", style=\"dotted\"";
    o << "]\n";
}

ostream& operator<<(ostream& o, const Node& t)
{
    t.print(o);
    return o;
}
////////////////////////////////////////////////////////////////////////////////

UnaryNode::UnaryNode(opcode operation)
    : Node(operation), child(NULL)
{
    // Nothing to do here.
}

Node* UnaryNode::clone()
{
    clone_address = make(operation);
    static_cast<UnaryNode*>(clone_address)->
        set_child(child->get_clone_address());
    return clone_address;
}
    

void UnaryNode::deactivate()
{
    Node::deactivate();
    child->sub_ref();
}

void UnaryNode::activate()
{  
    child->add_ref();
    Node::activate();
}

bool UnaryNode::operator==(const Node& rhs)
{
    if (operation != rhs.op() )
        return false;
    return *child == *static_cast<const UnaryNode&>(rhs).child;
}

void UnaryNode::print(ostream& o) const
{
    o << operation;
    if (child) {
        o << '(';
        child->print(o);
        o << ')';
    }
}

void UnaryNode::dot(ostream& o) const
{    
    Node::dot(o);
    
    o << "p" << this << "->p" << child
      << " [color = " << dot_arrow(operation) << "]\n";
}
    
void UnaryNode::set_child(Node* child_)
{
    child = child_;
    child->add_ref();
    marked = child->marked;
    if (marked) {
        // Evaluate for both floats and intervals to ensure that
        // result_float and result_interval are both populated
        eval(0,0,0);
        eval(FabInterval(0),FabInterval(0),FabInterval(0));
    } else {
        weight = child->get_weight() + 1;
    }
}

bool UnaryNode::null_children() const
{
    return child == NULL;
}

////////////////////////////////////////////////////////////////////////////////

BinaryNode::BinaryNode(opcode operation)
    : Node(operation), left(NULL), right(NULL)
{
    // Nothing to do here.
}

Node* BinaryNode::clone()
{
    clone_address = make(operation);
    static_cast<BinaryNode*>(clone_address)->
        set_children(left->get_clone_address(),
                     right->get_clone_address());
    return clone_address;
}

void BinaryNode::deactivate()
{
    Node::deactivate();
    left->sub_ref();
    right->sub_ref();
}

void BinaryNode::activate()
{
    left->add_ref();
    right->add_ref();
    Node::activate();
}

bool BinaryNode::operator==(const Node& rhs)
{
    if (operation != rhs.op() )
        return false;
    return *left  == *static_cast<const BinaryNode&>(rhs).left &&
           *right == *static_cast<const BinaryNode&>(rhs).right;
}

void BinaryNode::print(ostream& o) const
{
    if (operation == OP_ATAN2 || operation == OP_POW ||
        operation == OP_MAX || operation == OP_MIN)
        o << operation;
    if (left){
        o << '(';
        left->print(o);
    }

    if (operation == OP_ATAN2 || operation == OP_POW ||
        operation == OP_MAX || operation == OP_MIN) {
        if (left && right)
            o << ", ";
    }
    else
        o << operation;
        
    if (right) {
        right->print(o);
        o << ')';
    }
}

void BinaryNode::dot(ostream& o) const
{    
    Node::dot(o);
    
    o << "p" << this << "->p" << left
      << " [color = " << dot_arrow(operation) << "]\n";
    o << "p" << this << "->p" << right
      << " [color = " << dot_arrow(operation) << "]\n";
}

void BinaryNode::set_children(Node* left_, Node* right_)
{
    left = left_;
    left->add_ref();
    right = right_;
    right->add_ref();
    marked = left->marked && right->marked;
    if (marked) {
        // Evaluate for both floats and intervals to ensure that
        // result_float and result_interval are both populated
        eval(0,0,0);
        eval(FabInterval(0),FabInterval(0),FabInterval(0));
    } else {
        weight = max(left->get_weight() + 1, right->get_weight() + 1);
    }

}

bool BinaryNode::null_children() const
{
    return left == NULL || right == NULL;
}

////////////////////////////////////////////////////////////////////////////////

NonaryNode::NonaryNode(opcode operation)
    : Node(operation)
{
    // Nothing to do here.
}

Node* NonaryNode::clone()
{
    clone_address = make(operation);
    return clone_address;
}

void NonaryNode::deactivate() {
    Node::deactivate();
}

void NonaryNode::activate() 
{
    Node::activate();
}

bool NonaryNode::operator==(const Node& rhs)
{
    return operation == rhs.op();
}

void NonaryNode::print(ostream& o) const
{
    o << operation;
}

bool NonaryNode::null_children() const
{
    return false;
}