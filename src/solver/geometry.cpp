#include <cmath>

#include "geometry.hpp"

using namespace std;

Vec3f::Vec3f()
    : x(0), y(0), z(0)
{ /* Nothing to do here */ }

Vec3f::Vec3f(float x, float y)
    : x(x), y(y), z(0)
{
    // Nothing to do here.
}

Vec3f::Vec3f(float x, float y, float z)
    : x(x), y(y), z(z)
{
    // Nothing to do here.
}

Vec3f Vec3f::operator+(const Vec3f& rhs) const
{
    return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3f Vec3f::operator-(const Vec3f& rhs) const
{
    return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3f Vec3f::operator*(const float rhs) const
{
    return Vec3f(x*rhs, y*rhs, z*rhs);
}

Vec3f Vec3f::operator/(const float rhs) const
{
    return Vec3f(x/rhs, y/rhs, z/rhs);
}

bool Vec3f::operator<(const Vec3f& rhs) const
{
    if (x != rhs.x)
        return x < rhs.x;
    if (y != rhs.y)
        return y < rhs.y;
    return z < rhs.z;
}

bool Vec3f::operator==(const Vec3f& rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vec3f::operator!=(const Vec3f& rhs) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z;
}

float Vec3f::len() const
{
    return sqrt(x*x + y*y + z*z);
}

Vec3f Vec3f::norm() const
{
    float len = sqrt(x*x + y*y + z*z);
    return Vec3f(x / len, y / len, z / len);
}

Vec3f Vec3f::rotate90() const
{
    return Vec3f(-y, x);
}

float Vec3f::dot(const Vec3f& rhs) const
{
    return x*rhs.x + y*rhs.y + z*rhs.z;
}

std::ostream& operator<<(std::ostream& o, const Vec3f& v)
{
    o << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return o;
}

////////////////////////////////////////////////////////////////////////////////

Triangle::Triangle(Vec3f v1, Vec3f v2, Vec3f v3)
    : v1(v1), v2(v2), v3(v3)
{ /* Nothing to do here */ }

bool Triangle::operator<(const Triangle& rhs) const
{
    if (v1 != rhs.v1)
        return v1 < rhs.v1;
    if (v2 != rhs.v2)
        return v2 < rhs.v2;
    return v3 < rhs.v3;
}

////////////////////////////////////////////////////////////////////////////////

Edge::Edge(Vec3f v1, Vec3f v2)
    : v1(v1), v2(v2)
{ /* Nothing to do here */ }

bool Edge::operator<(const Edge& rhs) const
{
    if (v1 != rhs.v1)
        return v1 < rhs.v1;
    return v2 < rhs.v2;
}

////////////////////////////////////////////////////////////////////////////////
Path::Path()
    : std::list<Vec3f>()
{ /* Nothing to do here */ }


Path::Path(Vec3f v1, Vec3f v2)
    : std::list<Vec3f>()
{
    this->push_back(v1);
    this->push_back(v2);
}

bool Path::operator<(const Path& rhs) const
{
    Path::const_iterator it1 = this->begin();
    Path::const_iterator it2 = rhs.begin();
    
    while (true) {
        if (it2 == rhs.end())
            return false;
        if (it1 == this->end())
            return true;

        if (*it1 != *it2)
            return *it1 < *it2;
        
        ++it1;
        ++it2;
    }
}

////////////////////////////////////////////////////////////////////////////////

PathSet& PathSet::operator+=(Path rhs)
{
    Vec3f start = rhs.front();
    Vec3f end = rhs.back();
    
    // Check to see if we can weld the end of an old path to the start of this
    // new path.
    std::map<Vec3f, std::list<Path>::iterator>::iterator it;
    it = endings.find(start);
    
    if (it != endings.end()) {
        // Find the existing path that we're going to join
        list<Path>::iterator target = it->second;
        
        // Remove the existing path from the look-up maps, since
        // it's about to be modified
        beginnings.erase(target->front());
        endings.erase(target->back());
        
        // Check to see if we should merge this new segment.
        //
        //  Ascii art rendition:
        //     B
        //   ----->
        //   ^   /
        // A |  / C
        //   | /
        //   |/
        //
        // A is the last segment of the existing path
        // B is the first segment of the path being appended
        // C is the path that we will get if we merge the two.
        //
        // Decimation depends on the area of the triangle ABC.
        Path::const_iterator it = target->end();
        Vec3f a = *(--it);
        a = a - *(--it);
        Vec3f c = *it;

        it = rhs.begin();
        Vec3f b = *(it++);
        b = b - *(it);
        c = c - *it;

        float A = a.len();
        float B = b.len();
        float C = c.len();
        float area = (A+B+C)*(B+C-A)*(A+C-B)*(A+B-C);
                
        // Merge segments if the area of the resulting triangle
        // is sufficiently small.
        if (area < decimation_error)
            target->pop_back();

        // We remove the front node (since it is the end of one path
        // and the start of the other), then splice the paths together.
        rhs.pop_front();
        rhs.splice(rhs.begin(), *target);
        
        paths.erase(target);
        
        start = rhs.front();
    }
    
    // Check to see if we can weld the end of this new path to the start
    // of an old path.
    it = beginnings.find(end);
    if (it != beginnings.end()) {
        list<Path>::iterator target = it->second;
        beginnings.erase(target->front());
        endings.erase(target->back());
        
        // Check to see if we should merge this new segment.
        // We calculate the area of the triangle that will be deleted
        // if we decimate, then compare it to the variable decimation_error.
        Path::const_iterator it = target->begin();
        Vec3f a = *(it);
        a = a - *(++it);
        Vec3f c = *it;

        it = rhs.end();
        Vec3f b = *(--it);
        b = b - *(--it);
        c = c - *it;

        float A = a.len();
        float B = b.len();
        float C = c.len();
        float area = (A+B+C)*(B+C-A)*(A+C-B)*(A+B-C);
                
        // Merge segments if the area of the resulting triangle
        // is sufficiently small.
        if (area < decimation_error)
            target->pop_front();

        rhs.pop_back();
        rhs.splice(rhs.end(), *target);
            
        beginnings.erase(end);
        paths.erase(target);
        
        end = rhs.back();
    }

    paths.push_front(rhs);
    
    beginnings[start] = paths.begin();
    endings[end] = paths.begin();
    return *this;
}