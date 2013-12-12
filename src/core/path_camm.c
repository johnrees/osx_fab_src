//
// path_camm.c
//    convert path to Roland vinylcutter .camm
//
// Neil Gershenfeld
// CBA MIT 9/1/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_camm(struct fab_vars *v, char *output_file_name, float force, float velocity) {
   //
   // write path to Roland vinylcutter file
   //
	FILE *output_file;
   int x,y,nsegs=0,npts=0;
   float scale,xoffset,yoffset;
   output_file = fopen(output_file_name,"w");
   fprintf(output_file,"PA;PA;!ST1;!FS%f;VS%f;\n",force,velocity);
   scale = 40.0*v->dx/(v->nx-1.0); // 40/mm
   xoffset = 40.0*v->xmin;
   yoffset = 40.0*v->ymin;
   v->path->segment = v->path->last;
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      x = xoffset + scale * v->path->segment->point->first->value;
      y = yoffset + scale * (v->ny - v->path->segment->point->first->next->value);
      fprintf(output_file,"PU%d,%d;\n",x,y); // move up to start point
      fprintf(output_file,"PU%d,%d;\n",x,y); // hack: repeat in case comm dropped
      fprintf(output_file,"PD%d,%d;\n",x,y); // move down
      fprintf(output_file,"PD%d,%d;\n",x,y); // hack: repeat in case comm dropped
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
         fprintf(output_file,"PD%d,%d;\n",x,y); // move down
         fprintf(output_file,"PD%d,%d;\n",x,y); // hack: repeat in case comm dropped
         npts += 1;
         }
      //fprintf(output_file,"\n",x,y);
      if (v->path->segment->previous == 0)
         break;
      v->path->segment = v->path->segment->previous;
      }
   fprintf(output_file,"PU0,0;\n"); // pen up to origin
   fprintf(output_file,"PU0,0;\n"); // hack: repeat in case comm dropped
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
   float xmin,ymin,force,velocity;
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 7))) {
      printf("command line: path_camm in.path out.camm [force [velocity [xmin ymin]]]\n");
      printf("   in.path = input path file\n");
      printf("   out.camm = output Roland vinylcutter file\n");
      printf("   force = cutting force (optional, grams, default 45)\n");
      printf("   velocity = cutting speed (optional, cm/s, default 2)\n");
      printf("   xmin = left position (optional, mm, default path value)\n");
      printf("   ymin = bottom position (optional, mm, default path value)\n");
      exit(-1);
      }
   if (argc == 3) {
      force = 45;
      velocity = 2;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&force);
      velocity = 2;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&force);
      sscanf(argv[4],"%f",&velocity);
      }
   //
   // read path
   //
   fab_read_path(&v,argv[1]);
   //
   // origin
   //
   if (argc == 7) {
      sscanf(argv[5],"%lf",&v.xmin);
      sscanf(argv[6],"%lf",&v.ymin);
      }
   //
   // write .epi
   //
   fab_write_camm(&v,argv[2],force,velocity);
   }

