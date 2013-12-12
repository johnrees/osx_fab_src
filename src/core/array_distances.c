//
// array_distances.c
//    find distances from edges
//    array_distances in.array out.array threshold
//
// Neil Gershenfeld
// CBA MIT 8/5/10
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
   int x,y,threshold;
   float distance;
   //
   // command line args
   //
   if (argc != 4) {
      printf("command line: array_distances in.array out.array threshold\n");
      exit(-1);
      }
   //
   // read array
   //
   fab_read_array(&v,argv[1]);
   //
   // threshold
   //
   sscanf(argv[3],"%d",&threshold);
   fab_threshold(&v,threshold);
   //
   // find edges
   //
   fab_edges(&v);
   //
   // find edge distances
   //
   fab_distances(&v);
   //
   // put distances into array
   //
   for (y = 0; y < v.ny; ++y) {
      for (x = 0; x < v.nx; ++x) {
         if (v.xptr[y][x] != -1) {
            distance = sqrt((v.xptr[y][x]-x)*(v.xptr[y][x]-x)
               + (v.yptr[y][x]-y)*(v.yptr[y][x]-y));
            v.array[y][x] = (int) distance;
            }
         else
            v.array[y][x] = 128;
         }
      }
   //
   // write array
   //
   fab_write_array(&v,argv[2]);
   }

