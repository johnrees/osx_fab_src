//
// path_time.c
//    estimate path time
//
// Neil Gershenfeld 10/29/13
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

void fab_find_time(struct fab_vars *v, float units, float move, float height, float jog, float plunge, float *time, int *segments, int *points) {
   float xscale, yscale, zscale;
   float xoffset, yoffset, zoffset;
   float xold, yold, zold, xnew, ynew, znew;
   if ((v->path->dof < 2) || (v->path->dof > 3)) {
         printf("path_time: path degrees of freedom must be 2 or 3\n");
         exit(-1);
         }
   xoffset = v->xmin/units;
   yoffset = v->ymin/units;
   xscale = v->dx/(v->nx-1.0)/units;
   yscale = v->dy/(v->ny-1.0)/units;
   if (v->path->dof == 3) {
      zoffset = v->zmin/units;
      if (v->nz > 1)
         zscale = v->dz/(units*v->nz);
      else
         zscale = 0;
      }
   //
   // start at origin
   //
   xold = 0;
   yold = 0;
   zold = 0;
   *time = 0;
   *segments = 0;
   *points = 0;
   //
   // follow segments
   //
   v->path->segment = v->path->first;
   while (1) {
      *segments += 1;
      //
      // follow segment points
      //
      v->path->segment->point = v->path->segment->first;
      *points += 1;
      xnew = xoffset + xscale * v->path->segment->point->first->value;
      ynew = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
      znew = height;
      //
      // jog to first point
      //
      *time += sqrt((xnew-xold)*(xnew-xold)+(ynew-yold)*(ynew-yold))/jog;
      xold = xnew;
      yold = ynew;
      //
      // plunge to first point
      //
      if (v->path->dof == 3) {
         znew = zoffset + zscale * v->path->segment->point->first->next->next->value;
         *time += sqrt((znew-zold)*(znew-zold))/plunge;
         zold = znew;
         }
      while (1) {
         //
         // check if last point
         //
         if (v->path->segment->point->next == 0) {
            znew = height;
            *time += sqrt((znew-zold)*(znew-zold))/plunge;
            zold = znew;
            break;
            }
         //
         // move to next point
         //
         v->path->segment->point = v->path->segment->point->next;
         *points += 1;
         xnew = xoffset + xscale * v->path->segment->point->first->value;
         ynew = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
         if (v->path->dof == 2) {
            *time += sqrt((xnew-xold)*(xnew-xold)+(ynew-yold)*(ynew-yold))/move;
            xold = xnew;
            yold = ynew;
            }
         else if (v->path->dof == 3) {
            znew = zoffset + zscale * v->path->segment->point->first->next->next->value;
            *time += sqrt((xnew-xold)*(xnew-xold)+(ynew-yold)*(ynew-yold)+(znew-zold)*(znew-zold))/move;
            xold = xnew;
            yold = ynew;
            zold = znew;
            }
         }
      //
      // check for next segment
      //
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      *segments += 1;
      }
   //
   // return
   //
   return;
   }

int main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float units, move, height, jog, plunge, time;
   int segments, points;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6))) {
      printf("command line: path_time in.path move_speed [jog_height [jog_speed [plunge_speed]]]\n");
      printf("   in.path = input path file\n");
      printf("   move_speed = speed of path segments (mm/s)\n");
      printf("   jog_height = height between path segments (mm, optional, default 0)\n");
      printf("   jog_speed = speed between path segments (mm/s, optional, default move_speed)\n");
      printf("   plunge_speed = speed from jog to move  (mm/s, optional, default move_speed)\n");
      exit(-1);
      }
   if (argc == 3) {
      sscanf(argv[2],"%f",&move);
      height = 0;
      jog = move;
      plunge = move;
      }
   else if (argc == 4) {
      sscanf(argv[2],"%f",&move);
      sscanf(argv[3],"%f",&height);
      jog = move;
      plunge = move;
      }
   else if (argc == 5) {
      sscanf(argv[2],"%f",&move);
      sscanf(argv[3],"%f",&height);
      sscanf(argv[4],"%f",&jog);
      plunge = move;
      }
   else if (argc == 6) {
      sscanf(argv[2],"%f",&move);
      sscanf(argv[3],"%f",&height);
      sscanf(argv[4],"%f",&jog);
      sscanf(argv[5],"%f",&plunge);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // calculate time
   //
   fab_find_time(&v,1.0,move,height,jog,plunge,&time,&segments,&points);
   //
   // report and return
   //
   printf("path time: %.0f seconds, %.1f minutes, %.2f hours\n",time,time/60.0,time/(60.0*60.0));
   }
