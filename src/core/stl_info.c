//
// stl_info.c
//    report .stl info
//
// Neil Gershenfeld
// CBA MIT 3/6/11
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
   float units,resolution;
   char axis;
   //
   // command line args
   //
   if (argc != 2) {
      printf("command line: stl_info in.stl\n");
      printf("   in.stl = input binary STL file\n");
      exit(-1);
      }
   //
   //  read .stl
   //
   fab_read_stl(&v,argv[1]);
   }
