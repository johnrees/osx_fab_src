//
// path_eps.c
//    convert .path to .eps
//
// Neil Gershenfeld 7/4/13
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

void fab_write_eps(struct fab_vars *v, char *output_file_name) {
   //
   // write path to PostScript file
   //
   int x,y,z,current_z;
   float margin = 0.5; // lower left margin, in inches
   float gray;
	FILE *output_file;
   int nsegs=0, npts=0;
   float scale;
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"%%! path_eps output\n");
   fprintf(output_file,"%%%%BoundingBox: %f %f %f %f\n",72.0*margin,72.0*margin,
      72.0*(margin+v->dx/25.4),72.0*(margin+v->dy/25.4));
   fprintf(output_file,"/m {moveto} def\n");
   fprintf(output_file,"/l {lineto} def\n");
   fprintf(output_file,"/g {setgray} def\n");
   fprintf(output_file,"/s {stroke} def\n");
   fprintf(output_file,"72 72 scale\n");
   fprintf(output_file,"%f %f translate\n",margin,margin);
   fprintf(output_file,"1 setlinewidth\n");
   scale = v->dx/(25.4*(v->nx-1.0));
   fprintf(output_file,"%f %f scale\n",scale,scale);
   current_z = 0;
   fprintf(output_file,"0 g\n");
   v->path->segment = v->path->first;
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      x = v->path->segment->point->first->value;
      y = v->ny - v->path->segment->point->first->next->value;
      if (v->path->dof == 3) {
         z = v->path->segment->point->first->next->next->value;
         if (z != current_z) {
            gray = 0.9 * z / (v->nz - 1.0);
            fprintf(output_file,"%.3f g\n",gray);
            current_z = z;
            }
         }
      fprintf(output_file,"%d %d m\n",x,y);
      nsegs += 1;
      while (1) {
         //
         // follow points
         //
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         x = v->path->segment->point->first->value;
         y = v->ny - v->path->segment->point->first->next->value;
         fprintf(output_file,"%d %d l\n",x,y);
         if (v->path->dof == 3) {
            z = v->path->segment->point->first->next->next->value;
            if (z != current_z) {
               gray = 0.9 * 0.5 * (z + current_z) / (v->nz - 1.0);
               fprintf(output_file,"s %.3f g %d %d m\n",gray,x,y);
               current_z = z;
               }
            }
         npts += 1;
         }
      fprintf(output_file,"s\n");
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   //fprintf(output_file,"showpage\n");
   fclose(output_file);
   printf("write %s\n",output_file_name);
   printf("   segments: %d, points: %d\n",nsegs,npts);
   }

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   struct fab_vars vnew;
   init_vars(&vnew);
   char view;
   int x,y,z,nz,dz;
   float scale;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4))) {
      printf("command line: path_eps in.path out.eps [view]\n");
      printf("   in.path = input path file\n");
      printf("   out.eps= output PostScript file\n");
      printf("   view = view projection(s) (optional, z|3, default z)\n");
      exit(-1);
      }
   view = 'z';
   if (argc == 4)
      view = argv[3][0];
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // check view
   //
   if (view == 'z') {
      //
      // write eps
      //
      fab_write_eps(&v,argv[2]);
      }
   else if (view == '3') {
      if (v.path->dof < 3) {
         printf("path_eps: oops -- path not 3D\n");
         exit(-1);
         }
      nz = v.nx * v.dz / v.dx;
      scale = ((float) nz) / v.nz;
      vnew.nx = v.nx + nz;
      vnew.ny = v.ny + nz;
      vnew.nz = v.nz;
      vnew.dx = v.dx + v.dz;
      vnew.dy = v.dy + v.dz;
      vnew.dz = v.dz;
      vnew.xmin = v.xmin;
      vnew.ymin = v.ymin;
      vnew.zmin = v.zmin;
      fab_path_start(&vnew,v.path->dof);
      //
      // follow path
      //
      v.path->segment = v.path->first;
      while (1) {
         //
         // follow segments
         //
         v.path->segment->point = v.path->segment->first;
         fab_path_segment(&vnew);
         while (1) {
            //
            // follow points
            //
            fab_path_point(&vnew);
            z = v.path->segment->point->first->next->next->value;
            dz = nz - scale*z;
            y = dz + v.path->segment->point->first->next->value;
            x = dz + v.path->segment->point->first->value;
            fab_path_axis(&vnew,x);
            fab_path_axis(&vnew,y);
            fab_path_axis(&vnew,z);
            if (v.path->segment->point->next == 0)
               break;
            v.path->segment->point = v.path->segment->point->next;
            }
         if (v.path->segment->next == 0)
            break;
         v.path->segment = v.path->segment->next;
         }
      //
      // write eps
      //
      fab_write_eps(&vnew,argv[2]);
      }
   }

