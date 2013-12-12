//
// path_join.c
//    join paths
//
// Neil Gershenfeld
// CBA MIT 11/6/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars vin1;
   init_vars(&vin1);
   struct fab_vars vin2;
   init_vars(&vin2);
   struct fab_vars vout;
   init_vars(&vout);
   float dx,dy;
   //
   // command line args
   //
   if (!((argc == 4) || (argc == 5) || (argc == 6))) {
      printf("command line: path_join in1.path in2.path out.path [dx [dy]]\n");
      printf("   in1.path = first input path file\n");
      printf("   in2.path = second input path file\n");
      printf("   out.path = joined output path file\n");
      printf("   dx = in1 horizontal offset (optional, mm, default 0)\n");
      printf("   dy = in1 vertical offset (optional, mm, default dx)\n");
      exit(-1);
      }
   if (argc == 4) {
      dx = 0;
      dy = dx;
      }
   if (argc == 5) {
      sscanf(argv[4],"%f",&dx);
      dy = dx;
      }
   else if (argc == 6) {
      sscanf(argv[4],"%f",&dx);
      sscanf(argv[5],"%f",&dy);
      }
   //
   // read paths
   //
   fab_read_path(&vin1,argv[1]);
   fab_read_path(&vin2,argv[2]);
   //
   // append path
   //
   fab_path_join(&vin1,&vin2,&vout,dx,dy);
   //
   // write path
   //
   fab_write_path(&vout,argv[3]);
   }

