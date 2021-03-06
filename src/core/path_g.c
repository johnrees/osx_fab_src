//
// path_g.c
//    convert path to G codes
//
// Neil Gershenfeld, David Carr
// CBA MIT 8/10/11
//
// (c) Massachusetts Institute of Technology 2011
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_g(struct fab_vars *v, char *output_file_name, int direction, float z_jog, float feed_rate, float z_feed_rate, float spindle_speed, int tool, int coolant) {
   //
   // write path to G-code file
   //
	FILE *output_file;
   int i,nsegs=0,npts=0;
   float units,xscale,yscale,zscale,x,y,z,xoffset,yoffset,zoffset;
   output_file = fopen(output_file_name,"w");
   units = 1.0/25.4; // inches
   xscale = units*v->dx/(v->nx-1.0);
   yscale = units*v->dy/(v->ny-1.0);
   if (v->nz > 1)
      zscale = units*v->dz/v->nz;
   else
      zscale = 0;
   xoffset = units*v->xmin;
   yoffset = units*v->ymin;
   zoffset = units*v->zmin;
   z_jog = units*z_jog;
   feed_rate = 60*units*feed_rate; // feed rate in inches per minute
   z_feed_rate = 60*units*z_feed_rate;
   //
   // Write G code header
   //
   //fprintf(output_file, "; G-Code generated by MIT CBA fab modules\n");
   fprintf(output_file, "%%\n"); // tape start
   //fprintf(output_file, "; http://kokompe.cba.mit.edu/dist\n\n");
   // Clear all state: XY plane, inch mode, cancel diameter compensation, cancel length offset
   // coordinate system 1, cancel motion, non-incremental motion, feed/minute mode
   fprintf(output_file,"G17\n");
   fprintf(output_file,"G20\n");
   fprintf(output_file,"G40\n");
   fprintf(output_file,"G49\n");
   fprintf(output_file,"G54\n");
   fprintf(output_file,"G80\n");
   fprintf(output_file,"G90\n");
   fprintf(output_file,"G94\n");
   fprintf(output_file,"T%dM06\n",tool); // tool selection, tool change
   fprintf(output_file,"F%0.4f\n",feed_rate); // feed rate
   fprintf(output_file,"S%0.4f\n",spindle_speed); // spindle speed
   if (coolant == 1)
      fprintf(output_file,"M08\n"); // coolant on
   fprintf(output_file,"G00Z%0.4f\n",z_jog); // move up before starting spindle
   fprintf(output_file,"M03\n"); // spindle on clockwise
   fprintf(output_file,"G04 P1\n"); // give spindle 1 second to spin up
   //
   // follow segments in reverse order (mill boundaries last)
   //
   v->path->segment = v->path->last;
   while (1) {
      if (direction == 0)
         //
         // conventional
         //
         v->path->segment->point = v->path->segment->last;
      else
         //
         // climb
         //
         v->path->segment->point = v->path->segment->first;
      x = xoffset + xscale * v->path->segment->point->first->value;
      y = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
      //
      // move to first point
      //
      fprintf(output_file,"G00X%0.4fY%0.4fZ%0.4f\n",x,y,z_jog);
      //
      // move down
      //
      if (v->path->dof == 2) {
	fprintf(output_file,"G01Z%0.4f F%0.4f\n",zoffset,z_feed_rate);
	fprintf(output_file,"F%0.4f\n",feed_rate); //restore XY feed rate
      }
      else if (v->path->dof == 3) {
         z = zoffset + zscale * v->path->segment->point->first->next->next->value;
         fprintf(output_file,"G01Z%0.4f F%0.4f\n",z,z_feed_rate);
	 fprintf(output_file,"F%0.4f\n",feed_rate);
         }
      else {
         printf("path_g: path degrees of freedom must be 2 or 3\n");
         exit(-1);
         }
      nsegs += 1;
      while (1) {
         //
         // check if last point
         //
         if (direction == 0) {
            //
            // conventional
            //
            if (v->path->segment->point->previous == 0) {
               fprintf(output_file,"Z%0.4f\n",z_jog);
               break;
               }
            }
         else {
            //
            // climb
            //
            if (v->path->segment->point->next == 0) {
               fprintf(output_file,"Z%0.4f\n",z_jog);
               break;
               }
            }
         //
         // move to next point
         //
         if (direction == 0)
            //
            // conventional
            //
            v->path->segment->point = v->path->segment->point->previous;
         else
            //
            // climb
            //
            v->path->segment->point = v->path->segment->point->next;
         x = xoffset + xscale * v->path->segment->point->first->value;
         y = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
         if (v->path->dof == 2)
            fprintf(output_file,"X%0.4fY%0.4f\n",x,y);
         else if (v->path->dof == 3) {
            z = zoffset + zscale * v->path->segment->point->first->next->next->value;
            fprintf(output_file,"X%0.4fY%0.4fZ%0.4f\n",x,y,z);
            }
         else {
            printf("path_g: path degrees of freedom must be 2 or 3\n");
            exit(-1);
            }
         npts += 1;
         }
      //
      // check for previous segment
      //
      if (v->path->segment->previous == 0)
         break;
      v->path->segment = v->path->segment->previous;
      }
   //
   // close and return
   //
   fprintf(output_file,"G00Z%0.4f\n",z_jog); // move up before stopping spindle
   fprintf(output_file,"M05\n"); // spindle stop
   if (coolant == 1)
      fprintf(output_file,"M09\n"); // coolant off
   fprintf(output_file,"M30\n"); // program end and reset
   fprintf(output_file, "%%\n"); // tape end
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
   float z_jog,feed_rate,z_feed_rate,spindle_speed;
   int direction,tool,coolant;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6) || (argc == 7) || (argc == 8) || (argc == 9) || (argc == 10))) {
      printf("command line: path_g in.path out.g [direction [z_jog [feed [z_feed [spindle [tool [coolant]]]]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.g = output G-code file\n");
      printf("   direction = machining direction (optional, 0 conventional/1 climb, default 0)\n");
      printf("   z_jog = z jog height (optional, mm, default 25)\n");
      printf("   feed = feed rate (optional, mm/s, default 100)\n");
      printf("   z_feed = z plunge rate (optional, mm/s, default xy feed rate)\n");
      printf("   spindle = spindle speed (optional, RPM, default 5000)\n");
      printf("   tool = tool number (optional, default 1)\n");
      printf("   coolant = coolant on/off (optional, 0=off/1=on, default 1)\n");
      exit(-1);
      }
   if (argc == 3) {
      direction = 0;
      z_jog = 25.0;
      feed_rate = 5.0;
      z_feed_rate = feed_rate;
      spindle_speed = 5000.0;
      tool = 1;
      coolant = 1;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%d",&direction);
      z_jog = 25.0;
      feed_rate = 5.0;
      z_feed_rate = feed_rate;
      spindle_speed = 5000.0;
      tool = 1;
      coolant = 1;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      feed_rate = 5.0;
      z_feed_rate = feed_rate;
      spindle_speed = 5000.0;
      tool = 1;
      coolant = 1;
      }
   else if (argc == 6) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      sscanf(argv[5],"%f",&feed_rate);
      z_feed_rate = feed_rate;
      spindle_speed = 5000.0;
      tool = 1;
      coolant = 1;
      }
    else if (argc == 7) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      sscanf(argv[5],"%f",&feed_rate);
      sscanf(argv[6],"%f",&z_feed_rate);
      spindle_speed = 5000.0;
      tool = 1;
      coolant = 1;
      }
   else if (argc == 8) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      sscanf(argv[5],"%f",&feed_rate);
      sscanf(argv[6],"%f",&z_feed_rate);
      sscanf(argv[7],"%f",&spindle_speed);
      tool = 1;
      coolant = 1;
      }
   else if (argc == 9) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      sscanf(argv[5],"%f",&feed_rate);
      sscanf(argv[6],"%f",&z_feed_rate);
      sscanf(argv[7],"%f",&spindle_speed);
      sscanf(argv[8],"%d",&tool);
      coolant = 1;
      }
   else if (argc == 10) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%f",&z_jog);
      sscanf(argv[5],"%f",&feed_rate);
      sscanf(argv[6],"%f",&z_feed_rate);
      sscanf(argv[7],"%f",&spindle_speed);
      sscanf(argv[8],"%d",&tool);
      sscanf(argv[9],"%d",&coolant);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // write G codes
   //
   fab_write_g(&v,argv[2],direction,z_jog,feed_rate,z_feed_rate,spindle_speed,tool,coolant);
   }

