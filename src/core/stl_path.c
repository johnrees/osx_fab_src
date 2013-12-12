//
// stl_path.c
//    convert .stl to .path 
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

int main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float units,resolution;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5))) {
      printf("command line: stl_path in.stl out.path [units [resolution]]]\n");
      printf("   in.stl = input binary STL file\n");
      printf("   out.png = output PNG file\n");
      printf("   units = file units (optional, mm/unit, default 1)\n");
      printf("   resolution = image resolution (optional, pixels/mm, default 10)\n");
      exit(-1);
      }
   if (argc == 3) {
      units = 1;
      resolution = 10;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&units);
      resolution = 10;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&units);
      sscanf(argv[4],"%f",&resolution);
      }
   //
   //  read .stl
   //
   fab_read_stl(&v,argv[1]);
   //
   // convert mesh to path
   //
   fab_mesh_path(&v,units,resolution);
   //
   //  write .path
   //
   fab_write_path(&v,argv[2]);
   //
   // return
   //
   return(0);
   }
