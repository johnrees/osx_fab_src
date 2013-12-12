#ifndef FABTOOLS_H
#define FABTOOLS_H

#include <string>
#include <fstream>
#include <list>

#include <cstdlib>
#include <stdint.h>

#include <boost/thread.hpp>

#include "fab_interval.hpp"
#include "geometry.hpp"
#include "region.hpp"
#include "progress_bar.hpp"

enum solver_mode { SOLVE_BOOL, SOLVE_RGB, SOLVE_REAL };
enum output_mode { OUTPUT_PNG, OUTPUT_STL,
                   OUTPUT_SVG, OUTPUT_STATS, OUTPUT_NONE };

typedef struct FabVars {
    /* FabVars(output_mode o)
     *
     *  Constructor, taking in an output mode.
     */
    FabVars(output_mode o, int argc, char** argv);
    
    /* ~FabVars()
     *
     *  Destructor.  Deletes pixel arrays if they have been allocated.
     */
    ~FabVars();
    
    /* void load()
     *
     *  Imports the provided .math file and stores relevant values.
     */
    void load();


    /* void fill(Region r)
       void fill(Region r, unsigned char R, unsigned char G, unsigned char B)
     *
     *  Fills a region of the image with either solid white (with the first
     *  signature) or with a provided color.
     */
    void fill(Region r);
    void fill(Region r, unsigned char R, unsigned char G, unsigned char B);
    
    
    /* void add_triangles(std::list<Triangle> tris)
     *
     *  Adds a list of triangles to the stored triangle list by splicing
     *  the lists together.
     */
    void add_triangles(std::list<Triangle> tris);

    /* void add_paths(const PathSet& p)
     *
     *  Adds a set of paths to the stored set of paths, automatically
     *  decimating contiguous paths.
     */
    void add_paths(const PathSet& p);

    /* void add_volume(const int v)
     *
     *  Increments the saved volume count.
     */
    void add_volume(const uint64_t v);

    /* void write_png()
     *
     *  Writes out a png (either greyscale or RGB) based on data in either
     *  intensity or red, green, blue arrays.
     */
    void write_png();
    
    
    /* void write_stl()
     *
     *  Writes out an stl file from data stored in triangles.
     */
    void write_stl();


    /* void write_stl()
     *
     *  Writes out an svg file from data in paths.
     */
    void write_svg();
    
    

    /* float x(float i) const
       float y(float j) const
       float z(float k) const
     *
     *  Converts from lattice coordinates to real coordinates.
     */
    float x(float i) const;
    float y(float j) const;
    float z(float k) const;


    /* float x(float i) const
       float y(float j) const
       float z(float k) const
     *
     *  Converts from lattice intervals to real intervals.
     */
    FabInterval x(float imin, float imax) const;
    FabInterval y(float jmin, float jmax) const;
    FabInterval z(float kmin, float kmax) const;
    
    
    /* int k(float z) const
     *
     *  Converts from z coordinate to pixel grid value k
     */
    int k(float z) const;
    
    /* float scale(unsigned int k) const
     *
     *  Returns pixel brightness at a given lattice height.
     *  For greyscale images, this is between 0 and 65535; for RGB
     *  it is between 0 and 1.
     */
    float scale(unsigned int k) const;
    
    // Size of the lattice grid, in pixels/voxels
    int ni,nj,nk;
    
    // Minimum volume below which octree recursion won't occur.
    unsigned min_volume;
    // Minimum area below which quadtree recursion won't occur.
    int min_area;
    
    // Position and size of the lattice in cad units
    double xmin,ymin,zmin;
    double dx,dy,dz;

    // Scale factors
    double pixels_per_mm;
    double mm_per_unit;
    
    // Interpolation count for stl, svg
    int quality;
    
    // SVG stroke
    float stroke;
    
    // SVG decimation error (in pixels^2)
    float decimation_error;
    
    // Color, boolean, or real (distance metric)
    solver_mode mode;
    
    // png, svg, stl, or none
    output_mode output;
    
    // 2D or 3D evaluation style?
    bool projection;
    
    // Input and output file names
    std::string infile_name;
    std::string outfile_name;
    
    // The math string of interest
    std::string math_string;
    
    // Pixel arrays for output images
    uint8_t **red,**green,**blue;
    uint16_t **intensity;
    
    // Mutex to lock geometry entities
    boost::mutex geometry_lock;
    
    // Saved set of triangles
    std::list<Triangle> triangles;
    
    // Filled solid volume
    uint64_t volume;
    
    // Saved set of paths
    PathSet paths;
    
    // A progress bar that displays progress in ASCII
    ProgressBar pb;
    
} FabVars;

/* void fab_write_png_K16(FabVars* v, const char* output_file_name)
 *
 *  Outputs a 16-bit PNG based on v.intensity
 */
void fab_write_png_K16(FabVars* v, const char* output_file_name);

/* void fab_write_png_RGB24(FabVars* v, const char* output_file_name)
 *
 *  Outputs a 24-bit PNG based on v.{red, green, blue}
 */
void fab_write_png_RGB24(FabVars* v, const char* output_file_name);

#endif
