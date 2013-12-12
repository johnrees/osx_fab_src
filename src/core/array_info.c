//
// array_info.c
//    read and print array info
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
   if (argc != 2) {
      printf("command line: array_info in.array\n");
      exit(-1);
      }
   //
   // read array
   //
   fab_read_array(&v,argv[1]);
   }

