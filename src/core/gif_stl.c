//
// gif_stl.c
//    .gif to .stl
//
// Neil Gershenfeld 11/4/13
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

/*
gcc -O3 gif_stl.c ~/hg/kokompe/dev/mod/src/core/fab.c -o gif_stl -D_FILE_OFFSET_BITS=64 -Wall -lm -lgif -lpng
*/

#include "fab.h"

/*
vertices:
   ---
   6 7
   4 5
   ---
   2 3
   0 1
   ---
edges:
   ---
    k
   l j
    i
   ---
   h g
   e f
   ---
    c
   d b
    a
   ---
*/

//
// rotate_x
//   rotate rule around x and add
//
void rotate_x(char rules[255][20], int *index) {
   int i,b[8];
   int old_index = *index;
   for (i = 0; i < 8; ++i) b[i] = (*index & (1 << i)) >> i;
   *index = 
      (b[4] << 0)
     +(b[5] << 1)
     +(b[0] << 2)
     +(b[1] << 3)
     +(b[6] << 4)
     +(b[7] << 5)
     +(b[2] << 6)
     +(b[3] << 7);
   for (i
    = 0; i < strlen(rules[old_index]); ++i) {
      switch (rules[old_index][i]) {
         case 'a': rules[*index][i] = 'c'; break;
         case 'b': rules[*index][i] = 'g'; break;
         case 'c': rules[*index][i] = 'k'; break;
         case 'd': rules[*index][i] = 'h'; break;
         case 'e': rules[*index][i] = 'd'; break;
         case 'f': rules[*index][i] = 'b'; break;
         case 'g': rules[*index][i] = 'j'; break;
         case 'h': rules[*index][i] = 'l'; break;
         case 'i': rules[*index][i] = 'a'; break;
         case 'j': rules[*index][i] = 'f'; break;
         case 'k': rules[*index][i] = 'i'; break;
         case 'l': rules[*index][i] = 'e'; break;
         case ' ': rules[*index][i] = ' '; break;
         }
      }
   }
//
// rotate_y
//   rotate rule around y and add
//
void rotate_y(char rules[255][20], int *index) {
   int i,b[8];
   int old_index = *index;
   for (i = 0; i < 8; ++i) b[i] = (*index & (1 << i)) >> i;
   *index = 
      (b[1] << 0)
     +(b[5] << 1)
     +(b[3] << 2)
     +(b[7] << 3)
     +(b[0] << 4)
     +(b[4] << 5)
     +(b[2] << 6)
     +(b[6] << 7);
   for (i = 0; i < strlen(rules[old_index]); ++i) {
      switch (rules[old_index][i]) {
         case 'a': rules[*index][i] = 'e'; break;
         case 'b': rules[*index][i] = 'd'; break;
         case 'c': rules[*index][i] = 'h'; break;
         case 'd': rules[*index][i] = 'l'; break;
         case 'e': rules[*index][i] = 'i'; break;
         case 'f': rules[*index][i] = 'a'; break;
         case 'g': rules[*index][i] = 'c'; break;
         case 'h': rules[*index][i] = 'k'; break;
         case 'i': rules[*index][i] = 'f'; break;
         case 'j': rules[*index][i] = 'b'; break;
         case 'k': rules[*index][i] = 'g'; break;
         case 'l': rules[*index][i] = 'j'; break;
         case ' ': rules[*index][i] = ' '; break;
         }
      }
   }
//
// rotate_z
//   rotate rule around z and add
//
void rotate_z(char rules[255][20], int *index) {
   int i,b[8];
   int old_index = *index;
   for (i = 0; i < 8; ++i) b[i] = (*index & (1 << i)) >> i;
   *index = 
      (b[2] << 0)
     +(b[0] << 1)
     +(b[3] << 2)
     +(b[1] << 3)
     +(b[6] << 4)
     +(b[4] << 5)
     +(b[7] << 6)
     +(b[5] << 7);
   for (i = 0; i < strlen(rules[old_index]); ++i) {
      switch (rules[old_index][i]) {
         case 'a': rules[*index][i] = 'b'; break;
         case 'b': rules[*index][i] = 'c'; break;
         case 'c': rules[*index][i] = 'd'; break;
         case 'd': rules[*index][i] = 'a'; break;
         case 'e': rules[*index][i] = 'f'; break;
         case 'f': rules[*index][i] = 'g'; break;
         case 'g': rules[*index][i] = 'h'; break;
         case 'h': rules[*index][i] = 'e'; break;
         case 'i': rules[*index][i] = 'j'; break;
         case 'j': rules[*index][i] = 'k'; break;
         case 'k': rules[*index][i] = 'l'; break;
         case 'l': rules[*index][i] = 'i'; break;
         case ' ': rules[*index][i] = ' '; break;
         }
      }
   }
//
// print_rules
//    print the rule table
//
void print_rules(char rules[255][20]) {
   int i,b;
   printf("76543210\n");
   for (i = 0; i < 256; ++i) {
      for (b = 7; b >= 0; --b) {
         printf("%d",(i & (1 << b)) >> b);
         }
      printf(" %d %s\n",i,rules[i]);
      }
   }
// add_rules
//    add a rule and its variants to the table
//
void add_rules(char rules[255][20], int index, char *edges) {
   int i,j,k,l;
   strcpy(rules[index],edges);
   for (i=0; i<4; ++i) {
      for (j=0; j<4; ++j) {
         for (k=0; k<4; ++k) {
            rotate_x(rules,&index);
            }
         rotate_y(rules,&index);
         }
      rotate_z(rules,&index);
      }
   }
//
// init_rules
//    create the rule table
//
void init_rules(char rules[255][20]) {
   int i,j;
   for (i = 0; i < 256; ++i)
      for (j = 0; j < 20; ++j)
         rules[i][j] = 0;
   add_rules(rules,0b00000000,""); // 0
   add_rules(rules,0b11111111,""); // ~0
   add_rules(rules,0b00000001,"eda"); // 1
   add_rules(rules,0b11111110,"ade"); // ~1
   add_rules(rules,0b00000011,"fed dbf"); // 2
   add_rules(rules,0b11111100,"def fbd"); // ~2
   add_rules(rules,0b00100001,"eda jif"); // 3
   add_rules(rules,0b11011110,"ade fij"); // ~3
   add_rules(rules,0b10000001,"eda gkj"); // 4
   add_rules(rules,0b01111110,"ade jkg"); // ~4
   add_rules(rules,0b00001110,"fhg fdh fad"); // 5
   add_rules(rules,0b11110001,"ghf hdf daf"); // ~5
   add_rules(rules,0b10000011,"fed fdb gkj"); // 6
   add_rules(rules,0b01111100,"def bdf jkg"); // ~6
   add_rules(rules,0b10010010,"bfa ile gkj"); // 7
   add_rules(rules,0b01101101,"afb eli jkg"); // ~7
   add_rules(rules,0b00001111,"ehg feg"); // 8
   add_rules(rules,0b11110000,"ghe gef"); // ~8
   add_rules(rules,0b01001101,"elk eka akg agb"); // 9
   add_rules(rules,0b10110010,"kle ake gka bga"); // ~9
   add_rules(rules,0b10011001,"ild ida ckj cjb"); // 10
   add_rules(rules,0b01100110,"dli adi jkc bjc"); // ~10
   add_rules(rules,0b10001101,"hkj hja hae ajb"); // 11
   add_rules(rules,0b01110010,"jkh ajh eah bja"); // ~11
   add_rules(rules,0b00011110,"ile hgf hfd dfa"); // 12
   add_rules(rules,0b11100001,"eli fgh dfh afd"); // ~12
   add_rules(rules,0b01101001,"eda bcg lkh jif"); // 13
   add_rules(rules,0b10010110,"ade gcb hkl fij"); // ~13
   add_rules(rules,0b01001110,"lkg lga lad agf"); // 14
   add_rules(rules,0b10110001,"gkl agl dal fga"); // ~14
   }
//
// vertex
//    add a triangle vertex
//
void vertex(char c, int x, int y, int z, float voxel_size, float t, uint32_t *w, float *array) {
   switch(c) {
      case 'a':
         array[0] = x+(w[0]-t)/(w[0]-((float) w[1]));
         array[1] = y;
         array[2] = z;
         break;
      case 'b':
         array[0] = x+1;
         array[1] = y+(w[1]-t)/(w[1]-((float) w[3]));
         array[2] = z;
         break;
      case 'c':
         array[0] = x+(w[2]-t)/(w[2]-((float) w[3]));
         array[1] = y+1;
         array[2] = z;
         break;
      case 'd':
         array[0] = x;
         array[1] = y+(w[0]-t)/(w[0]-((float) w[2]));
         array[2] = z;
         break;
      case 'e':
         array[0] = x;
         array[1] = y;
         array[2] = z+(w[0]-t)/(w[0]-((float) w[4]));
         break;
      case 'f':
         array[0] = x+1;
         array[1] = y;
         array[2] = z+(w[1]-t)/(w[1]-((float) w[5]));
         break;
      case 'g':
         array[0] = x+1;
         array[1] = y+1;
         array[2] = z+(w[3]-t)/(w[3]-((float) w[7]));
         break;
      case 'h':
         array[0] = x;
         array[1] = y+1;
         array[2] = z+(w[2]-t)/(w[2]-((float) w[6]));
         break;
      case 'i':
         array[0] = x+(w[4]-t)/(w[4]-((float) w[5]));
         array[1] = y;
         array[2] = z+1;
         break;
      case 'j':
         array[0] = x+1;
         array[1] = y+(w[5]-t)/(w[5]-((float) w[7]));
         array[2] = z+1;
         break;
      case 'k':
         array[0] = x+(w[6]-t)/(w[6]-((float) w[7]));
         array[1] = y+1;
         array[2] = z+1;
         break;
      case 'l':
         array[0] = x;
         array[1] = y+(w[4]-t)/(w[4]-((float) w[6]));
         array[2] = z+1;
         break;
      }
      array[0] = voxel_size*array[0];
      array[1] = voxel_size*array[1];
      array[2] = voxel_size*array[2];
   }
//
// triangulate
//    triangulate voxel at lattice site x,y,z with vertex weights w
//
void triangulate(int x, int y, int z, float voxel_size, float t, uint32_t *w, char rules[255][20], struct fab_vars *v) {
   int i,index;
   char c;
   //
   // set index code
   //
   index = ((w[0] >= t) ? 0 : 1)
         + ((w[1] >= t) ? 0 : 2)
         + ((w[2] >= t) ? 0 : 4)
         + ((w[3] >= t) ? 0 : 8)
         + ((w[4] >= t) ? 0 : 16)
         + ((w[5] >= t) ? 0 : 32)
         + ((w[6] >= t) ? 0 : 64)
         + ((w[7] >= t) ? 0 : 128);
   //
   // add triangles for rule
   //
   i = 0;
   while (1) {
      //
      // loop over rule chars
      //
      c = rules[index][i];
      if (c == 0)
         //
         // end of rule
         //
         break;
      else if (c == ' ') {
         //
         // space between rules
         //
         i += 1;
         continue;
         }
      else {
         //
         // create triangle for rule
         //
         v->mesh->last = malloc(sizeof(struct fab_mesh_triangle_type));
         v->mesh->last->previous = v->mesh->triangle;
         v->mesh->triangle->next = v->mesh->last;
         v->mesh->triangle = v->mesh->last;
         v->mesh->triangle->next = 0;
         v->mesh->triangle->attribute = 0;
         v->mesh->triangle->normal[0] = v->mesh->triangle->normal[1] = v->mesh->triangle->normal[2] = 0;
         //
         // add vertices for rule
         //
         c = rules[index][i];
         vertex(c,x,y,z,voxel_size,t,w,v->mesh->triangle->v0);
         i += 1;
         c = rules[index][i];
         vertex(c,x,y,z,voxel_size,t,w,v->mesh->triangle->v1);
         i += 1;
         c = rules[index][i];
         vertex(c,x,y,z,voxel_size,t,w,v->mesh->triangle->v2);
         i += 1;
         }
      }
   }

void fab_write_stl(struct fab_vars *v, char *output_file_name) {
   //
   // write mesh as STL
   //
   FILE *output_file;
   char buf[80];
   uint32_t count;
   //
   // open output file
   //
   output_file = fopen(output_file_name, "wb");
   if (output_file == 0) {
      printf("fab.c: oops -- can't open %s\n",output_file_name);
      exit(-1);
      }
   //
   // write header
   //
   fwrite(buf,80,1,output_file);
   //
   // write count
   //
   count = 0;
   v->mesh->triangle = v->mesh->first;
   while (v->mesh->triangle->next != 0) {
      v->mesh->triangle = v->mesh->triangle->next;
      count += 1;
      }
   fwrite(&count,4,1,output_file);
   //
   // write triangles
   //
   v->mesh->triangle = v->mesh->first;
   while (v->mesh->triangle->next != 0) {
      v->mesh->triangle = v->mesh->triangle->next;
      fwrite(v->mesh->triangle->normal,4,3,output_file);
      fwrite(v->mesh->triangle->v0,4,3,output_file);
      fwrite(v->mesh->triangle->v1,4,3,output_file);
      fwrite(v->mesh->triangle->v2,4,3,output_file);
      fwrite(&(v->mesh->triangle->attribute),2,1,output_file);
      }
   //
   // return
   //
   printf("wrote %d triangles to %s\n",count,output_file_name);
   fclose(output_file);
   }

float interp(int x,int y,int i,int j,int k,uint32_t **lower_array,uint32_t **upper_array,int p) {
   //
   // trilinear interpolation within a voxel
   //
   return (lower_array[y][x]*((p+1.0-i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((p+1.0-k)/(p+1.0))
  + lower_array[y][x+1]*((i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((p+1.0-k)/(p+1.0))
  + lower_array[y+1][x]*((p+1.0-i)/(p+1.0))*((j)/(p+1.0))*((p+1.0-k)/(p+1.0))
  + lower_array[y+1][x+1]*((i)/(p+1.0))*((j)/(p+1.0))*((p+1.0-k)/(p+1.0))
  + upper_array[y][x]*((p+1.0-i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((k)/(p+1.0))
  + upper_array[y][x+1]*((i)/(p+1.0))*((p+1.0-j)/(p+1.0))*((k)/(p+1.0))
  + upper_array[y+1][x]*((p+1.0-i)/(p+1.0))*((j)/(p+1.0))*((k)/(p+1.0))
  + upper_array[y+1][x+1]*((i)/(p+1.0))*((j)/(p+1.0))*((k)/(p+1.0)));
  }

int main(int argc, char **argv) {
   //
   // local vars
   //
   GifFileType *GIFfile;
   GifRecordType GIFtype;
   GifByteType *GIFextension;
   GifPixelType *GIFline;
   uint32_t w[8],**lower_array,**upper_array;
   int x,y,z,i,j,k,n,p,imin,imax;
   int image_width,image_height,image_count,color_resolution,GIFcode,ret;
   float threshold,voxel_size;
   char comment[256],rules[255][20];
   struct fab_vars v;
   init_vars(&v);
   //
   // command line args
   //
   if (!((argc == 3) || (argc == 4) || (argc == 5) || (argc == 6))) {
      printf("command line: gif_stl in.gif out.stl [threshold [size [points [angle]]]]\n");
      printf("   in.gif = input GIF section file\n");
      printf("   out.stl = output STL file\n");
      printf("   threshold: surface threshold (0-255, default 128))\n");
      printf("   size = voxel size (mm, default from file))\n");
      printf("   points = points to interpolate per point (default 0)\n");
      printf("   to be implemented: angle = minimum relative face angle to decimate vertices (default 0)\n");
      exit(-1);
      }
   p = 0;
   threshold = 128;
   voxel_size = -1;
   image_width = -1;
   image_height = -1;
   image_count = -1;
   if (argc >= 4)
      sscanf(argv[3],"%f",&threshold);
   if (argc >= 5)
      sscanf(argv[4],"%f",&voxel_size);
   if (argc >= 6)
      sscanf(argv[5],"%d",&p);
   //
   // initialize the rule table
   //
   init_rules(rules);
   //
   // scan the file 
   //
   printf("read %s\n",argv[1]);
   color_resolution = -1;
   GIFfile = DGifOpenFileName(argv[1]);
   if (GIFfile == NULL) {
      printf("gif_stl: oops -- can not open %s\n",argv[1]);
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
                  printf("gif_stl: oops -- error reading line\n");
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
               if (voxel_size == -1)
                  sscanf(comment,"mm per pixel: %f;",&voxel_size);
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
            printf("gif_stl: oops -- undefined GIF record type\n");
            exit(-1);
            break;
         }
      } while (GIFtype != TERMINATE_RECORD_TYPE);
   if (GIFfile == NULL) {
      printf("gif_stl: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   if (voxel_size == -1) {
      voxel_size = 1.0;
      printf("   no pixel size found, assuming 1 mm\n");
      }
   printf("   voxel size (mm): %f, color resolution (bits): %d\n",voxel_size,color_resolution);
   printf("   number of images: %d, image width %d, image height %d\n",image_count,image_width,image_height);
   //
   // add empty border
   //
   image_width += 2;
   image_height += 2;
   image_count += 2;
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
   //
   // read the file
   //
   DGifCloseFile(GIFfile);
   GIFfile = DGifOpenFileName(argv[1]);
   if (GIFfile == NULL) {
      printf("gif_stl: oops -- can not open %s\n",argv[1]);
      exit(-1);
      }
   z = 0;
   v.mesh = malloc(sizeof(struct fab_mesh_type));
   v.mesh->triangle = malloc(sizeof(struct fab_mesh_triangle_type));
   v.mesh->first = v.mesh->triangle;
   v.mesh->last = v.mesh->triangle;
   v.mesh->triangle->previous = v.mesh->triangle->next = 0;
   imin = 10000;
   imax = 0;
   do {
      DGifGetRecordType(GIFfile,&GIFtype);
      switch (GIFtype) {
         case IMAGE_DESC_RECORD_TYPE:
            //
            // read image
            //
            DGifGetImageDesc(GIFfile);
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b   layer = %d",z);
            //
            // read layer
            //
            for (y = 0; y < (image_height-2); ++y) {
               ret = DGifGetLine(GIFfile,GIFline,GIFfile->SWidth);
               if (ret != GIF_OK) {
                  printf("gif_stl: oops -- error reading line\n");
                  exit(-1);
                  }
               for (x = 0; x < (image_width-2); ++x) {
                  lower_array[y+1][x+1] = upper_array[y+1][x+1];
                  upper_array[y+1][x+1] = GIFline[x];
                  if (upper_array[y+1][x+1] < imin) imin = upper_array[y+1][x+1];
                  if (upper_array[y+1][x+1] > imax) imax = upper_array[y+1][x+1];
                  }
               }
            if (p == 0) {
               //
               // no interpolation, loop over layer voxels
               //
               for (x = 0; x < (image_width-1); ++x) {
                  for (y = 0; y < (image_height-1); ++y) {
                     w[0] = lower_array[y][x];
                     w[1] = lower_array[y][x+1];
                     w[2] = lower_array[y+1][x];
                     w[3] = lower_array[y+1][x+1];
                     w[4] = upper_array[y][x];
                     w[5] = upper_array[y][x+1];
                     w[6] = upper_array[y+1][x];
                     w[7] = upper_array[y+1][x+1];
                     triangulate(x,y,z,voxel_size,threshold,w,rules,&v);
                     }
                  }
               }
            else {
               //
               // yes interpolation, loop over layer sub-voxels
               //
               for (x = 0; x < (image_width-1); ++x) {
                  for (y = 0; y < (image_height-1); ++y) {
                     for (i = 0; i <= p; ++i) {
                        for (j = 0; j <= p; ++j) {
                           for (k = 0; k <= p; ++k) {
                              w[0] = interp(x,y,i,j,k,lower_array,upper_array,p);
                              w[1] = interp(x,y,i+1,j,k,lower_array,upper_array,p);
                              w[2] = interp(x,y,i,j+1,k,lower_array,upper_array,p);
                              w[3] = interp(x,y,i+1,j+1,k,lower_array,upper_array,p);
                              w[4] = interp(x,y,i,j,k+1,lower_array,upper_array,p);
                              w[5] = interp(x,y,i+1,j,k+1,lower_array,upper_array,p);
                              w[6] = interp(x,y,i,j+1,k+1,lower_array,upper_array,p);
                              w[7] = interp(x,y,i+1,j+1,k+1,lower_array,upper_array,p);
                              triangulate((1+p)*x+i,(1+p)*y+j,(1+p)*z+k,voxel_size,threshold,w,rules,&v);
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
            printf("gif_stl: oops -- undefined GIF record type\n");
            exit(-1);
            break;
         }
      } while (GIFtype != TERMINATE_RECORD_TYPE);
   //
   // add empty top layer
   //
   printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b   layer = %d",z);
   for (y = 0; y < (image_height-2); ++y) {
      for (x = 0; x < (image_width-2); ++x) {
         lower_array[y+1][x+1] = upper_array[y+1][x+1];
         upper_array[y+1][x+1] = 0;
         }
      }
   if (p == 0) {
      //
      // no interpolation, loop over layer voxels
      //
      for (x = 0; x < (image_width-1); ++x) {
         for (y = 0; y < (image_height-1); ++y) {
            w[0] = lower_array[y][x];
            w[1] = lower_array[y][x+1];
            w[2] = lower_array[y+1][x];
            w[3] = lower_array[y+1][x+1];
            w[4] = upper_array[y][x];
            w[5] = upper_array[y][x+1];
            w[6] = upper_array[y+1][x];
            w[7] = upper_array[y+1][x+1];
            triangulate(x,y,z,voxel_size,threshold,w,rules,&v);
            }
         }
      }
   else {
      //
      // yes interpolation, loop over layer sub-voxels
      //
      for (x = 0; x < (image_width-1); ++x) {
         for (y = 0; y < (image_height-1); ++y) {
            for (i = 0; i <= p; ++i) {
               for (j = 0; j <= p; ++j) {
                  for (k = 0; k <= p; ++k) {
                     w[0] = interp(x,y,i,j,k,lower_array,upper_array,p);
                     w[1] = interp(x,y,i+1,j,k,lower_array,upper_array,p);
                     w[2] = interp(x,y,i,j+1,k,lower_array,upper_array,p);
                     w[3] = interp(x,y,i+1,j+1,k,lower_array,upper_array,p);
                     w[4] = interp(x,y,i,j,k+1,lower_array,upper_array,p);
                     w[5] = interp(x,y,i+1,j,k+1,lower_array,upper_array,p);
                     w[6] = interp(x,y,i,j+1,k+1,lower_array,upper_array,p);
                     w[7] = interp(x,y,i+1,j+1,k+1,lower_array,upper_array,p);
                     triangulate((1+p)*x+i,(1+p)*y+j,(1+p)*z+k,voxel_size,threshold,w,rules,&v);
                     }
                  }
               }
            }
         }
      }
   printf("\n");
   printf("   min: %d max: %d\n",imin,imax);
   //
   // write STL
   //
   fab_write_stl(&v,argv[2]);
   //
   // exit
   //
   DGifCloseFile(GIFfile);
   exit(0);
   }
