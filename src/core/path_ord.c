//
// path_ord.c
//    convert path to Omax waterjet .ord
//
// Neil Gershenfeld
// CBA MIT 10/5/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"
#define nostart -10000

void fab_write_ord(struct fab_vars *v, char *output_file_name, float lead, int quality, float xstart, float ystart) {
   //
   // write path to ShopBot file
   //
	FILE *output_file;
   int i,nsegs=0,npts=0;
   float units,xscale,yscale,x,y,xmin,ymin,x0,y0,x1,y1,xoffset,yoffset,xlead,ylead,dx,dy,norm_x,norm_y,norm;
   output_file = fopen(output_file_name,"w");
   units = 1.0/25.4; // inches
   xscale = units*v->dx/(v->nx-1.0);
   yscale = units*v->dy/(v->ny-1.0);
   xoffset = units*v->xmin;
   yoffset = units*v->ymin;
   lead = units*lead;
   //
   // write start if supplied
   //
   if (xstart != nostart) {
      xstart = units*xstart;
      ystart = units*ystart;
      fprintf(output_file,"%f, %f, 0, 0\n",xstart,ystart); // rapid traverse from lead-out
      }
   //
   // follow segments in reverse order
   //
   v->path->segment = v->path->last;
   while (1) {
      //
      // follow points in forward order
      //
      v->path->segment->point = v->path->segment->first;
      //
      // calculate and write lead-in perpendicular to segment start
      //
      x0 = xoffset + xscale * v->path->segment->point->first->value;
      y0 = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
      x1 = xoffset + xscale * v->path->segment->point->next->first->value;
      y1 = yoffset + yscale * (v->ny - v->path->segment->point->next->first->next->value);
      dx = x1 - x0;
      dy = y1 - y0;
      norm_x = -dy;
      norm_y = dx;
      norm = sqrt(norm_x*norm_x + norm_y*norm_y);
      norm_x = norm_x/norm;
      norm_y = norm_y/norm;
      xlead = x0 + norm_x*lead;
      ylead = y0 + norm_y*lead;
      fprintf(output_file,"%f, %f, 0, -9\n",xlead,ylead); // lead-in
      fprintf(output_file,"%f, %f, 0, %d\n",x0,y0,quality); // first point
      nsegs += 1;
      while (1) {
         if (v->path->segment->point->next == 0) {
            //
            // no next point in segment, write lead-out
            //
            x0 = xoffset + xscale * v->path->segment->point->previous->first->value;
            y0 = yoffset + yscale * (v->ny - v->path->segment->point->previous->first->next->value);
            x1 = xoffset + xscale * v->path->segment->point->first->value;
            y1 = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
            dx = x1 - x0;
            dy = y1 - y0;
            norm_x = -dy;
            norm_y = dx;
            norm = sqrt(norm_x*norm_x + norm_y*norm_y);
            norm_x = norm_x/norm;
            norm_y = norm_y/norm;
            xlead = x1 + norm_x*lead;
            ylead = y1 + norm_y*lead;
            fprintf(output_file,"%f, %f, 0, 0\n",xlead,ylead); // rapid traverse from lead-out
            break;
            }
         v->path->segment->point = v->path->segment->point->next;
         x = xoffset + xscale * v->path->segment->point->first->value;
         y = yoffset + yscale * (v->ny - v->path->segment->point->first->next->value);
         if (v->path->segment->point->next != 0)
            fprintf(output_file,"%f, %f, 0, %d\n",x,y,quality);
         else
            fprintf(output_file,"%f, %f, 0, -9\n",x,y); // lead-out next
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

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   float lead,xstart,ystart;
   int quality;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 7))) {
      printf("command line: path_ord in.path out.ord [lead [quality [xstart ystart]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.ord = output Omax waterjet file\n");
      printf("   lead = lead in/out (optional, mm, default 2)\n");
      printf("   quality = cut quality (optional, default -3)\n");
      printf("   xstart,ystart = start position (optional, mm, default path start)\n");
      exit(-1);
      }
   if (argc == 3) {
      lead = 2.0;
      quality = -3;
      xstart = nostart;
      ystart = nostart;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&lead);
      quality = -3;
      xstart = nostart;
      ystart = nostart;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&lead);
      sscanf(argv[4],"%d",&quality);
      xstart = nostart;
      ystart = nostart;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%f",&lead);
      sscanf(argv[4],"%d",&quality);
      sscanf(argv[5],"%f",&xstart);
      sscanf(argv[6],"%f",&ystart);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // write .ord
   //
   fab_write_ord(&v,argv[2],lead,quality,xstart,ystart);
   }

