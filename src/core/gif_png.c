//
// gif_png.c
//    .gif to .png
//
// Neil Gershenfeld 9/20/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

#define MAX_LINE 10000
#define BIG 1e10

/*
gcc -O3 gif_png.c ~/hg/kokompe/dev/mod/src/core/fab.c -o gif_png -D_FILE_OFFSET_BITS=64 -Wall -lm -lpng -lgif
*/

#include "fab.h"

int main(int argc, char **argv) {
   //
   // local vars
   //
   GifFileType *GIFfile;
   GifRecordType GIFtype;
   GifByteType *GIFextension;
   GifPixelType *GIFline;
   uint32_t **lower_array,**upper_array;
   double **image_array;
   double f,fmin,fmax;
   int x,y,z,h,i,j,k,n,p;
   int image_width,image_height,image_count,color_resolution,GIFcode,ret;
   float pixel_size,arg,rx,ry,rz;
   char type,comment[256];
   struct fab_vars v;
   init_vars(&v);
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6) || (argc == 7) || (argc == 10))) {
      printf("command line: gif_png in.gif out.png [type [arg [points [size [rx ry rz]]]]]\n");
      printf("   in.gif = input gif file\n");
      printf("   out.png = output PNG file\n");
      printf("   type = 'z' of density, 'h' for height (default z)\n");
      printf("   arg = gamma for 'z', threshold for 'h' (default 1)\n");
      printf("   points = points to interpolate per point (linear, default 0)\n");
      printf("   size = voxel size (mm, default from file))\n");
      printf("   to be implemented: rx,ry,rz = x,y,z rotation angles (degrees; default 0)\n");
      exit(-1);
      }
   type = 'z';
   p = 0;
   arg = 1;
   rx = ry = rz = 0;
   pixel_size = -1;
   image_width = -1;
   image_height = -1;
   if (argc >= 4) {
      sscanf(argv[3],"%c",&type);
      if (!((type == 'z') || (type == 'h'))) {
         printf("gif_png: oops -- type must be 'z' or 'h'\n");
         exit(-1);
         }
   if (argc >= 5)
      sscanf(argv[4],"%f",&arg);
      }
   if (argc >= 6)
      sscanf(argv[5],"%d",&p);
   if (argc >= 7)
      sscanf(argv[6],"%f",&pixel_size);
   if (argc >= 10) {
      sscanf(argv[7],"%f",&rx);
      sscanf(argv[8],"%f",&ry);
      sscanf(argv[9],"%f",&rz);
      }
   //
   // scan the file 
   //
   printf("read %s\n",argv[1]);
   color_resolution = -1;
   GIFfile = DGifOpenFileName(argv[1]);
   if (GIFfile == NULL) {
      printf("gif_png: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   GIFline = malloc(MAX_LINE*sizeof(GifPixelType));
   do {
      DGifGetRecordType(GIFfile,&GIFtype);
      switch (GIFtype) {
         case IMAGE_DESC_RECORD_TYPE:
            DGifGetImageDesc(GIFfile);
            image_width = GIFfile->SWidth;
            image_height = GIFfile->SHeight;
            image_count = GIFfile->ImageCount;
            color_resolution = GIFfile->SColorResolution;
            for (y = 0; y < GIFfile->SHeight; ++y) {
               ret = DGifGetLine(GIFfile,GIFline,GIFfile->SWidth);
               if (ret != GIF_OK) {
                  printf("gif_png: oops -- error reading line\n");
                  exit(-1);
                  }
               }
            break;
         case EXTENSION_RECORD_TYPE:
            DGifGetExtension(GIFfile,&GIFcode,&GIFextension);
            if (GIFcode == COMMENT_EXT_FUNC_CODE) {
               n = GIFextension[0];
               for (i = 1; i <= n; ++i)
                  comment[i-1] = GIFextension[i];
               comment[n] = 0;
               if (pixel_size == -1)
                  sscanf(comment,"mm per pixel: %f;",&pixel_size);
               }
            while (GIFextension != NULL)
               DGifGetExtensionNext(GIFfile,&GIFextension);
            break;
         case SCREEN_DESC_RECORD_TYPE:
            DGifGetScreenDesc(GIFfile);
            break;
         case TERMINATE_RECORD_TYPE:
            break;
         case UNDEFINED_RECORD_TYPE:
            printf("gif_png: oops -- undefined GIF record type\n");
            exit(-1);
            break;
         }
      } while (GIFtype != TERMINATE_RECORD_TYPE);
   if (GIFfile == NULL) {
      printf("gif_png: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   if (pixel_size == -1) {
      pixel_size = 1.0;
      printf("   no pixel size found, assuming 1 mm\n");
      }
   printf("   pixel size (mm): %f, color resolution (bits): %d\n",pixel_size,color_resolution);
   printf("   number of images: %d, image width %d, image height %d\n",image_count,image_width,image_height);
   //
   // check and set limits
   //
   v.nx = image_width + (image_width-1)*p;
   v.ny = image_height + (image_height-1)*p;
   v.nz = image_count + (image_count-1)*p;
   v.dx = v.nx*pixel_size;
   v.dy = v.ny*pixel_size;
   v.dz = v.nz*pixel_size;
   v.xmin = 0;
   v.ymin = 0;
   v.zmin = 0;
   //
   // allocate arrays
   //
   lower_array = malloc(image_height*sizeof(uint32_t *));
   for (y = 0; y < image_height; ++y) {
      lower_array[y] = malloc(image_width*sizeof(uint32_t));
      for (x = 0; x < image_width; ++x)
         lower_array[y][x] = 0;
      }
   upper_array = malloc(image_height*sizeof(uint32_t *));
   for (y = 0; y < image_height; ++y) {
      upper_array[y] = malloc(image_width*sizeof(uint32_t));
      for (x = 0; x < image_width; ++x)
         upper_array[y][x] = 0;
      }
   image_array = malloc(v.ny*sizeof(double *));
   for (y = 0; y < v.ny; ++y) {
      image_array[y] = malloc(v.nx*sizeof(double));
      for (x = 0; x < v.nx; ++x)
         image_array[y][x] = 0;
      }
   v.array = malloc(v.ny*sizeof(uint32_t *));
   for (y = 0; y < v.ny; ++y) {
      v.array[y] = malloc(v.nx*sizeof(uint32_t));
      for (x = 0; x < v.nx; ++x)
         v.array[y][x] = 0;
      }
   //
   // read the file
   //
   DGifCloseFile(GIFfile);
   GIFfile = DGifOpenFileName(argv[1]);
   if (GIFfile == NULL) {
      printf("gif_png: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   z = 0;
   do {
      DGifGetRecordType(GIFfile,&GIFtype);
      switch (GIFtype) {
         case IMAGE_DESC_RECORD_TYPE:
            //
            // read image
            //
            DGifGetImageDesc(GIFfile);
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b   layer = %d",z);
            if (z == 0) {
               //
               // read first layer
               //
               for (y = 0; y < image_height; ++y) {
                  ret = DGifGetLine(GIFfile,GIFline,GIFfile->SWidth);
                  if (ret != GIF_OK) {
                     printf("gif_png: oops -- error reading first line\n");
                     exit(-1);
                     }
                  for (x = 0; x < image_width; ++x)
                     upper_array[y][x] = GIFline[x];
                  }
               }
            else {
               //
               // read next layer
               //
               for (y = 0; y < image_height; ++y) {
                  ret = DGifGetLine(GIFfile,GIFline,GIFfile->SWidth);
                  if (ret != GIF_OK) {
                     printf("gif_png: oops -- error reading line\n");
                     exit(-1);
                     }
                  for (x = 0; x < image_width; ++x) {
                     lower_array[y][x] = upper_array[y][x];
                     upper_array[y][x] = GIFline[x];
                     }
                  }
               //
               // interpolate layer image
               //
               for (x = 0; x < (image_width-1); ++x) {
                  for (y = 0; y < (image_height-1); ++y) {
                     for (i = 0; i <= p; ++i) {
                        for (j = 0; j <= p; ++j) {
                           for (k = 0; k <= p; ++k) {
                              f = lower_array[y][x]*((p+1.0-i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((p+1.0-k)/(p+1.0))
                                + lower_array[y][x+1]*((i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((p+1.0-k)/(p+1.0))
                                + lower_array[y+1][x]*((p+1.0-i)/(p+1.0))*((j)/(p+1.0))*((p+1.0-k)/(p+1.0))
                                + lower_array[y+1][x+1]*((i)/(p+1.0))*((j)/(p+1.0))*((p+1.0-k)/(p+1.0))
                                + upper_array[y][x]*((p+1.0-i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((k)/(p+1.0))
                                + upper_array[y][x+1]*((i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((k)/(p+1.0))
                                + upper_array[y+1][x]*((p+1.0-i)/(p+1.0))*((j)/(p+1.0))*((k)/(p+1.0))
                                + upper_array[y+1][x+1]*((i)/(p+1.0))*((j)/(p+1.0))*((k)/(p+1.0));
                              if (type == 'z') {
                                 image_array[(1+p)*y+j][(1+p)*x+i] += f;
                                 }
                              else if (type == 'h') {
                                 h = (1+p)*z+k;
                                 if ((f > arg) && (h > image_array[(1+p)*y+j][(1+p)*x+i]))
                                    image_array[(1+p)*y+j][(1+p)*x+i] = h;
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            z += 1;
            break;
         case EXTENSION_RECORD_TYPE:
            DGifGetExtension(GIFfile,&GIFcode,&GIFextension);
            while (GIFextension != NULL)
               DGifGetExtensionNext(GIFfile,&GIFextension);
            break;
         case SCREEN_DESC_RECORD_TYPE:
            DGifGetScreenDesc(GIFfile);
            break;
         case TERMINATE_RECORD_TYPE:
            break;
         case UNDEFINED_RECORD_TYPE:
            printf("gif_png: oops -- undefined GIF record type\n");
            exit(-1);
            break;
         }
      } while (GIFtype != TERMINATE_RECORD_TYPE);
   printf("\n");
   //
   // scale image and copy to PNG array
   //
   if (type == 'z') {
      fmin = BIG;
      fmax = 0;
      for (x = 0; x < v.nx; ++x) {
         for (y = 0; y < v.ny; ++y) {
            if (image_array[y][x] > fmax) fmax = image_array[y][x];
            if (image_array[y][x] < fmin) fmin = image_array[y][x];
            }
         }
      if (arg == 1) {
         for (x = 0; x < v.nx; ++x)
            for (y = 0; y < v.ny; ++y)
               v.array[y][x] = 65536*(image_array[y][x]-fmin)/(fmax-fmin);
         }
      else {
         for (x = 0; x < v.nx; ++x)
            for (y = 0; y < v.ny; ++y)
               v.array[y][x] = 65536*pow((image_array[y][x]-fmin)/(fmax-fmin),arg);
         }
      }
   else if (type == 'h') {
      for (x = 0; x < v.nx; ++x)
         for (y = 0; y < v.ny; ++y)
            v.array[y][x] = 65536*image_array[y][x]/(v.nz-1.0);
      }
   //
   // write PNG
   //
   v.bit_depth = 16;
   fab_write_png_K(&v,argv[2]);
   //
   // exit
   //
   DGifCloseFile(GIFfile);
   exit(0);
   }
