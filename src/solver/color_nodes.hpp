#ifndef COLOR_NODES_H
#define COLOR_NODES_H

#include "node.hpp"
#include "node_macro.hpp"

// This header file defines nodes that take in colors
// as inputs.
NODE(ColorAnd, BinaryNode);
NODE(ColorOr,  BinaryNode);
NODE(ColorNot, UnaryNode);

#endif