//
// path_dxf.c
//    convert path to DXF
//    todo: 3D paths
//
// Neil Gershenfeld
// CBA MIT 8/25/12
//
// (c) Massachusetts Institute of Technology 2012
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_dxf(struct fab_vars *v, char *output_file_name) {
   //
   // write path to DXF file
   //
   int x,y,z;
   float x0,y0,x1,y1;
	FILE *output_file;
   int nsegs=0, npts=0;
   float scale,units;
   units = 1/25.4;
   scale = v->dx/(v->nx-1.0);
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"999\nDXF written by fab modules path_dxf\n");
   fprintf(output_file,"0\nSECTION\n");
   fprintf(output_file,"2\nHEADER\n");
   fprintf(output_file,"9\n$EXTMIN\n");
   fprintf(output_file,"10\n%f\n",units*v->xmin);
   fprintf(output_file,"20\n%f\n",units*v->ymin);
   fprintf(output_file,"9\n$EXTMAX\n");
   fprintf(output_file,"10\n%f\n",units*(v->xmin+v->dx));
   fprintf(output_file,"20\n%f\n",units*(v->ymin+v->dy));
   fprintf(output_file,"0\nENDSEC\n");
   fprintf(output_file,"0\nSECTION\n");
   fprintf(output_file,"2\nTABLES\n");
   fprintf(output_file,"0\nTABLE\n");
   fprintf(output_file,"2\nLTYPE\n70\n1\n");
   fprintf(output_file,"0\nLTYPE\n");
   fprintf(output_file,"2\nCONTINUOUS\n");
   fprintf(output_file,"70\n64\n3\n");
   fprintf(output_file,"Solid line\n");
   fprintf(output_file,"72\n65\n73\n0\n40\n0.000000\n");
   fprintf(output_file,"0\nENDTAB\n");
   fprintf(output_file,"0\nTABLE\n2\nLAYER\n70\n1\n");
   fprintf(output_file,"0\nLAYER\n2\ndefault\n70\n64\n62\n7\n6\n");
   fprintf(output_file,"CONTINUOUS\n0\nENDTAB\n");
   fprintf(output_file,"0\nENDSEC\n");
   fprintf(output_file,"0\nSECTION\n");
   fprintf(output_file,"2\nBLOCKS\n");
   fprintf(output_file,"0\nENDSEC\n");
   fprintf(output_file,"0\nSECTION\n");
   fprintf(output_file,"2\nENTITIES\n");
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
         }
      x0 = units*(v->xmin+scale*x);
      y0 = units*(v->ymin+scale*y);
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
         x1 = units*(v->xmin+scale*x);
         y1 = units*(v->ymin+scale*y);
         fprintf(output_file,"0\nLINE\n");
         fprintf(output_file,"10\n%f\n",x0);
         fprintf(output_file,"20\n%f\n",y0);
         fprintf(output_file,"11\n%f\n",x1);
         fprintf(output_file,"21\n%f\n",y1);
         x0 = x1;
         y0 = y1;
         if (v->path->dof == 3) {
            z = v->path->segment->point->first->next->next->value;
            }
         npts += 1;
         }
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   fprintf(output_file,"0\nENDSEC\n");
   fprintf(output_file,"0\nEOF\n");
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
   if (!(argc == 3)) {
      printf("command line: path_dxf in.path out.dxf\n");
      printf("   in.path = input path file\n");
      printf("   out.dxf = output DXF file\n");
      exit(-1);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // write dxf
   //
   fab_write_dxf(&v,argv[2]);
   }

