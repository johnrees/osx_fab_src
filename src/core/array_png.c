//
// array_png.c
//    converted array to a PNG image
//    array_png.c in.array out.png
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
//    z layers
//

#include "fab.h"

int main(int argc, char **argv) {
   //
   // variables
   //
   struct fab_vars v;
   init_vars(&v);
   //
   // command line args
   //
   if (argc != 3) {
      printf("command line: array_png.c in.array out.png\n");
      exit(-1);
      }
   //
   // read array
   //
   fab_read_array(&v,argv[1]);
   //
   // write PNG
   //
   fab_write_png(&v,argv[2]);
   }

