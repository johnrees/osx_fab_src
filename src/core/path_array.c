//
// path_array.c
//    array path
//
// Neil Gershenfeld
// CBA MIT 10/6/10
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
   struct fab_vars vin;
   init_vars(&vin);
   struct fab_vars vout;
   init_vars(&vout);
   int nx,ny;
   float dx,dy;
   //
   // command line args
   //
   if (!((argc == 5) || (argc == 6) || (argc == 7))) {
      printf("command line: path_array in.path out.path nx ny [dx [dy]]\n");
      printf("   in.path = input path file\n");
      printf("   out.path = output path file\n");
      printf("   nx = number of horizonal array elements\n");
      printf("   ny = number of vertical array elements\n");
      printf("   dx = array element horizontal spacing (optional, mm, default 0)\n");
      printf("   dy = array element vertical spacing (optional, mm, default dx)\n");
      exit(-1);
      }
   if (argc == 5) {
      sscanf(argv[3],"%d",&nx);
      sscanf(argv[4],"%d",&ny);
      dx = 0;
      dy = dx;
      }
   if (argc == 6) {
      sscanf(argv[3],"%d",&nx);
      sscanf(argv[4],"%d",&ny);
      sscanf(argv[5],"%f",&dx);
      dy = dx;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%d",&nx);
      sscanf(argv[4],"%d",&ny);
      sscanf(argv[5],"%f",&dx);
      sscanf(argv[6],"%f",&dy);
      }
   //
   // read path
   //
   fab_read_path(&vin,argv[1]);
   //
   // array path
   //
   fab_path_array(&vin,&vout,nx,ny,dx,dy);
   //
   // write path
   //
   fab_write_path(&vout,argv[2]);
   }

