//
// png_offset.c
//    offset PNG
//    png_offset in.png out.png threshold distance
//
// Neil Gershenfeld
// CBA MIT 9/11/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//
// todo
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
   int x,y,count;
   float intensity,distance;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5))) {
      printf("command line: png_offset in.png out.png [intensity [distance]]\n");
      printf("   in.png = input PNG file\n");
      printf("   out.png = input PNG file\n");
      printf("   intensity = intensity level to slice (optional, 0-1, default 0.5)\n");
      printf("   distance = distance to offset (optional, mm, default 0)\n");
      exit(-1);
      }
   if (argc == 3) {
      intensity = 0.5;
      distance = 0;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&intensity);
      distance = 0;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&intensity);
      sscanf(argv[4],"%f",&distance);
      }
   //
   // read PNG
   //
   fab_read_png(&v,argv[1]);
   //
   // threshold
   //
   fab_threshold(&v,intensity);
   //
   // find edges
   //
   fab_edges(&v);
   //
   // find edge distances
   //
   fab_distances(&v);
   //
   // offset
   //
   count = fab_offset(&v,distance);
   printf("png_offset: %d points remain\n",count);
   //
   // set edge directions
   //
   fab_directions(&v);
   //
   // shade states
   //
   fab_shade_states(&v);
   //
   // write PNG
   //
   fab_write_png_K(&v,argv[2]);
   }

