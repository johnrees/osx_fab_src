#ifndef REGION_H
#define REGION_H

#include <list>
#include <stdint.h>

// Forward declarations
struct FabVars;

// A note on terminology:
//      x, y, z refer to coordinates in space.
//      i, j, k refer to pixel discretized coordinates

// Small structure to hold a 3D lattice region.
typedef struct Region {

    /* Region()
     *
     *  Constructs a empty region.
     */
    Region();

    /* Region(const FabVars& v)
     *
     *  Constructs a region that fills the entire space defined in v.
     */
    Region(const FabVars& v);
    
    
    /* Region(unsigned int imin, unsigned int jmin, unsigned int kmin,
              unsigned int imax, unsigned int jmax, unsigned int kmax)
     
     *  Constructs a region from the provided bounds.
     */
    Region(unsigned int imin, unsigned int jmin, unsigned int kmin,
           unsigned int imax, unsigned int jmax, unsigned int kmax);
    
    
    /* std::list<Region> split() const
     *
     *  Splits a region in half all three axes, returning a list of
     *  the resulting subregions.
     */
    std::list<Region> split() const;
    
    /* std::list<Region> split(int count) const
     *
     *  Splits a region into a provided number of subregions, returning
     *  a list of the resulting subregions.  The split is biased towards
     *  the larger axes of the initial region.
     */
    std::list<Region> split(int count) const;
    std::list<Region> split_xy(int count) const;
    
    /* std::list<Region> split(int isplit, int jsplit, int ksplit) const
     *
     *  Splits a region with a specific number of subregions per axis.
     */
    std::list<Region> split(int isplit, int jsplit, int ksplit) const;
    
    // Boundaries of the region
    int imin, jmin, kmin, imax, jmax, kmax;
    
    // Dimensions of the region
    int ni, nj, nk;
    
    // Volume of the region
    uint64_t volume;
    
} Region;

/* std::ostream& operator<<(std::ostream& o, const Region& r)
 *
 *  Prints a region.
 */
std::ostream& operator<<(std::ostream& o, const Region& r);

#endif