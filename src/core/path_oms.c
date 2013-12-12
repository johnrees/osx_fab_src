//
// path_oms.c
//    convert path to Resonetics excimer micromachining center .oms
//
// Neil Gershenfeld
// CBA MIT 6/4/13
//
// (c) Massachusetts Institute of Technology 2013
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_oms(struct fab_vars *v, char *output_file_name, float velocity, float acceleration, int period) {
   //
   // write path to Epilog lasercutter file
   //
	FILE *output_file;
   float x,y,scale,xoffset,yoffset;
   float slew_velocity = 1.0;
   float slew_acceleration = 5.0;
   int npts=0,nsegs=0;
   int settle = 100;
   scale = v->dx/(v->nx-1.0); // mm
   xoffset = v->xmin;
   yoffset = v->ymin;
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"AA LP0,0,0,0,0\n"); // set origin
   fprintf(output_file,"PP%d\n",period); // set pulse period (in us)
   v->path->segment = v->path->first;
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      x = xoffset + scale * v->path->segment->point->first->value;
      y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
      //fprintf(output_file,"VL%.1f,%.1f\n",slew_velocity,slew_velocity);
      // redundantly include pulses/s in VL
      fprintf(output_file,"VL%.1f,%.1f,,%.0f\n",slew_velocity,slew_velocity,1.0e6/period);
      fprintf(output_file,"AC%.1f,%.1f\n",slew_acceleration,slew_acceleration);
      fprintf(output_file,"MA%f,%f\n",x,y);
      //fprintf(output_file,"VL%.1f,%.1f\n",velocity,velocity);
      // redundantly include pulses/s in VL
      fprintf(output_file,"VL%.1f,%.1f,,%.0f\n",velocity,velocity,1.0e6/period);
      fprintf(output_file,"AC%.1f,%.1f\n",acceleration,acceleration);
      fprintf(output_file,"WT%d\n",settle);
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
         fprintf(output_file,"CutAbs %f,%f\n",x,y);
         npts += 1;
         }
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   fprintf(output_file,"END\n");
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
   float velocity, acceleration;
   int period;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6))) {
      printf("command line: path_oms in.path out.oms [velocity [acceleration [period]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.oms = output Resonetics excimer micromachining center file\n");
      printf("   velocity (default 0.1)\n");
      printf("   acceleration (default 5.0)\n");
      printf("   period (usec, default 10000)\n");
      exit(-1);
      }
   if (argc == 3) {
      velocity = 0.1;
      acceleration = 5.0;
      period = 10000;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&velocity);
      acceleration = 5.0;
      period = 10000;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&velocity);
      sscanf(argv[4],"%f",&acceleration);
      period = 10000;
      }
   else if (argc == 6) {
      sscanf(argv[3],"%f",&velocity);
      sscanf(argv[4],"%f",&acceleration);
      sscanf(argv[5],"%d",&period);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // write .oms
   //
   fab_write_oms(&v,argv[2],velocity,acceleration,period);
   }
