//
// fab.h
//    fab modules header
//
// Neil Gershenfeld 7/4/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include <gif_lib.h>
#include <inttypes.h>

#define fab_big 1e10

//
// data structures
//

struct fab_vars {
   unsigned char empty;
   unsigned char interior;
   unsigned char edge;
   unsigned char north;
   unsigned char south;
   unsigned char east;
   unsigned char west;
   unsigned char stop;
   unsigned char corner;
   unsigned char corner2;
   unsigned char direction;
   unsigned int nx,ny,nz;
   unsigned int bit_depth;
   unsigned int word_size;
   double dx,dy,dz;
   double xmin,ymin,zmin;
   uint32_t **array;
   uint32_t **distances,**g,**h; // for distance transform
   uint32_t *starts,*minimums; // "
   uint32_t **ddx,**ddy; // testing
   struct fab_path_type *path;
   struct fab_mesh_type *mesh;
   png_bytep *row_pointers;
   png_structp png_ptr;
   png_infop info_ptr;
   };

struct fab_path_type {
   struct fab_path_segment_type *first,*segment,*last;
   int dof;
   };

struct fab_path_segment_type {
   struct fab_path_segment_type *previous,*next;
   struct fab_path_point_type *first,*point,*last;
   };

struct fab_path_point_type {
   struct fab_path_point_type *previous,*next;
   struct fab_path_axis_type *first,*axis,*last;
   };

struct fab_path_axis_type {
   struct fab_path_axis_type *previous,*next;
   int value;
   };

struct fab_mesh_type {
   struct fab_mesh_triangle_type *first,*triangle,*last;
   float min[3];
   float max[3];
   int number;
   };

struct fab_mesh_triangle_type {
   struct fab_mesh_triangle_type *previous,*next;
   float normal[3],v0[3],v1[3],v2[3];
   int attribute;
   };

//
// function prototypes
//

//
// initialization
//
void init_vars(struct fab_vars *v);
//
// input
//
void fab_read_stl(struct fab_vars *v, char *input_file_name);
//void fab_read_svg(struct fab_vars *v, char *input_file_name);
void fab_read_png(struct fab_vars *v, char *input_file_name);
void fab_read_array(struct fab_vars *v, char *input_file_name);
void fab_read_path(struct fab_vars *v, char *input_file_name);
//
// output
//
void fab_write_png_K(struct fab_vars *v, char *output_file_name);
void fab_write_array(struct fab_vars *v, char *output_file_name);
void fab_write_path(struct fab_vars *v, char *output_file_name);
//
// shading
//
void fab_shade_states(struct fab_vars *v);
void fab_shade_line(struct fab_vars *v, int x0, int y0, int x1, int y1, int intensity);
void fab_shade_path(struct fab_vars *v);
void fab_shade_path_displace(struct fab_vars *v);
void fab_shade_mesh(struct fab_vars *v, float units, float resolution, char axis);
void fab_shade_triangle(struct fab_vars *v, float units, int S, int X, int Y, int Z);
//
// array operations
//
void fab_png_array(struct fab_vars *v);
void fab_rescale(struct fab_vars *v, float min, float max);
int fab_edges(struct fab_vars *v);
void fab_threshold(struct fab_vars *v, float threshold);
void fab_distances(struct fab_vars *v);
int fab_offset(struct fab_vars *v, float distance);
void fab_directions(struct fab_vars *v);
//
// mesh operations
//
void fab_mesh_path(struct fab_vars *v, float units, float resolution);
//
// path operations
//
void fab_follow(struct fab_vars *v, float error, int y, int x, int z);
void fab_path_start(struct fab_vars *v, int dof);
void fab_path_segment(struct fab_vars *v);
void fab_path_point(struct fab_vars *v);
void fab_path_axis(struct fab_vars *v, int value);
void fab_path_join(struct fab_vars *vin1, struct fab_vars *vin2, struct fab_vars *vout, float dx, float dy);
void fab_path_cat(struct fab_vars *vin1, struct fab_vars *vin2, struct fab_vars *vout);
void fab_path_append(struct fab_vars *vin1, struct fab_vars *vin2);
void fab_path_array(struct fab_vars *vin, struct fab_vars *vout, int nx, int ny, float dx, float dy);
void fab_vectorize(struct fab_vars *v, float error, int z);
void fab_halftone(struct fab_vars *v, float threshold, int points, float size, float spacing, float offset, int invert);
