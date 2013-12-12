//
// path_info.c
//    report .path info
//
// Neil Gershenfeld 10/3/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//
// todo
//    gradient intensity
//

#include "fab.h"

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float units,resolution;
   char axis;
   //
   // command line args
   //
   if (argc != 2) {
      printf("command line: path_info in.path\n");
      printf("   in.path = input path file\n");
      exit(-1);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   }
