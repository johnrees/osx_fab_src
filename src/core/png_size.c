//
// png_size.c
//    display and edit PNG size
//
// Neil Gershenfeld
// CBA MIT 8/28/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//
// todo
//    variable units
//    pipe I/O
//    variable bit depth
//

#include "fab.h"

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float dx,dy;
   //
   // command line args
   //
   if (!((argc == 2) || (argc == 3) || (argc == 4))) {
      printf("command line: png_size in.png [dx [dy]]\n");
      printf("   in.png = input PNG file\n");
      printf("   dx = set width (optional, mm)\n");
      printf("   dy = set height (optional, mm)\n");
      exit(-1);
      }
   //
   // read image
   //
   fab_read_png(&v,argv[1]);
   //
   // rescale if size given
   //
   if (argc == 3) {
      sscanf(argv[2],"%f",&dx);
      dy = v.dy * dx / ((float) v.dx);
      v.dx = dx;
      v.dy = dy;
      fab_write_png_K(&v,argv[1]);
      }
   else if (argc == 4) {
      sscanf(argv[2],"%f",&dx);
      sscanf(argv[3],"%f",&dy);
      v.dx = dx;
      v.dy = dy;
      fab_write_png_K(&v,argv[1]);
      }
   }

