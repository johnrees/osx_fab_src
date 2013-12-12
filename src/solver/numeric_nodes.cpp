#include "numeric_nodes.hpp"
#include "switches.hpp"

using namespace std;

using boost::logic::indeterminate;
using namespace boost::numeric::interval_lib;

////////////////////////////////////////////////////////////////////////////////

NumericAbs::NumericAbs()
    : UnaryNode(OP_ABS)
{
    // Nothing to do here.
}

void NumericAbs::eval(const float X, const float Y, const float Z)
{
    result_float = abs(child->result_float);
}

void NumericAbs::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = abs(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericCos::NumericCos()
    : UnaryNode(OP_COS)
{
    // Nothing to do here.
}

void NumericCos::eval(const float X, const float Y, const float Z)
{
    result_float = cos(child->result_float);
}

void NumericCos::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = cos(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericSin::NumericSin()
    : UnaryNode(OP_SIN)
{
    // Nothing to do here.
}

void NumericSin::eval(const float X, const float Y, const float Z)
{
    result_float = sin(child->result_float);
}

void NumericSin::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = sin(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericACos::NumericACos()
    : UnaryNode(OP_ACOS)
{
    // Nothing to do here.
}

void NumericACos::eval(const float X, const float Y, const float Z)
{
    result_float = acos(child->result_float);
}

void NumericACos::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = acos(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericASin::NumericASin()
    : UnaryNode(OP_ASIN)
{
    // Nothing to do here.
}

void NumericASin::eval(const float X, const float Y, const float Z)
{
    result_float = asin(child->result_float);
}

void NumericASin::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = asin(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericATan::NumericATan()
    : UnaryNode(OP_ATAN)
{
    // Nothing to do here.
}

void NumericATan::eval(const float X, const float Y, const float Z)
{
    result_float = atan(child->result_float);
}

void NumericATan::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = atan(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericSqrt::NumericSqrt()
    : UnaryNode(OP_SQRT)
{
    // Nothing to do here.
}

void NumericSqrt::eval(const float X, const float Y, const float Z)
{
    result_float = sqrt(child->result_float);
}

void NumericSqrt::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = sqrt(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericNeg::NumericNeg()
    : UnaryNode(OP_NEGATIVE)
{
    // Nothing to do here.
}

void NumericNeg::eval(const float X, const float Y, const float Z)
{
    result_float = -child->result_float;
}

void NumericNeg::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = -child->result_interval;
}

////////////////////////////////////////////////////////////////////////////////

NumericExp::NumericExp()
    : UnaryNode(OP_EXP)
{
    // Nothing to do here.
}

void NumericExp::eval(const float X, const float Y, const float Z)
{
    result_float = exp(child->result_float);
}

void NumericExp::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = exp(child->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericSgn::NumericSgn()
    : UnaryNode(OP_SGN)
{
    // Nothing to do here.
}

void NumericSgn::eval(const float X, const float Y, const float Z)
{
    if (child->result_float > 0)
        result_float = 1;
    else if (child->result_float < 0)
        result_float = -1;
    else
        result_float = 0;
}

void NumericSgn::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = sgn(child->result_interval);
}
////////////////////////////////////////////////////////////////////////////////

NumericPlus::NumericPlus()
    : BinaryNode(OP_PLUS)
{
    // Nothing to do here.
}

void NumericPlus::eval(const float X, const float Y, const float Z)
{
    result_float = left->result_float + right->result_float;
}

void NumericPlus::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = left->result_interval + right->result_interval;
}

////////////////////////////////////////////////////////////////////////////////

NumericMinus::NumericMinus()
    : BinaryNode(OP_MINUS)
{
    // Nothing to do here.
}

void NumericMinus::eval(const float X, const float Y, const float Z)
{
    result_float = left->result_float - right->result_float;
}

void NumericMinus::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = left->result_interval - right->result_interval;
}

////////////////////////////////////////////////////////////////////////////////

NumericMult::NumericMult()
    : BinaryNode(OP_MULT)
{
    // Nothing to do here.
}

void NumericMult::eval(const float X, const float Y, const float Z)
{
    result_float = left->result_float * right->result_float;
}

void NumericMult::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = left->result_interval * right->result_interval;
}

////////////////////////////////////////////////////////////////////////////////

NumericDiv::NumericDiv()
    : BinaryNode(OP_DIV)
{
    // Nothing to do here.
}

void NumericDiv::eval(const float X, const float Y, const float Z)
{
    result_float = left->result_float / right->result_float;
}

void NumericDiv::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = left->result_interval / right->result_interval;
}

////////////////////////////////////////////////////////////////////////////////

NumericATan2::NumericATan2()
    : BinaryNode(OP_ATAN2)
{
    // Nothing to do here.
}

void NumericATan2::eval(const float X, const float Y, const float Z)
{
    result_float = atan2(left->result_float, right->result_float);
}

void NumericATan2::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = atan2(left->result_interval, right->result_interval);
}

////////////////////////////////////////////////////////////////////////////////

NumericPow::NumericPow()
    : BinaryNode(OP_POW)
{
    // Nothing to do here.
}

void NumericPow::eval(const float X, const float Y, const float Z)
{
    result_float = pow(left->result_float, right->result_float);
}

void NumericPow::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = pow(left->result_interval, right->result_interval.lower());
}

////////////////////////////////////////////////////////////////////////////////

NumericMin::NumericMin()
    : BinaryNode(OP_MIN), left_cached(false), right_cached(false)
{
    // Nothing to do here.
}

void NumericMin::eval(const float X, const float Y, const float Z)
{
    result_float = min(left->result_float, right->result_float);    
}

void NumericMin::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = min(left->result_interval, right->result_interval);

#if MINMAX_PRUNE
    if (right->result_interval < left->result_interval) {
        if (!left_cached) {
            left_cached = true;
            left->sub_ref();
        }
        
        // In case this node gets deactivated out, save a float result
        left->result_float = left->result_interval.lower();
        
    } else if (left_cached) {
        left_cached = false;
        left->add_ref();
    }
    
    if (left->result_interval < right->result_interval) {
        if (!right_cached) {
            right->sub_ref();
            right_cached = true;
        }
        
        // In case this node gets deactivated, save a float result
        right->result_float = right->result_interval.lower();
        
    } else if (right_cached) {
        right_cached = false;
        right->add_ref();
    }
#endif

}

void NumericMin::deactivate()
{
    Node::deactivate();
    if (!left_cached)
        left->sub_ref();
    if (!right_cached)
        right->sub_ref();
}

void NumericMin::activate()
{    
    if (!left_cached)
        left->add_ref();   
    if (!right_cached)
        right->add_ref();
    Node::activate();
}

////////////////////////////////////////////////////////////////////////////////

NumericMax::NumericMax()
    : BinaryNode(OP_MAX), left_cached(false), right_cached(false)
{
    // Nothing to do here.
}

void NumericMax::eval(const float X, const float Y, const float Z)
{
    result_float = max(left->result_float, right->result_float);
}

void NumericMax::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = max(left->result_interval, right->result_interval);
    
#if MINMAX_PRUNE
    if (right->result_interval > left->result_interval) {
        if (!left_cached) {
            left->sub_ref();
            left_cached = true;
        }
        
        // In case this node gets deactivated out, save a float result
        left->result_float = left->result_interval.lower();
        
    } else if (left_cached) {
        left->add_ref();
        left_cached = false;
    }
    
    if (left->result_interval > right->result_interval) {
        if (!right_cached) {
            right->sub_ref();
            right_cached = true;
        }
        
        // In case this node gets deactivated, save a float result
        right->result_float = right->result_interval.lower();
        
    } else if (right_cached) {
        right->add_ref();
        right_cached = false;
    }
#endif

}


void NumericMax::activate()
{
    if (!left_cached)
        left->add_ref();
    if (!right_cached)
        right->add_ref();
    Node::activate();
}

void NumericMax::deactivate()
{
    Node::deactivate();
    if (!left_cached)
        left->sub_ref();
    if (!right_cached)
        right->sub_ref();
}

////////////////////////////////////////////////////////////////////////////////
VarX::VarX()
    : NonaryNode(VAR_X)
{
    // Nothing to do here.
}

void VarX::eval(const float X, const float Y, const float Z)
{
    result_float = X;
}

void VarX::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = X;
}

////////////////////////////////////////////////////////////////////////////////

VarY::VarY()
    : NonaryNode(VAR_Y)
{
    // Nothing to do here.
}

void VarY::eval(const float X, const float Y, const float Z)
{
    result_float = Y;
}

void VarY::eval(const FabInterval& X,
                const FabInterval& Y,
                const FabInterval& Z)
{
    result_interval = Y;
}

////////////////////////////////////////////////////////////////////////////////

VarZ::VarZ()
    : NonaryNode(VAR_Z)
{
    // Nothing to do here.
}

void VarZ::eval(const float X, const float Y, const float Z)
{
    result_float = Z;
}

void VarZ::eval(const FabInterval& X,
                      const FabInterval& Y,
                      const FabInterval& Z)
{
    result_interval = Z;
}

////////////////////////////////////////////////////////////////////////////////

NumericConst::NumericConst(float value)
    : NonaryNode(NUM_CONST)
{
    marked = true;

    result_float = value;
    result_interval = value;
}

bool NumericConst::operator==(const Node& rhs)
{
    if (operation != rhs.op())
        return false;
    return result_float == rhs.result_float;
}

void NumericConst::print(ostream& o) const
{
    o << result_float;
}

void NumericConst::dot(ostream& o) const
{
    o << "\"p" << this << "\" [shape = " << dot_shape(operation)
      << ", color = " << dot_color(operation) << ", fontsize = 24, label = \""
      << result_float << "\", style=\"dotted\"]\n";
}