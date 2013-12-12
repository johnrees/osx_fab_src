//
// path_png.c
//    convert path to PNG
//
// Neil Gershenfeld
// CBA MIT 2/26/11
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   int x,y,intensity,number,n,count,count_sum;
   float distance,error,z;
   //
   // command line args
   //
   if (argc != 3) {
      printf("command line: path_png in.path out.png\n");
      printf("   in.path = input path file\n");
      printf("   out.png = output PNG file\n");
      exit(-1);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // shade path with z displacement
   //
   fab_shade_path_displace(&v);
   //
   // write PNG
   //
   fab_write_png_K(&v,argv[2]);
   }

