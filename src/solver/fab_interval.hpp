#ifndef FAB_INTERVAL_H
#define FAB_INTERVAL_H

#include <iostream>

#include <boost/numeric/interval.hpp>
#include <boost/logic/tribool.hpp>

// We are using a specific type of Boost interval, which we define
// in this unreasonably long typedef.
typedef boost::numeric::interval<float,
    boost::numeric::interval_lib::policies<
        boost::numeric::interval_lib::save_state_nothing<
            boost::numeric::interval_lib::rounded_transc_exact<float>
        >,
        boost::numeric::interval_lib::checking_base<float>
    >
> FabInterval;

// Interval comparisons should be done with tribool logic.
#include <boost/numeric/interval/compare/tribool.hpp>
using namespace boost::numeric::interval_lib::compare::tribool;

/* interval atan2(interval y, interval x)
 *
 *  Computes atan2 of two intervals.
 */
template<class T, class Policies>
boost::numeric::interval<T, Policies> atan2(
    const boost::numeric::interval<T, Policies>& y,
    const boost::numeric::interval<T, Policies>& x)
{

    // Do the 9 atan2 cases
    // Interval could be entirely within a quadrant (4 cases)
    // could span an axis without containing the origin (4 cases)
    // or could contain the origin (1 case)

    // For each case, when we draw the rectangle representing the 
    // domain of atan2 on the plane, the range of possible angles is 
    // spanned by the angles from the origin to two of the corners of
    // the rectangle. (These corners are enumerated below, from drawing
    // the picture.)
    
    if (x.lower() > 0) {  // To the right of the y axis
        if (y.lower() > 0)        // 1st quadrant
            return boost::numeric::interval<T, Policies>(atan2(y.lower(), x.upper()), atan2(y.upper(), x.lower()));
        else if (y.upper() < 0)	// 4th quadrant
            return boost::numeric::interval<T, Policies>(atan2(y.lower(), x.lower()), atan2(y.upper(), x.upper()));            
        else    // straddling the positive x axis
            return boost::numeric::interval<T, Policies>(atan2(y.lower(), x.lower()), atan2(y.upper(), x.lower()));
    } else if (x.upper() < 0) {   // To the left of the y axis

        if (y.lower() > 0)        // 2nd quadrant
            return boost::numeric::interval<T, Policies>(atan2(y.upper(), x.upper()), atan2(y.lower(), x.lower()));
        else if (y.upper() < 0)   // 3rd quadrant
            return boost::numeric::interval<T, Policies>(atan2(y.upper(),x.lower()), atan2(y.lower(),x.upper()));
        else    // straddling the negative x axis
                // branch cut --- all we can say is -pi to pi
            return boost::numeric::interval<T, Policies>(-M_PI, M_PI);
    }
    else {  // Straddling the y axis
        if (y.lower() > 0)	// straddling the positive y axis
            return boost::numeric::interval<T, Policies>(atan2(y.lower(),x.upper()), atan2(y.lower(),x.lower()));
        else if (y.upper() < 0) 	// straddling the negative y axis
            return boost::numeric::interval<T, Policies>(atan2(y.upper(),x.lower()), atan2(y.upper(),x.upper()));
        else	// interval contains the origin
                // all we can say is -pi to pi
            return boost::numeric::interval<T, Policies>(-M_PI, M_PI);
    }
}

/* interval sgn(interval x)
 *
 *  Returns the sign of the input interval, which is either -1, 1, 0, or
 *  interval(-1, 1)
 */
template<class T, class Policies>
boost::numeric::interval<T, Policies> sgn(
    const boost::numeric::interval<T, Policies>& x)
{
    if (x.lower() > 0)
        return 1;
    else if (x.upper() < 0)
        return -1;
    else if (x.lower() == 0 and x.upper() == 0)
        return 0;
    else
        return boost::numeric::interval<T, Policies>(-1, 1);
}

/* ostream& operator<<(ostream& o, interval& i)
 *
 *  Prints an interval
 */
template <class T, class P>
std::ostream& operator<<(std::ostream& o, const boost::numeric::interval<T,P>& i)
{
    return o << '[' << i.lower() << ' ' << i.upper() << ']';
}
#endif