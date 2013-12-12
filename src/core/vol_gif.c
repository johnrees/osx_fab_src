//
// vol_gif.c
//    .vol to .gif
//
// Neil Gershenfeld 11/29/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//

/*
gcc -O3 vol_gif.c -o vol_gif -D_FILE_OFFSET_BITS=64 -Wall -lm -lgif
*/

#include "fab.h"

void read_voxel_f(FILE *input_file,float *f,int x,int y,int z,int *xi,int *yi,int *zi,int ix,int iy,int iz) {
   int r,match=0;
   while (1) {
      if ((x == *xi) && (y == *yi) && (z == *zi)) {
         match = 1;
         }
      r = fread(f,sizeof(*f),1,input_file);
      if (r == 0) {
         printf("vol_gif: oops -- not enough points in file\n");
         exit(-1);
         }
      *xi += 1;
      if (*xi == ix) {
         *xi = 0;
         *yi += 1;
         if (*yi == iy) {
            *yi = 0;
            *zi += 1;
            }
         }
      if (match == 1)
         return;
      }
   }

void read_voxel_i(FILE *input_file,uint16_t *i,int x,int y,int z,int *xi,int *yi,int *zi,int ix,int iy,int iz) {
   int r,match=0;
   while (1) {
      if ((x == *xi) && (y == *yi) && (z == *zi)) {
         match = 1;
         }
      r = fread(i,sizeof(*i),1,input_file);
      if (r == 0) {
         printf("vol_gif: oops -- not enough points in file\n");
         exit(-1);
         }
      *xi += 1;
      if (*xi == ix) {
         *xi = 0;
         *yi += 1;
         if (*yi == iy) {
            *yi = 0;
            *zi += 1;
            }
         }
      if (match == 1)
         return;
      }
   }

int main(int argc, char **argv) {
   FILE *input_file;
   float arg,size,rx,ry,rz;
   float f,fmin=1e10,fmax=-1e10;
   uint16_t i;
   uint64_t count;
   int xi,yi,zi,xo,yo,zo,nx,ny,nz,dx,dy,dz,x0,y0,z0,h;
   int **image;
   char format,type,comment[256];
   GifFileType *GIFfile;
   ColorMapObject *GIFcmap;
   GifPixelType *GIFline;
   //
   // command line args
   //
   if (!((argc == 6) || (argc == 7)  || (argc == 8) || (argc == 9) || (argc == 10) || (argc == 13) || (argc == 16) || (argc == 19))) {
      printf("command line: vol_gif in.vol out.gif nx ny nz [format [type [arg [size [dx dy dz [x0 y0 z0 [rx ry rz]]]]]]]\n");
      printf("   in.vol = input volume file\n");
      printf("   out.gif = output GIF file\n");
      printf("   nx,ny,nz = x,y,z input voxel number\n");
      printf("   format = 'f' for float 32, 'i' for uint16_t (default 'f')\n");
      printf("   type = 's' for section, 'h' for height (default 's')\n");
      printf("   arg = gamma for 's', threshold for 'h' (default 1)\n");
      printf("   size = mm per voxel (default 1)\n");
      printf("   dx,dy,dz = x,y,z output voxel number (default all)\n");
      printf("   x0,y0,z0 = x,y,z output voxel origin (default 0)\n");
      printf("   to be implemented: rx,ry,rz = view rotation angles (degrees; default 0)\n");
      exit(-1);
      }
   format = 'f';
   type = 's';
   arg = 1;
   size = 1.0;
   rx = ry = rz = 0;
   sscanf(argv[3],"%d",&nx);
   sscanf(argv[4],"%d",&ny);
   sscanf(argv[5],"%d",&nz);
   dx = nx; dy = ny; dz = nz;
   x0 = y0 = z0 = 0;
   if (argc >= 7) {
      sscanf(argv[6],"%c",&format);
      if (!((format == 'f') || (format == 'i'))) {
         printf("vol_gif: oops -- format must be 'f' or 'i'\n");
         exit(-1);
         }
      }
   if (argc >= 8) {
      sscanf(argv[7],"%c",&type);
      if (!((type == 's') || (type == 'h'))) {
         printf("vol_gif: oops -- type must be 's' or 'h'\n");
         exit(-1);
         }
      }
   if (argc >= 9) {
      sscanf(argv[8],"%f",&arg);
      }
   if (argc >= 10) {
      sscanf(argv[9],"%f",&size);
      }
   if (argc >= 13) {
      sscanf(argv[10],"%d",&x0);
      sscanf(argv[11],"%d",&y0);
      sscanf(argv[12],"%d",&z0);
      }
   if (argc >= 16) {
      sscanf(argv[13],"%d",&dx);
      sscanf(argv[14],"%d",&dy);
      sscanf(argv[15],"%d",&dz);
      }
   if (argc >= 19) {
      sscanf(argv[16],"%f",&rx);
      sscanf(argv[17],"%f",&ry);
      sscanf(argv[18],"%f",&rz);
      }
   //
   // check and find limits
   //
   input_file = fopen(argv[1],"rb");
   if (input_file == NULL) {
      printf("vol_gif: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   if (((x0 + dx) > nx) || ((y0 + dy) > ny) || ((z0 + dz) > nz)) {
      printf("vol_gif: oops -- region too large\n");
      exit(-1);
      }
   printf("read %s\n",argv[1]);
   if (format == 'f') {
      count = 0;
      while (fread(&f,sizeof(f),1,input_file) != 0) {
         if (f > fmax) fmax = f;
         if (f < fmin) fmin = f;
         count += 1;
         }
      }
   else if (format == 'i') {
      count = 0;
      while (fread(&i,sizeof(i),1,input_file) != 0) {
         if (i > fmax) fmax = i;
         if (i < fmin) fmin = i;
         count += 1;
         }
      }
   printf("   %" PRIu64 " points, min %f, max %f\n",count,fmin,fmax);
   printf("   nx ny nz: %d %d %d\n",nx,ny,nz);
   rewind(input_file);
   //
   // set up color map
   //
   GIFcmap = MakeMapObject(256, NULL);
   for (i = 0; i < 256; i++) {
      GIFcmap->Colors[i].Red = i;
      GIFcmap->Colors[i].Green = i;
      GIFcmap->Colors[i].Blue = i;
      }
   //
   // open GIF file
   //
   printf("write %s\n",argv[2]);
   GIFfile = EGifOpenFileName(argv[2], 0);
   EGifPutScreenDesc(GIFfile,dx,dy,8,0,GIFcmap);
   unsigned char loop_count[] = {1,0,0};
   EGifPutExtensionFirst(GIFfile, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0");
   EGifPutExtensionLast(GIFfile, APPLICATION_EXT_FUNC_CODE, 3, loop_count);
   unsigned char delay_count[5] = { 
      0, // no transparency
      0, // delay time
      0, // delay time
      0 // transparent index not used
      };
   //
   // allocate image
   //
   image = malloc(dy*sizeof(int *));
   for (yo = 0; yo < dy; ++yo) {
      image[yo] = malloc(dx*sizeof(int));
      for (xo = 0; xo < dx; ++xo)
         image[yo][xo] = 0;
      }
   GIFline = malloc(dx*sizeof(GifPixelType));
   //
   // scan file
   //
   xi = yi = zi = 0;
   for (zo = 0; zo < dz; ++zo) {
      printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b   layer = %d",zo);
      EGifPutExtension(GIFfile,GRAPHICS_EXT_FUNC_CODE,4,delay_count);
      EGifPutImageDesc(GIFfile,0,0,dx,dy,0,NULL);
      //
      // read layer
      //
      for (yo = 0; yo < dy; ++yo) {
         for (xo = 0; xo < dx; ++xo) {
            if (format == 'f') {
               read_voxel_f(input_file,&f,xo+x0,yo+y0,zo+z0,&xi,&yi,&zi,nx,ny,nz);
               if (type == 'h') {
                  h = 255*zo/(nz-1.0);
                  if ((h > image[yo][xo]) && (f > arg))
                     image[yo][xo] = h;
                  GIFline[xo] = image[yo][xo]*(nz-1.0)/zo;
                  }
               else if (type == 's') {
                  GIFline[xo] = 255*pow((f-fmin)/(fmax-fmin),arg);
                  }
               }
            else if (format == 'i') {
               read_voxel_i(input_file,&i,xo+x0,yo+y0,zo+z0,&xi,&yi,&zi,nx,ny,nz);
               if (type == 'h') {
                  h = 255*zo/(nz-1.0);
                  if ((h > image[yo][xo]) && (i > arg))
                     image[yo][xo] = h;
                  GIFline[xo] = image[yo][xo]*(nz-1.0)/zo;
                  }
               else if (type == 's') {
                  GIFline[xo] = 255*pow((i-fmin)/(fmax-fmin),arg);
                  }
               }
            }
         EGifPutLine(GIFfile,GIFline,dx);
         }
      }
   printf("\n");
   //
   // put mm per pixel in comment
   //
   sprintf(comment,"mm per pixel: %f;",size);
   EGifPutComment(GIFfile,comment);
   //
   // exit
   //
   fclose(input_file);
   EGifCloseFile(GIFfile);
   exit(0);
   }
