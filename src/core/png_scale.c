//
// png_scale.c
//    rescale PNG intensity 
//
// Neil Gershenfeld
// CBA MIT 1/22/11
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
   float min,max;
   //
   // command line args
   //
   if (argc != 5) {
      printf("command line: png_scale in.png out.png low high\n");
      printf("   in.png = input PNG file\n");
      printf("   out.png = output PNG file\n");
      printf("   low = rescaled intensity minimum (0-1)\n");
      printf("   high = rescaled intensity maximum (0-1)\n");
      exit(-1);
      }
   //
   // read PNG
   //
   fab_read_png(&v,argv[1]);
   //
   // rescale
   //
   sscanf(argv[3],"%f",&min);
   sscanf(argv[4],"%f",&max);
   fab_rescale(&v,min,max);
   //
   // write PNG
   //
   fab_write_png_K(&v,argv[2]);
   }

