#ifndef LOGIC_NODES_H
#define LOGIC_NODES_H

#include "node.hpp"
#include "node_macro.hpp"

NODE(LogicAnd, BinaryNode);
NODE(LogicOr, BinaryNode);
NODE(LogicNeq, BinaryNode);
NODE(LogicNot, UnaryNode);

NODE(TransitionLt, BinaryNode);
NODE(TransitionLeq, BinaryNode);
NODE(TransitionGt, BinaryNode);
NODE(TransitionGeq, BinaryNode);
#endif