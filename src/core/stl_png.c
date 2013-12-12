//
// stl_png.c
//    convert STL to PNG
//
// Neil Gershenfeld 10/4/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

#include "fab.h"

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float units,resolution;
   char axis;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6))) {
      printf("command line: stl_png in.stl out.png [units [resolution [axis]]]\n");
      printf("   in.stl = input binary STL file\n");
      printf("   out.png = output PNG file\n");
      printf("   units = file units (optional, mm/unit, default 1)\n");
      printf("   resolution = image resolution (optional, pixels/mm, default 10)\n");
      printf("   axis = projection axis (optional, top or bottom, x|X|y|Y|z|Z, default z)\n");
      exit(-1);
      }
   if (argc == 3) {
      units = 1;
      resolution = 10;
      axis = 'z';
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&units);
      resolution = 10;
      axis = 'z';
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&units);
      sscanf(argv[4],"%f",&resolution);
      axis = 'z';
      }
   else if (argc == 6) {
      sscanf(argv[3],"%f",&units);
      sscanf(argv[4],"%f",&resolution);
      sscanf(argv[5],"%c",&axis);
      }
   //
   //  read STL
   //
   fab_read_stl(&v,argv[1]);
   //
   // draw mesh into array
   //
   fab_shade_mesh(&v,units,resolution,axis);
   //
   // move origin to corner
   //
   v.xmin = 0;
   v.ymin = 0;
   v.zmin = 0;
   //
   //  write PNG
   //
   fab_write_png_K(&v,argv[2]);
   }
