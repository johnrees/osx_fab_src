//
// png_distances.c
//    find distances from edges in PNG
//
// Neil Gershenfeld
// CBA MIT 7/24/11
//
// (c) Massachusetts Institute of Technology 2011
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
   int x,y,distances;
   float intensity,distance;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5))) {
      printf("command line: png_distances in.png out.png [intensity [distances]]\n");
      printf("   in.png = input PNG file\n");
      printf("   out.png = input PNG file\n");
      printf("   intensity = intensity level to slice (optional, 0-1, default 0.5)\n");
      printf("   distances = show distances (optional, 0/1, default 1)\n");
      exit(-1);
      }
   if (argc == 3) {
      intensity = 0.5;
      distances = 1;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&intensity);
      distances = 1;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&intensity);
      sscanf(argv[4],"%d",&distances);
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
   // find distances
   //
   if (distances == 1) {
      printf("   find distances\n");
      fab_distances(&v);
      //
      // copy distances to image array
      //
      for (y = 0; y < v.ny; ++y)
         for (x = 0; x < v.nx; ++x)
            v.array[y][x] = v.distances[y][x];
      }
   //
   // scale 
   //
   v.bit_depth = 16;
   fab_rescale(&v,0,1);
   //
   // write PNG
   //
   fab_write_png_K(&v,argv[2]);
   }

