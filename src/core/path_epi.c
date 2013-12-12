//
// path_epi.c
//    convert path to Epilog lasercutter .epi
//
// Neil Gershenfeld 8/18/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

#include "fab.h"

void fab_write_epi(struct fab_vars *v, char *output_file_name, int power, int max_power, int speed, int focus, int rate) {
   //
   // write path to Epilog lasercutter file
   //
	FILE *output_file;
   int i,x,y,z,current_z,layer_power,nsegs=0,npts=0;
   float scale,xoffset,yoffset;
   output_file = fopen(output_file_name,"w");
   scale = 600.0*v->dx/(25.4*(v->nx-1.0)); // 600 DPI
   xoffset = 600.0*v->xmin/25.4;
   yoffset = 600.0*(v->ymin-v->dy)/25.4;
   if (focus == 0)
      // 
      // init with autofocus off
      //
      fprintf(output_file,"%%-12345X@PJL JOB NAME=%s\r\nE@PJL ENTER LANGUAGE=PCL\r\n&y0A&l0U&l0Z&u600D*p0X*p0Y*t600R*r0F&y50P&z50S*r6600T*r5100S*r1A*rC%%1BIN;XR%d;YP%d;ZS%d;\n",output_file_name,rate,power,speed);
   else
      //
      // init with autofocus on
      //
      fprintf(output_file,"%%-12345X@PJL JOB NAME=%s\r\nE@PJL ENTER LANGUAGE=PCL\r\n&y1A&l0U&l0Z&u600D*p0X*p0Y*t600R*r0F&y50P&z50S*r6600T*r5100S*r1A*rC%%1BIN;XR%d;YP%d;ZS%d;\n",output_file_name,rate,power,speed);
   current_z = 0;
   fprintf(output_file,"YP%d;\n",power);
   v->path->segment = v->path->last;
   while (1) {
      //
      // follow segments in reverse order
      //
      v->path->segment->point = v->path->segment->first;
      // x = xoffset + scale * (v->nx - v->path->segment->point->first->value);
      // y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
      x = xoffset + scale * v->path->segment->point->first->value;
      y = yoffset + scale * v->path->segment->point->first->next->value;
      if (v->path->dof >= 3) {
         z = v->path->segment->point->first->next->next->value;
         if (z != current_z) {
            layer_power = power + (max_power-power) * z / (v->nz - 1.0);
            fprintf(output_file,"YP%d;\n",layer_power);
            current_z = z;
            }
         }
      fprintf(output_file,"PU%d,%d;",x,y);
      nsegs += 1;
      while (1) {
         //
         // follow points
         //
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         // x = xoffset + scale * (v->nx - v->path->segment->point->first->value);
         // y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
         x = xoffset + scale * v->path->segment->point->first->value;
         y = yoffset + scale * v->path->segment->point->first->next->value;
         if (v->path->dof >= 3) {
            z = v->path->segment->point->first->next->next->value;
            if (z != current_z) {
               layer_power = power + (max_power-power) * z / (v->nz - 1.0);
               fprintf(output_file,"YP%d;\n",layer_power);
               current_z = z;
               }
            }
         fprintf(output_file,"PD%d,%d;",x,y);
         npts += 1;
         }
      fprintf(output_file,"\n",x,y);
      if (v->path->segment == v->path->first)
         break;
      v->path->segment = v->path->segment->previous;
      }
   fprintf(output_file,"%%0B%%1BPUE%%-12345X@PJL EOJ \r\n");
   //
   // end-of-file padding hack from Epilog print driver
   //
   for (i = 0; i < 10000; ++i)
      fprintf(output_file," ");
   // fprintf(output_file,"%c",26); // ^z
   //
   // close and return
   //
   fclose(output_file);
   printf("wrote %s\n",output_file_name);
   printf("   segments: %d, points: %d\n",nsegs,npts);
   }

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   int power,max_power,speed,focus,rate;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6) || (argc == 8) || (argc == 9) || (argc == 10))) {
      printf("command line: path_epi in.path out.epi [power [speed [focus [xmin ymin [ [rate [max_power]]]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.epi= output Epilog lasercutter file\n");
      printf("   power = percent power, for minimum z value (optional, 0-100, default 100)\n");
      printf("   speed = percent speed (optional, 0-100, default 100)\n");
      printf("   focus = autofocus (optional, 0=off | 1=on, default on)\n");
      printf("   xmin = left position (optional, mm, default path, 0 = left side of bed)\n");
      printf("   ymin = front position (optional, mm, default path, 0 = back, front positive)\n");
      printf("   rate = pulse rate (optional, frequency, default 2500)\n");
      printf("   max_power = percent power, for maximum z value (optional, 0-100, default power)\n");
      exit(-1);
      }
   if (argc == 3) {
      power = 100;
      speed = 100;
      focus = 1;
      rate = 2500;
      max_power = power;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%d",&power);
      speed = 100;
      focus = 1;
      rate = 2500;
      max_power = power;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      focus = 1;
      rate = 2500;
      max_power = power;
      }
   else if ((argc == 6) || (argc == 8)) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      sscanf(argv[5],"%d",&focus);
      rate = 2500;
      max_power = power;
      }
   else if (argc == 9) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      sscanf(argv[5],"%d",&focus);
      sscanf(argv[8],"%d",&rate);
      max_power = power;
      }
   else if (argc == 10) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      sscanf(argv[5],"%d",&focus);
      sscanf(argv[8],"%d",&rate);
      sscanf(argv[9],"%d",&max_power);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // origin
   //
   if ((argc == 8) || (argc == 9) || (argc == 10)) {
      sscanf(argv[6],"%lf",&v.xmin);
      sscanf(argv[7],"%lf",&v.ymin);
      }
   //
   // write .epi
   //
   fab_write_epi(&v,argv[2],power,max_power,speed,focus,rate);
   }

