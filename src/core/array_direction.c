//
// array_direction.c
//    find array edge directions
//    array_direction in.array out.array threshold
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

int main(int argc, char **argv) {
   //
   // variables
   //
   struct fab_vars v;
   init_vars(&v);
   int threshold;
   //
   // command line args
   //
   if (argc != 4) {
      printf("command line: array_direction in.array out.array threshold\n");
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
   // set directions
   //
   fab_directions(&v);
   //
   // write array
   //
   fab_write_array(&v,argv[2]);
   }

