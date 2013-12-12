//
// path_uni.c
//    convert path to Universal lasercutter .uni
//
// Neil Gershenfeld
// CBA MIT 9/21/11
//
// (c) Massachusetts Institute of Technology 2011
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_uni(struct fab_vars *v, char *output_file_name, int power, int max_power, int speed, int rate) {
   //
   // write path to Universal lasercutter file
   //
	FILE *output_file;
   int x,y,z,current_z,layer_power,nsegs=0,npts=0;
   unsigned char ppi,speed_hi,speed_lo,power_hi,power_lo;
   float scale,xoffset,yoffset;
   //
   output_file = fopen(output_file_name,"w");
   scale = 1000.0*v->dx/(25.4*(v->nx-1.0)); // 1000 DPI
   xoffset = 1000.0*v->xmin/25.4;
   yoffset = 1000.0*v->ymin/25.4;
   fprintf(output_file,"Z"); // initialize
   fprintf(output_file,"t%s~;",output_file_name); // title
   fprintf(output_file,"IN;DF;PS0;DT~"); // initialize
   ppi = rate/10;
   fprintf(output_file,"s%c",ppi); // PPI
   speed_hi = (648*speed)/256;
   speed_lo = (648*speed)%256;
   fprintf(output_file,"v%c%c",speed_hi,speed_lo); // speed
   power_hi = (320*power)/256;
   power_lo = (320*power)%256;
   fprintf(output_file,"p%c%c",power_hi,power_lo); // power
   fprintf(output_file,"a%c",2); // air assist on high
   current_z = 0;
   v->path->segment = v->path->last;
   while (1) {
      //
      // follow segments in reverse order
      //
      v->path->segment->point = v->path->segment->first;
      x = xoffset + scale * v->path->segment->point->first->value;
      y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
      if (v->path->dof == 3) {
         z = v->path->segment->point->first->next->next->value;
         if (z != current_z) {
            layer_power = power + (max_power-power) * z / (v->nz - 1.0);
            power_hi = (320*layer_power)/256;
            power_lo = (320*layer_power)%256;
            fprintf(output_file,"p%c%c",power_hi,power_lo); // power
            current_z = z;
            }
         }
      fprintf(output_file,"PU;PA%d,%d;PD;",x,y);
      nsegs += 1;
      while (1) {
         //
         // follow points
         //
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         x = xoffset + scale * v->path->segment->point->first->value;
         y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
         if (v->path->dof == 3) {
            z = v->path->segment->point->first->next->next->value;
            if (z != current_z) {
               layer_power = power + (max_power-power) * z / (v->nz - 1.0);
               fprintf(output_file,"YP%d;\n",layer_power);
               current_z = z;
               }
            }
         fprintf(output_file,"PA%d,%d;",x,y);
         npts += 1;
         }
      fprintf(output_file,"\n",x,y);
      if (v->path->segment == v->path->first)
         break;
      v->path->segment = v->path->segment->previous;
      }
   fprintf(output_file,"e"); // end of file
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
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 7) || (argc == 8) || (argc == 9))) {
      printf("command line: path_uni in.path out.uni [power [speed [xmin ymin [rate [max_power]]]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.uni= output Universal lasercutter file\n");
      printf("   power = percent power (optional, 0-100, default 100)\n");
      printf("   speed = percent speed (optional, 0-100, default 100)\n");
      printf("   xmin = left position (optional, mm, default path, 0 = left side of bed)\n");
      printf("   ymin = front position (optional, mm, default path, 0 = back, front positive)\n");
      printf("   rate = pulse rate (optional, frequency, default 500)\n");
      printf("   max_power = maximum power for maximum z value (optional, 0-100, default 100)\n");
      exit(-1);
      }
   if (argc == 3) {
      power = 100;
      speed = 100;
      rate = 500;
      max_power = 100;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%d",&power);
      speed = 100;
      rate = 500;
      max_power = 100;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      rate = 500;
      max_power = 100;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      rate = 500;
      max_power = 100;
      }
   else if (argc == 8) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      sscanf(argv[7],"%d",&rate);
      max_power = 100;
      }
   else if (argc == 9) {
      sscanf(argv[3],"%d",&power);
      sscanf(argv[4],"%d",&speed);
      sscanf(argv[7],"%d",&rate);
      sscanf(argv[8],"%d",&max_power);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // origin
   //
   if ((argc == 7) || (argc == 8) || (argc == 9)) {
      sscanf(argv[5],"%lf",&v.xmin);
      sscanf(argv[6],"%lf",&v.ymin);
      }
   //
   // write .epi
   //
   fab_write_uni(&v,argv[2],power,max_power,speed,rate);
   }

