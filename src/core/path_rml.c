//
// path_rml.c
//    convert path to Roland Modela .rml
//
// Neil Gershenfeld
// CBA MIT 9/6/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_rml(struct fab_vars *v, char *output_file_name, float speed, int direction, float z_up) {
   //
   // write path to Roland Modela file
   //
	FILE *output_file;
   int i,x,y,z,iz_down,iz_up,nsegs=0,npts=0;
   float xscale,yscale,zscale,xoffset,yoffset,zoffset;
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"PA;PA;"); // plot absolute
   fprintf(output_file,"VS%.1f;!VZ%.1f;",speed,speed);
   xscale = 40.0*v->dx/(v->nx-1.0); // 40/mm
   yscale = 40.0*v->dy/(v->ny-1.0); // 40/mm
   if (v->nz > 1)
      zscale = 40.0*v->dz/v->nz; // 40/mm
   else
      zscale = 0;
   xoffset = 40.0*v->xmin;
   yoffset = 40.0*v->ymin;
   zoffset = 40.0*v->zmin;
   iz_up = 40.0*z_up;
   iz_down = zoffset;
   fprintf(output_file,"!PZ%d,%d;",iz_down,iz_up); // set z down, jog 
   fprintf(output_file,"!MC1;\n"); // turn motor on
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
      // move up to first point
      //
      fprintf(output_file,"PU%d,%d;\n",x,y);
      //
      // move down
      //
      if (v->path->dof == 2) {
         fprintf(output_file,"PD%d,%d;\n",x,y);
         }
      else if (v->path->dof == 3) {
         z = zoffset + zscale * v->path->segment->point->first->next->next->value;
         fprintf(output_file,"Z%d,%d,%d;\n",x,y,z);
         }
      else {
         printf("path_rml: path degrees of freedom must be 2 or 3\n");
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
               fprintf(output_file,"PU%d,%d;\n",x,y);
               break;
               }
            }
         else {
            //
            // climb
            //
            if (v->path->segment->point->next == 0) {
               fprintf(output_file,"PU%d,%d;\n",x,y);
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
         if (v->path->dof == 2) {
            fprintf(output_file,"PD%d,%d;\n",x,y);
            }
         else if (v->path->dof == 3) {
            z = zoffset + zscale * v->path->segment->point->first->next->next->value;
            fprintf(output_file,"Z%d,%d,%d;\n",x,y,z);
            }
         else {
            printf("path_rml: path degrees of freedom must be 2 or 3\n");
            exit(-1);
            }
         npts += 1;
         }
      //
      // check for previous segment
      //
      //fprintf(output_file,"\n",x,y);
      if (v->path->segment->previous == 0)
         break;
      v->path->segment = v->path->segment->previous;
      }
   //
   // pad end of file with motor off commands for Modela buffering bug
   //
   for (i = 0; i < 1000; ++i)
      fprintf(output_file,"!MC0;");
   //
   // return to home
   //
   fprintf(output_file,"\nH;\n");
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
   float speed,z_up;
   int direction;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4)  || (argc == 5) || (argc == 6) || (argc == 7) || (argc == 8) || (argc == 9))) {
      printf("command line: path_rml in.path out.rml [speed [direction [jog [xmin ymin [zmin]]]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.rml = output Roland Modela file\n");
      printf("   speed = cutting speed (optional, mm/s, default 4)\n");
      printf("   direction = machining direction (optional, 0 conventional/1 climb, default 1)\n");
      printf("   jog = jog height (optional, mm, default 1)\n");
      printf("   xmin = left position (optional, mm, default path value)\n");
      printf("   ymin = front position (optional, mm, default path value)\n");
      printf("   zmin = bottom position (optional, -mm, default path value)\n");
      exit(-1);
      }
   speed = 4;
   direction = 1;
   z_up = 1;
   if (argc >= 4)
      sscanf(argv[3],"%f",&speed);
   if (argc >= 5)
      sscanf(argv[4],"%d",&direction);
   if (argc >= 6)
      sscanf(argv[5],"%f",&z_up);
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // check origin
   //
   if (argc >= 7)
      sscanf(argv[6],"%lf",&v.xmin);
   if (argc >= 8)
      sscanf(argv[7],"%lf",&v.ymin);
   if (argc >= 9)
      sscanf(argv[8],"%lf",&v.zmin);
   //
   // write .rml
   //
   fab_write_rml(&v,argv[2],speed,direction,z_up);
   }

