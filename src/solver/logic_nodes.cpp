#include "logic_nodes.hpp"

using boost::logic::indeterminate;
using namespace boost::numeric::interval_lib;

////////////////////////////////////////////////////////////////////////////////
LogicAnd::LogicAnd()
    : BinaryNode(OP_AND)
{
    // Nothing to do here
}

void LogicAnd::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_bool && right->result_bool;
}

void LogicAnd::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_bool = left->result_bool && right->result_bool;
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

LogicOr::LogicOr()
    : BinaryNode(OP_OR)
{
    // Nothing to do here
}

void LogicOr::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_bool || right->result_bool;
}

void LogicOr::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_bool = left->result_bool || right->result_bool;
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

LogicNeq::LogicNeq()
    : BinaryNode(OP_NEQ)
{
    // Nothing to do here
}

void LogicNeq::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_bool != right->result_bool;
}

void LogicNeq::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_bool = left->result_bool != right->result_bool;
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

LogicNot::LogicNot()
    : UnaryNode(OP_NOT)
{
    // Nothing to do here.
}

void LogicNot::eval(const float X, const float Y, const float Z)
{
    result_bool = !child->result_bool;
}

void LogicNot::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_bool = !child->result_bool;
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

TransitionLt::TransitionLt()
    : BinaryNode(OP_LT)
{
    // Nothing to do here
}

void TransitionLt::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_float < right->result_float;
}

void TransitionLt::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    try {
        result_bool = left->result_interval < right->result_interval;
    } catch (comparison_error& e) {
        result_bool = indeterminate;
    }
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

TransitionLeq::TransitionLeq()
    : BinaryNode(OP_LEQ)
{
    // Nothing to do here
}

void TransitionLeq::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_float <= right->result_float;
}

void TransitionLeq::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    try {
        result_bool = left->result_interval <= right->result_interval;
    } catch (comparison_error& e) {
        result_bool = indeterminate;
    }
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

TransitionGt::TransitionGt()
    : BinaryNode(OP_GT)
{
    // Nothing to do here
}

void TransitionGt::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_float > right->result_float;
}

void TransitionGt::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    try {
        result_bool = left->result_interval > right->result_interval;
    } catch (comparison_error& e) {
        result_bool = indeterminate;
    }
    marked = !indeterminate(result_bool);
}

////////////////////////////////////////////////////////////////////////////////

TransitionGeq::TransitionGeq()
    : BinaryNode(OP_GEQ)
{
    // Nothing to do here
}

void TransitionGeq::eval(const float X, const float Y, const float Z)
{
    result_bool = left->result_float >= right->result_float;
}

void TransitionGeq::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    try {
        result_bool = left->result_interval >= right->result_interval;
    } catch (comparison_error& e) {
        result_bool = indeterminate;
    }
    marked = !indeterminate(result_bool);
}