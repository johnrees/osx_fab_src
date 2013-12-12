#ifndef GEOMETRY
#define GEOMETRY

#include <iostream>
#include <list>
#include <map>

// Lightweight 3D vector with overloaded operations
class Vec3f
{
public:

    /* Vec3f()
       Vec3f(float x, float y)
       Vec3f(float x, float y, float z)

     *  Constructors.  Unspecified variables are set to 0.
     */
    Vec3f();
    Vec3f(float x, float y);
    Vec3f(float x, float y, float z);
    
    
    /* Vec3f operator+(const Vec3f& rhs) const
       Vec3f operator-(const Vec3f& rhs) const
       Vec3f operator*(const float rhs) const
       Vec3f operator/(const float rhs) const

     *  Overloaded arithmetic operators.
     */
    Vec3f operator+(const Vec3f& rhs) const;
    Vec3f operator-(const Vec3f& rhs) const;
    Vec3f operator*(const float rhs) const;
    Vec3f operator/(const float rhs) const;
    
    /* bool operator<(const Vec3f& rhs) const
       bool operator==(const Vec3f& rhs) const
       bool operator!=(const Vec3f& rhs) const

     *  Overloaded logical operators.
     */
    bool operator<(const Vec3f& rhs) const;
    bool operator==(const Vec3f& rhs) const;
    bool operator!=(const Vec3f& rhs) const;
    
    /* Vec3f norm() const
     *
     *  Calculates a normalized (unit) vector.
     */
    Vec3f norm() const;
    
    /* float len() const
     *
     *  Calculates the vector's length.
     */
    float len() const;
    
    /* Vec3f rotate90() const
     *
     *  Returns the vector rotated by 90 degrees about the z axis.
     */
    Vec3f rotate90() const;
    
    
    /* float dot(const Vec3f& rhs) const
     *
     *  Calculates the dot product of two vectors.
     */
    float dot(const Vec3f& rhs) const;
    
    // Output operator.
    friend std::ostream& operator<<(std::ostream& o, const Vec3f& v);

    float x, y, z;
};

// Output operator.
std::ostream& operator<<(std::ostream& o, const Vec3f& v);

////////////////////////////////////////////////////////////////////////////////

// Structure to hold a triangle (three Vec3fs)
typedef struct Triangle {
    Triangle(Vec3f v1, Vec3f v2, Vec3f v3);
    bool operator<(const Triangle& rhs) const;
        
    Vec3f v1, v2, v3;
} Triangle;

////////////////////////////////////////////////////////////////////////////////

// Structure to hold an edge (two Vec3fs)
typedef struct Edge {
    Edge(Vec3f v1, Vec3f v2);
    bool operator<(const Edge& rhs) const;
        
    Vec3f v1, v2;
} Edge;

////////////////////////////////////////////////////////////////////////////////

// A path is a list of vectors overloaded with a comparison operator.
class Path : public std::list<Vec3f>
{
public:
    Path();
    Path(Vec3f v1, Vec3f v2);
    
    bool operator<(const Path& rhs) const;
};

////////////////////////////////////////////////////////////////////////////////

// A set of paths.  From the outside, it looks like a list<Path>, but it also
// supports the += operator and performs decimation on appended paths.
class PathSet
{
public:
    /* PathSet()
     *
     *  Default constructor.
     */
    PathSet() : decimation_error(1) { /* Nothing to do here */ }
    
    /* PathSet(float de)
     *
     *  Constructor which sets decimation_error.
     */
    PathSet(float de) : decimation_error(de) { /* Nothing to do here */ }
    
    /* PathSet& operator+=(Path p)
     *
     *  Appends a path to the set, with automatic path decimation:
     *  If the path's start or end coincides with another path's end
     *  or start, then it may merge them depending on the resulting
     *  decimation error.
     */
    PathSet& operator+=(Path p);
    
    /* std::list<Path>::iterator begin()
       std::list<Path>::iterator end()
       std::list<Path>::const_iterator begin()
       std::list<Path>::const_iterator end()
     *
     *  Various iterators, which allows us to look like a list<Path>
     *  to outside users.
     */
    std::list<Path>::iterator begin() { return paths.begin(); }
    std::list<Path>::iterator end()   { return paths.end(); }
    std::list<Path>::const_iterator begin() const { return paths.begin(); }
    std::list<Path>::const_iterator end() const   { return paths.end(); }
    
    // List of active paths
    std::list<Path> paths;
    
    // Maximum allowed decimation error
    float decimation_error;
    
private:
    // Maps points to paths for which they are the start.
    std::map<Vec3f, std::list<Path>::iterator> beginnings;
    
    // Maps points to paths for which they are the end.
    std::map<Vec3f, std::list<Path>::iterator> endings;
};
    
#endif