//
// array_offset.c
//    offset array
//    array_offset in.array out.array threshold distance
//
// Neil Gershenfeld
// CBA MIT 7/30/10
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
   int x,y,threshold,count;
   float distance;
   //
   // command line args
   //
   if (argc != 5) {
      printf("command line: array_offset in.array out.array threshold distance\n");
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
   // offset
   //
   distance = atof(argv[4]);
   count = fab_offset(&v,distance);
   printf("%d points remain\n",count);
   //
   // set edge directions
   //
   fab_directions(&v);
   //
   // write array
   //
   fab_write_array(&v,argv[2]);
   }

