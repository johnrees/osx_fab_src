//
// png_grb.c
//    PNG to Gerber (RS-274X)
//
// Neil Gershenfeld
// CBA MIT 11/22/12
//
// (c) Massachusetts Institute of Technology 2012
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

void fab_write_grb(struct fab_vars *v, char *output_file_name) {
	FILE *output_file;
   int i,j,k,x,y,width,height;
   float units,dx,dy,xmin,ymin;
   output_file = fopen(output_file_name,"w");
   //
   // write header
   //
   fprintf(output_file,"%%FSLAX24Y24*%%\n"); // leading zeros omitted, absolute coordinates, 2.4
   fprintf(output_file,"%%MOIN*%%\n"); // inches units
   fprintf(output_file,"%%OFA0B0*%%\n"); // no offset
   //
   // write apertures
   //
   units = 1.0/25.4; // inches
   dx = units*v->dx/(v->nx-1.0);
   dy = units*v->dy/(v->ny-1.0);
   xmin = units*v->xmin;
   ymin = units*v->ymin;
   for (i = 1; i <= 10; ++i) {
      for (j = 1; j <= 10; ++j) {
         fprintf(output_file,"%%ADD%dR,%.4fX%.4f*%%\n",i*10+j,j*dx,i*dy);
         }
      }
   //
   // write flashes
   //
   for (i = (v->ny-1); i >= 0; --i) {
      for (j = 0; j < v->nx; ++j) {
         //
         // find next starting pixel
         //
         if (v->array[i][j] > 0) {
            //
            // find width
            //
            v->array[i][j] = 0;
            width = 1;
            while ((width < 10) && ((j+width) < v->nx)){
               if (v->array[i][j+width] != 0) {
                  v->array[i][j+width] = 0;
                  width += 1;
                  }
               else
                  break;
               }
            //
            // find height
            //
            height = 1;
            while ((height < 10) && ((i-height) >= 0)){
               for (k = 0; k < width; ++k) {
                  if (v->array[i-height][j+k] == 0)
                     goto height_continue;
                  }
               for (k = 0; k < width; ++k)
                  v->array[i-height][j+k] = 0;
               height += 1;
               }
            height_continue:
            //
            // flash it
            //
            x = 10000*(xmin+dx*(j+width/2.0));
            y = 10000*(ymin+dy*(v->ny-1-i+height/2.0));
            fprintf(output_file,"D%d*\n",height*10+width);
            fprintf(output_file,"X%dY%dD03*\n",x,y);
            }
         }
      }
   //
   // close file
   //
   fprintf(output_file,"M02*\n"); // end of file
   fclose(output_file);
   printf("wrote %s\n",output_file_name);
   }

main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   //
   // command line args
   //
   if ((argc < 3) || (argc > 3)) {
      printf("command line: png_grb in.png out.grb\n");
      printf("   in.png = input PNG file\n");
      printf("   out.grb = output Gerber (RS-274X) file\n");
      exit(-1);
      }
   //
   // read PNG
   //
   fab_read_png(&v,argv[1]);
   //
   // copy image to array
   //
   fab_png_array(&v);
   //
   // write array to Gerber
   //
   fab_write_grb(&v,argv[2]);
   //
   // exit
   //
   exit(0);
   }

