//
// path_sbp.c
//    convert path to ShopBot .sbp
//
// Neil Gershenfeld
// CBA MIT 10/1/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_sbp(struct fab_vars *v, char *output_file_name, int direction, int spindle_speed, float xy_speed, float z_speed, float xy_jog_speed, float z_jog_speed, float z_jog, float units) {
   //
   // write path to ShopBot file
   //
	FILE *output_file;
   int i,nsegs=0,npts=0;
   float xscale,yscale,zscale,x,y,z,xoffset,yoffset,zoffset;
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"SA\r\n"); // set to absolute distances
   fprintf(output_file,"TR,%d,1\r\n",spindle_speed); // set spindle speed
   fprintf(output_file,"SO,1,1\r\n"); // set output number 1 to on
   fprintf(output_file,"pause,2\r\n"); // let spindle come up to speed
   xscale = v->dx/(v->nx-1.0)/units;
   yscale = v->dy/(v->ny-1.0)/units;
   if (v->nz > 1)
      zscale = v->dz/(units*v->nz);
   else
      zscale = 0;
   xoffset = v->xmin/units;
   yoffset = v->ymin/units;
   zoffset = v->zmin/units;
   xy_speed = xy_speed/units;
   z_speed = z_speed/units;
   xy_jog_speed = xy_jog_speed/units;
   z_jog_speed = z_jog_speed/units;
   z_jog = z_jog/units;
   fprintf(output_file,"MS,%f,%f\r\n",xy_speed,z_speed); // set xy,z speed
   fprintf(output_file,"JS,%f,%f\r\n",xy_jog_speed,z_jog_speed); // set jog xy,z speed
   fprintf(output_file,"JZ,%f\r\n",z_jog); // move up
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
      fprintf(output_file,"J2,%f,%f\r\n",x,y);
      //
      // move down
      //
      if (v->path->dof == 2)
         fprintf(output_file,"MZ,%f\r\n",zoffset);
      else if (v->path->dof == 3) {
         z = zoffset + zscale * v->path->segment->point->first->next->next->value;
         fprintf(output_file,"M3,%f,%f,%f\r\n",x,y,z);
         }
      else {
         printf("path_sbp: path degrees of freedom must be 2 or 3\n");
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
               fprintf(output_file,"MZ,%f\r\n",z_jog);
               break;
               }
            }
         else {
            //
            // climb
            //
            if (v->path->segment->point->next == 0) {
               fprintf(output_file,"MZ,%f\r\n",z_jog);
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
            fprintf(output_file,"M2,%f,%f\r\n",x,y);
         else if (v->path->dof == 3) {
            z = zoffset + zscale * v->path->segment->point->first->next->next->value;
            fprintf(output_file,"M3,%f,%f,%f\r\n",x,y,z);
            }
         else {
            printf("path_sbp: path degrees of freedom must be 2 or 3\n");
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
   fclose(output_file);
   printf("wrote %s\n",output_file_name);
   printf("   segments: %d, points: %d\n",nsegs,npts);
   }

int main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float xy_speed, z_speed, xy_jog_speed, z_jog_speed, z_jog, units;
   int direction,spindle_speed;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 7) || (argc == 10) || (argc == 11))) {
      printf("command line: path_sbp in.path out.sbp [direction [spindle_speed [xy_speed z_speed [xy_jog_speed z_jog_speed z_jog [units]]]]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.sbp = output ShopBot file\n");
      printf("   direction = machining direction (optional, 0 conventional/1 climb, default 0)\n");
      printf("   spindle_speed = spindle speed (optional, if control installed, RPM, default 12000)\n");
      printf("   xy_speed = xy cutting speed (optional, mm/s, default 30)\n");
      printf("   z_speed = z cutting speed (optional, mm/s, default 30)\n");
      printf("   xy_jog_speed = xy jog speed (optional, mm/s, default 150)\n");
      printf("   z_jog_speed = z jog speed (optional, mm/s, default 150)\n");
      printf("   z_jog = z jog height (optional, mm, default 25)\n");
      printf("   units = mm per file unit (optional, default 25.4)\n");
      exit(-1);
      }
   if (argc == 3) {
      direction = 0;
      spindle_speed = 12000;
      xy_speed = 30;
      z_speed = 30;
      xy_jog_speed = 150;
      z_jog_speed = 150;
      z_jog = 25;
      units = 25.4;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%d",&direction);
      spindle_speed = 12000;
      xy_speed = 30;
      z_speed = 30;
      xy_jog_speed = 150;
      z_jog_speed = 150;
      z_jog = 25;
      units = 25.4;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%d",&spindle_speed);
      xy_speed = 30;
      z_speed = 30;
      xy_jog_speed = 150;
      z_jog_speed = 150;
      z_jog = 25;
      units = 25.4;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%d",&spindle_speed);
      sscanf(argv[5],"%f",&xy_speed);
      sscanf(argv[6],"%f",&z_speed);
      xy_jog_speed = 150;
      z_jog_speed = 150;
      z_jog = 25;
      units = 25.4;
      }
   else if (argc == 10) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%d",&spindle_speed);
      sscanf(argv[5],"%f",&xy_speed);
      sscanf(argv[6],"%f",&z_speed);
      sscanf(argv[7],"%f",&xy_jog_speed);
      sscanf(argv[8],"%f",&z_jog_speed);
      sscanf(argv[9],"%f",&z_jog);
      units = 25.4;
      }
   else if (argc == 11) {
      sscanf(argv[3],"%d",&direction);
      sscanf(argv[4],"%d",&spindle_speed);
      sscanf(argv[5],"%f",&xy_speed);
      sscanf(argv[6],"%f",&z_speed);
      sscanf(argv[7],"%f",&xy_jog_speed);
      sscanf(argv[8],"%f",&z_jog_speed);
      sscanf(argv[9],"%f",&z_jog);
      sscanf(argv[10],"%f",&units);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // write .sbp
   //
   fab_write_sbp(&v,argv[2],direction,spindle_speed,xy_speed,z_speed,xy_jog_speed,z_jog_speed,z_jog,units);
   return 0;
   }

