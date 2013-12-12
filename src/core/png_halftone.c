//
// png_halftone.c
//    halftone PNG to path
//
// Neil Gershenfeld
// CBA MIT 9/9/10
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
   int points, invert;
   float threshold, size, spacing, offset;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6) || (argc == 7) || (argc == 8) || (argc == 9))) {
      printf("command line: png_halftone in.png out.path [threshold [points [size [spacing [offset [invert]]]]]]\n");
      printf("   in.png = input PNG file\n");
      printf("   out.path = output path file\n");
      printf("   threshold = minimum spot radius (optional, pixels default 1)\n");
      printf("   points = points per spot (optional, default 8)\n");
      printf("   size = maximum spot size (optional, mm, default 1)\n");
      printf("   spacing = spot spacing (optional, 1 = size, default 1)\n");
      printf("   offset = row offset (optional, 1 = size, default 0.5)\n");
      printf("   offset = row offset (optional, 1 = size, default 0.5)\n");
      printf("   invert = invert image (0 = no (default), 1 = yes)\n");
      exit(-1);
      }
   if (argc == 3) {
      threshold = 1.0;
      points = 8;
      size = 1.0;
      spacing = 1.0;
      offset = 0.5;
      invert = 0;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&threshold);
      points = 8;
      size = 1.0;
      spacing = 1.0;
      offset = 0.5;
      invert = 0;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&threshold);
      sscanf(argv[4],"%d",&points);
      size = 1.0;
      spacing = 1.0;
      offset = 0.5;
      invert = 0;
      }
   else if (argc == 6) {
      sscanf(argv[3],"%f",&threshold);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%f",&size);
      spacing = 1.0;
      offset = 0.5;
      invert = 0;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%f",&threshold);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%f",&size);
      sscanf(argv[6],"%f",&spacing);
      offset = 0.5;
      invert = 0;
      }
   else if (argc == 8) {
      sscanf(argv[3],"%f",&threshold);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%f",&size);
      sscanf(argv[6],"%f",&spacing);
      sscanf(argv[7],"%f",&offset);
      invert = 0;
      }
   else if (argc == 9) {
      sscanf(argv[3],"%f",&threshold);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%f",&size);
      sscanf(argv[6],"%f",&spacing);
      sscanf(argv[7],"%f",&offset);
      sscanf(argv[8],"%d",&invert);
      }
   //
   // read PNG
   //
   fab_read_png(&v,argv[1]);
   //
   // halftone
   //
   fab_halftone(&v,threshold,points,size,spacing,offset,invert);
   //
   // write path
   //
   fab_write_path(&v,argv[2]);
   }

