//
// fab.c
//    fab modules routines
//
// Neil Gershenfeld 9/8/13
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

//
// initialization
//

void init_vars(struct fab_vars *v) {
   //
   // fab_vars initialization
   //
   v->empty = 0;
   v->interior = 1;
   v->edge = (1 << 1);
   v->north = (1 << 2);
   v->west = (2 << 2);
   v->east = (3 << 2);
   v->south = (4 << 2);
   v->stop = (5 << 2);
   v->corner = (6 << 2);
   v->corner2 = (7 << 2);
   v->direction = (7 << 2);
   v->g = 0;
   v->h = 0;
   v->dx = 0;
   v->dy = 0;
   v->dz = 0;
   v->xmin = 0;
   v->ymin = 0;
   v->zmin = 0;
   v->distances = 0;
   v->starts= 0;
   v->minimums= 0;
   }

void fab_limits(struct fab_vars *v, float *vertex) {
	//
	// update limits for vertex
	//
	int i;
	for (i = 0; i < 3; ++i) {
		if (vertex[i] > v->mesh->max[i])
			v->mesh->max[i] = vertex[i];
		if (vertex[i] < v->mesh->min[i])
			v->mesh->min[i] = vertex[i];
	}
}

//
// input
//

void fab_read_stl(struct fab_vars *v, char *input_file_name) {
   //
   // read STL into triangle mesh
   //
	FILE *input_file;
   char buf[80],*ptr;
   uint32_t i,size,ret;
   //
   // read file
   //
   input_file = fopen(input_file_name, "rb");
   if (input_file == 0) {
      printf("fab.c: oops -- can't open %s\n",input_file_name);
      exit(-1);
      }
   ret = fread(buf,80,1,input_file);
   ptr = strstr(buf,"facet");
   if (ptr != NULL) {
      printf("fab.c: oops -- must be binary STL file\n");
      exit(-1);
      }
   ret = fread(&size,4,1,input_file);
	v->mesh = malloc(sizeof(struct fab_mesh_type));
	v->mesh->triangle = malloc(sizeof(struct fab_mesh_triangle_type));
   v->mesh->first = v->mesh->triangle;
   v->mesh->last = v->mesh->triangle;
   v->mesh->triangle->previous = v->mesh->triangle->next = 0;
   ret = fread(&(v->mesh->triangle->normal),4,3,input_file);
   ret = fread(&(v->mesh->triangle->v0),4,3,input_file);
   ret = fread(&(v->mesh->triangle->v1),4,3,input_file);
   ret = fread(&(v->mesh->triangle->v2),4,3,input_file);
   ret = fread(&(v->mesh->triangle->attribute),2,1,input_file);
   for (i = 1; i < size; ++i) {
	   v->mesh->last = malloc(sizeof(struct fab_mesh_triangle_type));
      v->mesh->last->previous = v->mesh->triangle;
      v->mesh->triangle->next = v->mesh->last;
      v->mesh->triangle = v->mesh->last;
      v->mesh->triangle->previous = v->mesh->triangle->next = 0;
      ret = fread(&(v->mesh->triangle->normal),4,3,input_file);
      ret = fread(&(v->mesh->triangle->v0),4,3,input_file);
      ret = fread(&(v->mesh->triangle->v1),4,3,input_file);
      ret = fread(&(v->mesh->triangle->v2),4,3,input_file);
      ret = fread(&(v->mesh->triangle->attribute),2,1,input_file);
      }
   fclose(input_file);
   //
   // check read
   //
   if (ret == 0) {
      printf("fab.c: oops -- file read failed\n");
      exit(-1);
      }
   v->mesh->min[0] = fab_big;
   v->mesh->min[1] = fab_big;
   v->mesh->min[2] = fab_big;
   v->mesh->max[0] = -fab_big;
   v->mesh->max[1] = -fab_big;
   v->mesh->max[2] = -fab_big;
   v->mesh->triangle = v->mesh->first;
   while (v->mesh->triangle != 0) {
      fab_limits(v, v->mesh->triangle->v0);
      fab_limits(v, v->mesh->triangle->v1);
      fab_limits(v, v->mesh->triangle->v2);
      v->mesh->triangle = v->mesh->triangle->next;
      }
   //
   // report and return
   //
   v->mesh->number = size;
   printf("read %s\n",input_file_name);
   printf("   number of triangles: %d\n",v->mesh->number);
   printf("   xmin, xmax: %f %f\n",v->mesh->min[0],v->mesh->max[0]);
   printf("   ymin, ymax: %f %f\n",v->mesh->min[1],v->mesh->max[1]);
   printf("   zmin, zmax: %f %f\n",v->mesh->min[2],v->mesh->max[2]);
   }

void fab_read_png(struct fab_vars *v, char *input_file_name) {
   //
   // read PNG into fab_vars
   //
	FILE *input_file;
   int y;
   png_uint_32 res_x,res_y;
   int unit_type;
   //
   // read PNG file
   //
   input_file = fopen(input_file_name, "rb");
   if (input_file == 0) {
      printf("fab.c: oops -- can't open %s\n",input_file_name);
      exit(-1);
      }
   v->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                       NULL, NULL, NULL);
   v->info_ptr = png_create_info_struct(v->png_ptr);
   png_init_io(v->png_ptr, input_file);
   png_read_info(v->png_ptr, v->info_ptr);
   //
   // get size
   //
   v->nx = png_get_image_width(v->png_ptr, v->info_ptr);
   v->ny = png_get_image_height(v->png_ptr, v->info_ptr);
   v->bit_depth = png_get_bit_depth(v->png_ptr, v->info_ptr);
   //
   // get units
   //
   png_get_pHYs(v->png_ptr, v->info_ptr, &res_x, &res_y, &unit_type);
   if (unit_type == PNG_RESOLUTION_METER) {
      v->dx = 1000 * v->nx / ((float) res_x);
      v->dy = 1000 * v->ny / ((float) res_y);
   }
   else {
      printf("fab.c: don't recognize PNG units, assuming 72/inch\n");
      res_x = (72*1000.0)/(25.4);
      res_y = (72*1000.0)/(25.4);
      v->dx = 1000 * v->nx / ((float) res_x);
      v->dy = 1000 * v->ny / ((float) res_y);
      }
   //
   // get texts
   //
   png_textp text_ptr;
   int num_text;
   png_get_text(v->png_ptr, v->info_ptr, &text_ptr, &num_text);
   int t;
   float xmin=0,ymin=0,zmin=0,zmax=0;
   for (t = 0; t < num_text; ++t) {
      if (0 == strcmp(text_ptr[t].key, "zmax"))
         sscanf(text_ptr[t].text, "%g", &zmax);
      else if (0 == strcmp(text_ptr[t].key, "zmin"))
         sscanf(text_ptr[t].text, "%g", &zmin);
      else if (0 == strcmp(text_ptr[t].key, "ymin"))
         sscanf(text_ptr[t].text, "%g", &ymin);
      else if (0 == strcmp(text_ptr[t].key, "xmin"))
         sscanf(text_ptr[t].text, "%g", &xmin);
      }
   //
   // set pixels
   //
	v->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * v->ny);
	for (y = 0; y < v->ny; ++y)
		v->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(v->png_ptr,
		                                                         v->info_ptr));
	png_read_image(v->png_ptr, v->row_pointers);
   //
   // set variables
   //
   v->nz = 1;
   if (zmax != zmin)
      v->dz = zmax - zmin;
   else
      v->dz = 0;
   v->xmin = xmin;
   v->ymin = ymin;
   v->zmin = zmin;
   //
   // allocate array
   //
   v->array = malloc(v->ny*sizeof(uint32_t *));
   for (y = 0; y < v->ny; ++y)
      v->array[y] = malloc(v->nx*sizeof(uint32_t));
   //
   // copy image to array
   //
   fab_png_array(v);
   //
   // close and return
   //
   fclose(input_file);
   printf("read %s\n",input_file_name);
   printf("   bit depth: %d\n",v->bit_depth);
   printf("   x pixels: %d, y pixels: %d\n",v->nx,v->ny);
   printf("   x pixels/m: %d, y pixels/m: %d\n",(int) res_x,(int) res_y);
   if (v->dz != 0) {
      printf("   dx: %f mm, dy: %f mm, dz: %f mm\n",v->dx,v->dy,v->dz);
      }
   else {
      printf("   dx: %f mm, dy: %f mm\n",v->dx,v->dy);
      }
   }

void fab_read_array(struct fab_vars *v, char *input_file_name) {
   //
   // read array
   //
   FILE *input_file;
   int x,y,ret;
   input_file = fopen(input_file_name,"rb");
   //
   // read vars
   //
   ret = fread(&v->nx,4,1,input_file);
   ret = fread(&v->ny,4,1,input_file);
   ret = fread(&v->nz,4,1,input_file);
   ret = fread(&v->dx,4,1,input_file);
   ret = fread(&v->dy,4,1,input_file);
   ret = fread(&v->dz,4,1,input_file);
   ret = fread(&v->xmin,4,1,input_file);
   ret = fread(&v->ymin,4,1,input_file);
   ret = fread(&v->zmin,4,1,input_file);
   //
   // allocate array
   //
   v->array = malloc(v->ny*sizeof(uint32_t *));
   for (y = 0; y < v->ny; ++y)
      v->array[y] = malloc(v->nx*sizeof(uint32_t));
   //
   // read array
   //
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x)
         ret = fread(&v->array[y][x],1,1,input_file);
   //
   // check read
   //
   if (ret == 0) {
      printf("fab.c: oops -- file read failed\n");
      exit(-1);
      }
   //
   // print, close, and return
   //
   printf("read %s\n",input_file_name);
   printf("   nx: %d, ny: %d, nz: %d\n",v->nx,v->ny,v->nz);
   printf("   dx: %f, dy: %f, dz: %f\n",v->dx,v->dy,v->dz);
   printf("   xmin: %f, ymin: %f, zmin: %f\n",v->xmin,v->ymin,v->zmin);
   fclose(input_file);
   }

void fab_read_path(struct fab_vars *v, char *input_file_name) {
   //
   // read path from file
   //
	FILE *input_file;
   int x,y,z,dof,nsegs=0,npts=0;
   char line[255];
   input_file = fopen(input_file_name,"r");
   if (input_file == 0) {
      printf("fab.c: oops -- can't open %s\n",input_file_name);
      exit(-1);
      }
   printf("read %s\n",input_file_name);
   //
   // scan file
   //
   while (fgets(line,sizeof line,input_file) != NULL) {
      if (0 == strncmp(line,"dof:",4)) {
         sscanf(line,"dof: %d",&dof);
         printf("   degrees of freedom: %d\n",dof);
         fab_path_start(v,dof);
         }
      else if (0 == strncmp(line,"units:",6)) {
         printf("   %s",line); // todo: parse and use units
         }
      else if (0 == strncmp(line,"nx ny:",6)) {
         sscanf(line,"nx ny: %d %d",&v->nx,&v->ny);
         printf("   nx: %d, ny: %d\n",v->nx,v->ny);
         }
      else if (0 == strncmp(line,"nx ny nz:",9)) {
         sscanf(line,"nx ny nz: %d %d %d",&v->nx,&v->ny,&v->nz);
         printf("   nx: %d, ny: %d, nz: %d\n",v->nx,v->ny,v->nz);
         }
      else if (0 == strncmp(line,"dx dy:",6)) {
         sscanf(line,"dx dy: %lf %lf",&v->dx,&v->dy);
         printf("   dx: %f, dy: %f\n",v->dx,v->dy);
         }
      else if (0 == strncmp(line,"dx dy dz:",9)) {
         sscanf(line,"dx dy dz: %lf %lf %lf",&v->dx,&v->dy,&v->dz);
         printf("   dx: %f, dy: %f, dz: %f\n",v->dx,v->dy,v->dz);
         }
      else if (0 == strncmp(line,"xmin ymin:",10)) {
         sscanf(line,"xmin ymin: %lf %lf",&v->xmin,&v->ymin);
         printf("   xmin: %f, ymin: %f\n",v->xmin,v->ymin);
         }
      else if (0 == strncmp(line,"xmin ymin zmin:",15)) {
         sscanf(line,"xmin ymin zmin: %lf %lf %lf",&v->xmin,&v->ymin,&v->zmin);
         printf("   xmin: %f, ymin: %f, zmin: %f\n",v->xmin,v->ymin,v->zmin);
         }
      else if (0 == strncmp(line,"path start:",11)) {
         printf("   path start:\n");
         }
      else if (0 == strncmp(line,"segment start:",14)) {
         fab_path_segment(v);
         nsegs += 1;
         while (fgets(line,sizeof line,input_file) != NULL) {
            if (0 == strncmp(line,"segment end:",12)) {
               break;
               }
            else {
               fab_path_point(v);
               npts += 1;
               if (dof == 3) {
                  sscanf(line,"%d %d %d",&x,&y,&z);
                  fab_path_axis(v,x);
                  fab_path_axis(v,y);
                  fab_path_axis(v,z);
                  }
               else if (dof == 2) {
                  sscanf(line,"%d %d",&x,&y);
                  fab_path_axis(v,x);
                  fab_path_axis(v,y);
                  }
               else {
                  printf("fab.c: oops -- can't handle %d dof\n",dof);
                  exit(-1);
                  }
               }
            }
         }
      else if (0 == strncmp(line,"path end:",9)) {
         printf("   segments: %d, points: %d\n",nsegs,npts);
         break;
         }
      else {
         printf("fab.c: oops -- can't parse %s\n",line);
         exit(-1);
         }
      }
   fclose(input_file);
   //
   // create array
   //
   v->array = malloc(v->ny*sizeof(uint32_t *));
   for (y = 0; y < v->ny; ++y)
      v->array[y] = malloc(v->nx*sizeof(uint32_t));
   //
   // return
   //
   return;
   }

//
// output
//

void fab_write_png_K(struct fab_vars *v, char *output_file_name) {
   //
   // write grayscale PNG from fab_vars
   //
	FILE *output_file;
   int x,y;
   int num_text=0;
   char xmins[100],ymins[100],zmins[100],zmaxs[100];
   png_text text_ptr[4];
   png_uint_32 res_x,res_y;
   png_byte color_type;
   png_byte bit_depth;
   png_byte *ptr;
   //
   // open PNG file
   //
	output_file = fopen(output_file_name, "wb");
	v->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);
	v->info_ptr = png_create_info_struct(v->png_ptr);
	png_init_io(v->png_ptr, output_file);
   //
   // set vars
   //
   bit_depth = v->bit_depth;
   color_type = PNG_COLOR_TYPE_GRAY; 
	png_set_IHDR(v->png_ptr, v->info_ptr, v->nx, v->ny,
	   bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   res_x = 1000 * v->nx / v->dx;
   res_y = 1000 * v->ny / v->dy;
   png_set_pHYs(v->png_ptr, v->info_ptr, res_x, res_y, PNG_RESOLUTION_METER);
   text_ptr[0].key = "xmin";
   sprintf(xmins,"%f",v->xmin);
   text_ptr[0].text = xmins;
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   num_text += 1;
   text_ptr[1].key = "ymin";
   sprintf(ymins,"%f",v->ymin);
   text_ptr[1].text = ymins;
   text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
   num_text += 1;
   if (v->dz != 0) {
      text_ptr[2].key = "zmin";
      sprintf(zmins,"%f",v->zmin);
      text_ptr[2].text = zmins;
      text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
      num_text += 1;
      text_ptr[3].key = "zmax";
      sprintf(zmaxs,"%f",(v->zmin + v->dz));
      text_ptr[3].text = zmaxs;
      text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
      num_text += 1;
      }
   png_set_text(v->png_ptr, v->info_ptr, text_ptr, num_text);
   png_write_info(v->png_ptr, v->info_ptr);
   //
   // allocate pixels
   //
	v->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * v->ny);
	for (y = 0; y < v->ny; ++y)
		v->row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(v->png_ptr,
		                                                         v->info_ptr));
   //
   // set pixels
   //
   if (bit_depth == 8) {
      for (y = 0; y < v->ny; ++y)
         for (x = 0; x < v->nx; ++x) {
      		ptr = &(v->row_pointers[y][x]);
      	   ptr[0] = v->array[y][x];
      	   }
         }
   else if (bit_depth == 16) {
      for (y = 0; y < v->ny; ++y)
         for (x = 0; x < v->nx; ++x) {
      		ptr = &(v->row_pointers[y][x*2]);
      	   ptr[0] = (v->array[y][x] >> 8) & 255;
      	   ptr[1] = v->array[y][x] & 255;
      	   }
         }
   else {
      printf("fab.c: oops -- don't recognize bit depth %d\n",v->bit_depth);
      exit(-1);
      }
   //
   // write, close, and return
   //
	png_write_image(v->png_ptr, v->row_pointers);
	png_write_end(v->png_ptr, NULL);
   fclose(output_file);
   printf("write %s\n",output_file_name);
   printf("   x pixels: %d, y pixels: %d\n",v->nx,v->ny);
   printf("   x pixels/m: %d, y pixels/m: %d\n",(int) res_x,(int) res_y);
   if (v->dz != 0) {
      printf("   dx: %f mm, dy: %f mm, dz: %f mm\n",v->dx,v->dy,v->dz);
      }
   else {
      printf("   dx: %f mm, dy: %f mm\n",v->dx,v->dy);
      }
   }

void fab_write_array(struct fab_vars *v, char *output_file_name) {
   //
   // write array
   //
   FILE *output_file;
   int x,y;
   output_file = fopen(output_file_name,"wb");
   fwrite(&v->nx,4,1,output_file);
   fwrite(&v->ny,4,1,output_file);
   fwrite(&v->nz,4,1,output_file);
   fwrite(&v->dx,4,1,output_file);
   fwrite(&v->dy,4,1,output_file);
   fwrite(&v->dz,4,1,output_file);
   fwrite(&v->xmin,4,1,output_file);
   fwrite(&v->ymin,4,1,output_file);
   fwrite(&v->zmin,4,1,output_file);
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x)
         fwrite(&v->array[y][x],1,1,output_file);
   fclose(output_file);
   printf("write %s\n",output_file_name);
   printf("   nx: %d, ny: %d, nz: %d\n",v->nx,v->ny,v->nz);
   printf("   dx: %f, dy: %f, dz: %f\n",v->dx,v->dy,v->dz);
   printf("   xmin: %f, ymin: %f, zmin: %f\n",v->xmin,v->ymin,v->zmin);
   }

void fab_write_path(struct fab_vars *v, char *output_file_name) {
   //
   // write path to file
   //
	FILE *output_file;
   int i,nsegs=0,npts=0;
   output_file = fopen(output_file_name,"w");
   v->path->segment = v->path->first;
   fprintf(output_file,"dof: %d\n",v->path->dof);
   fprintf(output_file,"units: ");
   for (i = 0; i < v->path->dof; ++i)
      fprintf(output_file,"mm ");
   fprintf(output_file,"\n");
   if (v->path->dof == 2) {
      fprintf(output_file,"nx ny: %d %d\n",v->nx,v->ny);
      fprintf(output_file,"dx dy: %f %f\n",v->dx,v->dy);
      fprintf(output_file,"xmin ymin: %f %f\n",v->xmin,v->ymin);
      }
   else if (v->path->dof > 2) {
      fprintf(output_file,"nx ny nz: %d %d %d\n",v->nx,v->ny,v->nz);
      fprintf(output_file,"dx dy dz: %f %f %f\n",v->dx,v->dy,v->dz);
      fprintf(output_file,"xmin ymin zmin: %f %f %f\n",v->xmin,v->ymin,v->zmin);
      }
   fprintf(output_file,"path start:\n");
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      fprintf(output_file,"segment start:\n");
      nsegs += 1;
      while (1) {
         //
         // follow points
         //
         v->path->segment->point->axis = v->path->segment->point->first;
         while (1) {
            //
            // follow axes
            //
            fprintf(output_file,"%d ",v->path->segment->point->axis->value);
            if (v->path->segment->point->axis->next == 0)
               break;
            v->path->segment->point->axis = v->path->segment->point->axis->next;
            }
         fprintf(output_file,"\n");
         npts += 1;
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         }
      fprintf(output_file,"segment end:\n");
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   fprintf(output_file,"path end:\n");
   fclose(output_file);
   printf("write %s\n",output_file_name);
   printf("   degrees of freedom: %d\n",v->path->dof);
   printf("   units: ");
   for (i = 0; i < v->path->dof; ++i) {
      printf("mm ");
      }
   printf("\n");
   printf("   segments: %d, points: %d\n",nsegs,npts);
   if (v->path->dof == 2) {
      printf("   nx: %d, ny: %d\n",v->nx,v->ny);
      printf("   dx: %f, dy: %f\n",v->dx,v->dy);
      printf("   xmin: %f, ymin: %f\n",v->xmin,v->ymin);
      }
   else if (v->path->dof > 2) {
      printf("   nx: %d, ny: %d, nz: %d\n",v->nx,v->ny,v->nz);
      printf("   dx: %f, dy: %f, dz: %f\n",v->dx,v->dy,v->dz);
      printf("   xmin: %f, ymin: %f, zmin: %f\n",v->xmin,v->ymin,v->zmin);
      }
   }

//
// shading
//

void fab_shade_states(struct fab_vars *v) {
   //
   // shade array states
   //
   int x,y;
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x) {
         if ((v->array[y][x] & v->direction) == v->north)
            v->array[y][x] = 200;
         else if ((v->array[y][x] & v->direction) == v->east)
            v->array[y][x] = 150;
         else if ((v->array[y][x] & v->direction) == v->south)
            v->array[y][x] = 100;
         else if ((v->array[y][x] & v->direction) == v->west)
            v->array[y][x] = 50;
         else if ((v->array[y][x] & v->direction) == v->stop)
            v->array[y][x] = 25;
         else if (v->array[y][x] == v->empty)
            v->array[y][x] = 0;
         else if (v->array[y][x] & v->edge)
            v->array[y][x] = 128;
         else if (v->array[y][x] & v->interior)
            v->array[y][x] = 255;
         }
   }

void fab_shade_line(struct fab_vars *v, int x0, int y0, int x1, int y1, int intensity) {
   //
   // shade line into array
   //
   int x,y,dx,dy,step;
   float slope;
   dx = x1 - x0;
   dy = y1 - y0;
   x = x0;
   y = y0;
   if ((dx == 0) && (dy == 0)) {
      if ((x0 > 0) && (x0 < v->nx) && (y0 > 0) && (y0 < v->ny))
         v->array[y0][x0] = intensity;
      }
   else if (abs(dx) == 0) {
      if (dy > 0)
         step = 1;
      else
         step = -1;
      do {
         if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
            v->array[y][x] = intensity;
         y += step;
         } while (y != y1);
      if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
         v->array[y][x] = intensity;
      }
   else if (abs(dy) == 0) {
      if (dx > 0)
         step = 1;
      else
         step = -1;
      do {
         if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
            v->array[y][x] = intensity;
         x += step;
         } while (x != x1);
      if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
         v->array[y][x] = intensity;
      }
   else {
      slope = ((float) dy)/((float) dx);
      if (dx > 0)
         step = 1;
      else
         step = -1;
      do {
         y = y0 + slope*(x-x0);
         if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
            v->array[y][x] = intensity;
         x += step;
         } while (x != x1);
      y = y0 + slope*(x-x0);
      if ((x > 0) && (x < v->nx) && (y > 0) && (y < v->ny))
         v->array[y][x] = intensity;
      }
   }

void fab_shade_path(struct fab_vars *v) {
   //
   // shade path into array
   //
   int x,y,x0,y0,x1,y1,intensity;
   //
   // set up and clear array
   //
   v->bit_depth = 8;
   intensity = pow(2,8) - 1;
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x)
         v->array[y][x] = 0;
   v->path->segment = v->path->first;
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      x0 = v->path->segment->point->first->value;
      y0 = v->path->segment->point->first->next->value;
      while (1) {
         //
         // follow points
         //
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         x1 = v->path->segment->point->first->value;
         y1 = v->path->segment->point->first->next->value;
         fab_shade_line(v,x0,y0,x1,y1,intensity);
         x0 = x1;
         y0 = y1;
         }
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   }

void fab_shade_path_displace(struct fab_vars *v) {
   //
   // shade path into array with z displacement
   //
   int x,y,x0,y0,z0,x1,y1,z1,nz,intensity;
   float scale;
   //
   // check for thickness
   //
   if (v->dz == 0) {
      //
      // no, draw 2D
      //
      fab_shade_path(v);
      return;
      }
   //
   // yes, enlarge and clear array for displacements
   //
   nz = v->nx * v->dz / v->dx;
   scale = ((float) nz) / v->nz;
   v->nx += nz;
   v->ny += nz;
   v->dx += v->dz;
   v->dy += v->dz;
   v->bit_depth = 8;
   v->array = malloc(v->ny*sizeof(uint32_t *));
   for (y = 0; y < v->ny; ++y)
      v->array[y] = malloc(v->nx*sizeof(uint32_t));
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x)
         v->array[y][x] = 0;
   //
   // follow path
   //
   v->path->segment = v->path->first;
   while (1) {
      //
      // follow segments
      //
      v->path->segment->point = v->path->segment->first;
      z0 = nz - scale * v->path->segment->point->first->next->next->value;
      y0 = z0 + v->path->segment->point->first->next->value;
      x0 = z0 + v->path->segment->point->first->value;
      while (1) {
         //
         // follow points
         //
         if (v->path->segment->point->next == 0)
            break;
         v->path->segment->point = v->path->segment->point->next;
         z1 = nz - scale * v->path->segment->point->first->next->next->value;
         y1 = z1 + v->path->segment->point->first->next->value;
         x1 = z1 + v->path->segment->point->first->value;
         intensity = (z0 + z1)/2;
         fab_shade_line(v,x0,y0,x1,y1,intensity);
         x0 = x1;
         y0 = y1;
         z0 = z1;
         }
      if (v->path->segment->next == 0)
         break;
      v->path->segment = v->path->segment->next;
      }
   }

int fab_row(struct fab_vars *v, float units, int Y, float value) {
    //
    // return array row
    //
    value = 0.5f + (v->ny - 1) * units * (value - v->mesh->min[Y]) / v->dy;
    return (value);
}
int fab_col(struct fab_vars *v, float units, int X, float value) {
    //
    // return array col
    //
    value = 0.5f + (v->nx - 1) * units * (value - v->mesh->min[X]) / v->dx;
    return value;
}

int fab_height(struct fab_vars *v, int S, int Z, float value) {
    //
    // return array height
    //
    value = ((1-S)/2) * 65535 + S * 65535 * (value - v->mesh->min[Z]) / (v->mesh->max[Z] - v->mesh->min[Z]);
    return value;
}

void fab_shade_triangle(struct fab_vars *v, float units, int S, int X, int Y, int Z) {
    //
    // shade triangle intro array
    //
    float slope;
    int xmin,ymin,zmin,x1,y1,z1,x2,y2,z2;
    int x20,z20,x21,z21,x10,z10,z;
    int i,j;
    int dir;
    int temp;
    #define fab_swap(a,b) temp=a; a=b; b=temp;
    //
    // find coords
    //
    xmin = fab_col(v, units, X, v->mesh->triangle->v0[X]);
    ymin = fab_row(v, units, Y, v->mesh->triangle->v0[Y]);
    zmin = fab_height(v, S, Z, v->mesh->triangle->v0[Z]);
    x1 = fab_col(v, units, X, v->mesh->triangle->v1[X]);
    y1 = fab_row(v, units, Y, v->mesh->triangle->v1[Y]);
    z1 = fab_height(v, S, Z, v->mesh->triangle->v1[Z]);
    x2 = fab_col(v, units, X, v->mesh->triangle->v2[X]);
    y2 = fab_row(v, units, Y, v->mesh->triangle->v2[Y]);
    z2 = fab_height(v, S, Z, v->mesh->triangle->v2[Z]);
    //
    // check normal if needs to be drawn
    //
    if ((S*((x1-xmin)*(y1-y2)-(x1-x2)*(y1-ymin))) >= 0)
        return;
    //
    // sort heights
    //
    if (y1 > y2) {
        fab_swap(x1,x2);
        fab_swap(y1,y2);
        fab_swap(z1,z2);
    }
    if (ymin > y1) {
        fab_swap(xmin,x1);
        fab_swap(ymin,y1);
        fab_swap(zmin,z1);
    }
    if (y1 > y2) {
        fab_swap(x1,x2);
        fab_swap(y1,y2);
        fab_swap(z1,z2);
    }
    //
    // check orientation
    //
    if (x1 < (xmin+((x2-xmin)*(y1-ymin))/(y2-ymin)))
        dir = 1;
    else
        dir = -1;
    //
    // draw
    //
    if (y2 != y1) {
        for (i = y2; i >= y1; --i) {
            x21 = x1 + ((x2-x1)*(i-y1))/(y2-y1);
            z21 = z1 + ((z2-z1)*(i-y1))/(y2-y1);
            x20 = xmin + ((x2-xmin)*(i-ymin))/(y2-ymin);
            z20 = zmin + ((z2-zmin)*(i-ymin))/(y2-ymin);
            if (x21 != x20)
                slope = (z20-z21)/((float) (x20-x21));
            else
                slope = 0;
            j = x21 - dir;
            while (j != x20) {
                j += dir;
                z = z21 + slope*(j-x21);
                if (z > v->array[v->ny-1-i][j])
                    v->array[v->ny-1-i][j] = z;
            }
        }
    }
    if (y1 != ymin) {
        for (i = y1; i >= ymin; --i) {
            x10 = xmin + ((x1-xmin)*(i-ymin))/(y1-ymin);
            z10 = zmin + ((z1-zmin)*(i-ymin))/(y1-ymin);
            x20 = xmin + ((x2-xmin)*(i-ymin))/(y2-ymin);
            z20 = zmin + ((z2-zmin)*(i-ymin))/(y2-ymin);
            if (x10 != x20)
                slope = (z20-z10)/((float) (x20-x10));
            else
                slope = 0;
            j = x10 - dir;
            while (j != x20) {
                j += dir;
                z = z10 + slope*(j-x10);
                if (z > v->array[v->ny-1-i][j])
                    v->array[v->ny-1-i][j] = z;
            }
        }
    }
}

void fab_shade_mesh(struct fab_vars *v, float units, float resolution, char axis) {
    //
    // shade triangle mesh into 16-bit array
    //
    int X,Y,Z,S;
    int i; 
    //
    // allocate array
    //
    if (axis == 'x') {
      X = 1; Y = 2; Z = 0; S = 1;
      }
    else if (axis == 'X') {
      X = 1; Y = 2; Z = 0; S = -1;
      }
    if (axis == 'y') {
      X = 2; Y = 0; Z = 1; S = 1;
      }
    else if (axis == 'Y') {
      X = 2; Y = 0; Z = 1; S = -1;
      }
    if (axis == 'z') {
      X = 0; Y = 1; Z = 2; S = 1;
      }
    else if (axis == 'Z') {
      X = 0; Y = 1; Z = 2; S = -1;
      }
    v->dx = units * (v->mesh->max[X]-v->mesh->min[X]);
    v->dy = units * (v->mesh->max[Y]-v->mesh->min[Y]);
    v->dz = units * (v->mesh->max[Z]-v->mesh->min[Z]);
    v->nx = resolution * v->dx;
    v->ny = resolution * v->dy;
    v->nz = 1;
    v->xmin = units * v->mesh->min[X];
    v->ymin = units * v->mesh->min[Y];
    v->zmin = units * v->mesh->min[Z];
    v->bit_depth = 16;
    //
    // alloc and init
    //
    v->array = malloc(v->ny*sizeof(uint32_t *));
    for (i = 0; i < v->ny; ++i) {
      v->array[i] = malloc(v->nx*sizeof(uint32_t));
      memset(v->array[i],'\0',v->nx*sizeof(uint32_t));
      }
    //
    // draw triangles into array
    //
    v->mesh->triangle = v->mesh->first;
    while (v->mesh->triangle != 0) {
      fab_shade_triangle(v, units, S, X, Y, Z);
      v->mesh->triangle = v->mesh->triangle->next;
      }
   }

//
// array operations
//

void fab_png_array(struct fab_vars *v) {
   //
   // copy PNG image to array
   //
   int intensity,value,x,y;
   int bit, byte;
   png_byte *ptr;
   png_colorp palette;
   png_color color;
   //png_uint_32 num_palette;
   int num_palette;
   
   if (png_get_bit_depth(v->png_ptr, v->info_ptr) == 1) {
      if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_PALETTE)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
               byte = x / 8;
               bit = 7 - x % 8;
               ptr = &(v->row_pointers[y][byte]);
               value = (ptr[0] >> bit) & 1;
               png_get_PLTE(v->png_ptr, v->info_ptr, &palette, &num_palette);
               color = palette[value];
               intensity = (color.red + color.green + color.blue)/3.0;
         	   v->array[y][x] = intensity;
         	   }
      else if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_GRAY)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
               byte = x / 8;
               bit = 7 - x % 8;
         		ptr = &(v->row_pointers[y][byte]);
               value = (ptr[0] >> bit) & 1;
         	   v->array[y][x] = value;
         	   }
      else {
         printf("fab.c: oops 3 -- image type not supported\n");
         exit(-1);
         }
      }
   else if (png_get_bit_depth(v->png_ptr, v->info_ptr) == 8) {
      if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_GRAY)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		ptr = &(v->row_pointers[y][x*1]);
         	   v->array[y][x] = ptr[0];
         	   }
      else if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		ptr = &(v->row_pointers[y][x*2]);
         	   v->array[y][x] = (int) ((1.0-ptr[1]/255.0)*255 + (0.0+(ptr[1]/255.0))*ptr[0]);
         	   }
      else if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_RGB)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		ptr = &(v->row_pointers[y][x*3]);
         	   v->array[y][x] = (int) ((ptr[0] + ptr[1] + ptr[2])/3.0);
         	   }
      else if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		ptr = &(v->row_pointers[y][x*4]);
         	   v->array[y][x] = (int) ((1.0-ptr[3]/255.0)*255 + (0.0+(ptr[3]/255.0))*((ptr[0] + ptr[1] + ptr[2])/3.0));
         	   }
      else if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_PALETTE) {
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		value = v->row_pointers[y][x];
               png_get_PLTE(v->png_ptr, v->info_ptr, &palette, &num_palette);
               color = palette[value];
               intensity = (color.red + color.green + color.blue)/3.0;
         	   v->array[y][x] = intensity;
         	   }
         }
      else {
         printf("fab.c: oops -- image type not supported\n");
         exit(-1);
         }
      }
   else if (png_get_bit_depth(v->png_ptr, v->info_ptr) == 16) {
      if (png_get_color_type(v->png_ptr, v->info_ptr) == PNG_COLOR_TYPE_GRAY)
         for (y = 0; y < v->ny; ++y)
            for (x = 0; x < v->nx; ++x) {
         		ptr = &(v->row_pointers[y][x*2]);
         	   v->array[y][x] = (ptr[0] << 8) + ptr[1];
         	   }
      else {
         printf("fab.c: oops -- image type not supported\n");
         exit(-1);
         }
      }
   else {
      printf("fab.c: oops -- image type not supported\n");
      exit(-1);
      }
   }

void fab_rescale(struct fab_vars *v, float min, float max) {
   //
   // rescale array intensity
   //
   int x,y,top;
   float low,high;
   //
   // find limits
   //
   top = pow(2,v->bit_depth) - 1;
   low = top;
   high = 0;
   for (y = 0; y < v->ny; ++y) {
      for (x = 0; x < v->nx; ++x) {
         if (v->array[y][x] > high)
            high = v->array[y][x];
         if (v->array[y][x] < low)
            low = v->array[y][x];
         }
      }
   //
   // rescale
   //
   for (y = 0; y < v->ny; ++y) {
      for (x = 0; x < v->nx; ++x) {
         v->array[y][x] = top*min + top*(max-min) * (v->array[y][x] - low) / (high - low);
         }
      }
   }

void add_edge(struct fab_vars *v, int* count, int y, int x) {
	//
	// set cell to interior edge
	//
	v->array[y][x] = v->interior | v->edge;
	++(*count);
}

int fab_edges(struct fab_vars *v) {
   //
   // find edges of thresholded array
   //
   int x,y,sum,count;

   count = 0;
   for (x = 1; x < (v->nx-1); ++x)
      if (v->array[0][x] & v->interior) {
         sum = (v->array[0][x-1] & v->interior)
            + (v->array[0][x+1] & v->interior)
            + (v->array[1][x] & v->interior);
         if (sum < 3)
            add_edge(v, &count, 0,x);
         }
      else if (v->array[v->ny-1][x] & v->interior) {
         sum = (v->array[v->ny-1][x-1] & v->interior)
            + (v->array[v->ny-1][x+1] & v->interior)
            + (v->array[v->ny-2][x] & v->interior);
         if (sum < 3)
            add_edge(v, &count, v->ny-1,x);
         }
   for (y = 1; y < (v->ny-1); ++y) {
      if (v->array[y][0] & v->interior) {
         sum = (v->array[y-1][0] & v->interior)
            + (v->array[y+1][0] & v->interior)
            + (v->array[y][1] & v->interior);
         if (sum < 3)
            add_edge(v, &count, y,0);
         }
      if (v->array[y][v->nx-1] & v->interior) {
         sum = (v->array[y-1][v->nx-1] & v->interior)
            + (v->array[y+1][v->nx-1] & v->interior)
            + (v->array[y][v->nx-2] & v->interior);
         if (sum < 3)
            add_edge(v, &count, y,v->nx-1);
         }
      for (x = 1; x < (v->nx-1); ++x) {
         if (v->array[y][x] & v->interior) {
            sum = (v->array[y-1][x] & v->interior)
               + (v->array[y+1][x] & v->interior)
               + (v->array[y][x-1] & v->interior)
               + (v->array[y][x+1] & v->interior);
            if (sum < 4)
               add_edge(v, &count, y,x);
            }
         }
      }
   return count;
   }

void fab_threshold(struct fab_vars *v, float intensity) {
   //
   // threshold array
   //
   int x,y;
   float threshold;
   threshold = (pow(2,v->bit_depth) - 1.0) * intensity;
   for (y = 0; y < v->ny; ++y)
      for (x = 0; x < v->nx; ++x)
         if (v->array[y][x] >= threshold)
            v->array[y][x] = v->interior;
         else
            v->array[y][x] = v->empty;
   }

int fab_distances_distance(struct fab_vars *v, int x, int y, int i) {
   return ((y-i)*(y-i) + (v->g[i][x])*(v->g[i][x]));
   }

int fab_distances_intersection(struct fab_vars *v, int x, int y0, int y1) {
   return ((0.0+(v->g[y0][x])*(v->g[y0][x])-(v->g[y1][x])*(v->g[y1][x])+y0*y0-y1*y1)/(2.0*(y0-y1)));
   }

void fab_distances(struct fab_vars *v) {
   //
   // find Euclidean distance to interior in a thresholded array
   //
   int x,y,closest,segment,newstart;
   double d;
   //
   // allocate arrays if needed
   //
   if (v->g == 0) {
      v->g = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->g[y] = malloc(v->nx*sizeof(uint32_t));
      }
   if (v->h == 0) {
      v->h = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->h[y] = malloc(v->nx*sizeof(uint32_t));
      }
   if (v->distances == 0) {
      v->distances = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->distances[y] = malloc(v->nx*sizeof(uint32_t));
      }
   if (v->starts == 0)
      v->starts = malloc(v->ny*sizeof(uint32_t *));
   if (v->minimums == 0)
      v->minimums = malloc(v->ny*sizeof(uint32_t *));
   //
   // column scan
   //  
   for (y = 0; y < v->ny; ++y) {
      //
      // right pass
      //
      closest = -(v->nx);
      for (x = 0; x < v->nx; ++x) {
         if (v->array[y][x] == v->interior) {
            v->g[y][x] = 0;
            closest = x;
            }
         else
            v->g[y][x] = (x-closest);
         }
      //
      // left pass
      //
      closest = 2*(v->nx);
      for (x = (v->nx - 1); x >= 0; --x) {
         if (v->array[y][x] == v->interior)
            closest = x;
         else {
            d = (closest-x);
            if (d < v->g[y][x])
               v->g[y][x] = d;
            }
         }
      }
   //
   // row scan
   //
   for (x = 0; x < v->nx; ++x) {
      printf("\b\b\b\b\b\b\b\b   %d",x);
      segment = 0;
      v->starts[0] = 0;
      v->minimums[0] = 0;
      //
      // down 
      //
      for (y = 1; y < v->ny; ++y) {
         while ((segment >= 0) &&
               (fab_distances_distance(v, x, v->starts[segment],v->minimums[segment])
               > fab_distances_distance(v, x, v->starts[segment],y)))
            segment -= 1;
         if (segment < 0) {
            segment = 0;
            v->minimums[0] = y;
            }
         else {
            newstart = 1 + fab_distances_intersection(v, x, v->minimums[segment],y);
            if (newstart < v->ny) {
               segment += 1;
               v->minimums[segment] = y;
               v->starts[segment] = newstart;
               }
            }
         }
      //
      // up 
      //
      for (y = (v->ny - 1); y >= 0; --y) {
         v->distances[y][x] = sqrt(fab_distances_distance(v, x, y, v->minimums[segment]));
         if (y == v->starts[segment])
            segment -= 1;
         }
      }
   printf("\n");
   }

void test_fab_distances(struct fab_vars *v) {
   //
   // find Euclidean distance to interior in a thresholded array
   //
   int x,y;
   //
   // allocate arrays if needed
   //
   if (v->ddx == 0) {
      v->ddx = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->ddx[y] = malloc(v->nx*sizeof(uint32_t));
      }
   if (v->ddy == 0) {
      v->ddy = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->ddy[y] = malloc(v->nx*sizeof(uint32_t));
      }
   if (v->distances == 0) {
      v->distances = malloc(v->ny*sizeof(uint32_t *));
      for (y = 0; y < v->ny; ++y)
         v->distances[y] = malloc(v->nx*sizeof(uint32_t));
      }
   //
   // column scan
   //  
   for (y = 0; y < v->ny; ++y) {
      printf("\b\b\b\b\b\b\b\b\b\b   y %d",y);
      //
      // right pass
      //
      v->ddx[y][0] = -1;
      v->ddy[y][0] = 0;
      for (x = 1; x < v->nx; ++x) {
         v->ddy[y][x] = 0;
         if (v->array[y][x] == v->interior)
            v->ddx[y][x] = 0;
         else
            v->ddx[y][x] = v->ddx[y][x-1] - 1;
         }
      //
      // left pass
      //
      v->ddx[y][v->nx-1] = 1;
      for (x = (v->nx - 2); x >= 0; --x) {
         if (abs(v->ddx[y][x]) > (abs(v->ddx[y][x+1])) + 1)
            v->ddx[y][x] = v->ddx[y][x+1] + 1;
         //v->distances[y][x] = sqrt(v->ddx[y][x]*v->ddx[y][x]+v->ddy[y][x]*v->ddy[y][x]);
         }
      }
   printf("\n");
   //
   // row scan
   //
   for (x = 0; x < v->nx; ++x) {
      printf("\b\b\b\b\b\b\b\b\b\b   x %d",x);
      //
      // down 
      //
      v->ddx[0][x] = 0;
      v->ddy[0][x] = -1;
      for (y = 1; y < v->ny; ++y) {
         if ((v->ddx[y][x]*v->ddx[y][x]) > (v->ddx[y-1][x]*v->ddx[y-1][x] + (v->ddy[y-1][x]-1)*(v->ddy[y-1][x]-1))) {
            v->ddx[y][x] = v->ddx[y-1][x];
            v->ddy[y][x] = v->ddy[y-1][x]-1;
            }
         v->distances[y][x] = sqrt(v->ddx[y][x]*v->ddx[y][x]+v->ddy[y][x]*v->ddy[y][x]);
         }
      //
      // up 
      //
      /*
      v->ddx[v->ny-1][x] = 0;
      v->ddy[v->ny-1][x] = 1;
      for (y = (v->ny - 2); y >= 0; --y) {
         if ((v->ddx[y][x]*v->ddx[y][x]+v->ddy[y][x]*v->ddy[y][x]) > (v->ddx[y+1][x]*v->ddx[y+1][x] + (abs(v->ddy[y+1][x])+1)*(abs(v->ddy[y+1][x])+1))) {
            v->ddx[y][x] = v->ddx[y+1][x];
            v->ddy[y][x] = v->ddy[y+1][x]+1;
            }
         //v->distances[y][x] = sqrt(v->ddx[y][x]*v->ddx[y][x]+v->ddy[y][x]*v->ddy[y][x]);
         }
      */
      }
   printf("\n");
   }

int fab_offset(struct fab_vars *v, float distance) {
   //
   // use distances to offset array
   //    real units, assumes square pixels
   //    returns remaining points
   //
   int x,y,remaining_count;
   float image_distance;
   image_distance = distance * v->nx/v->dx;
   //
   // loop over array
   //
   remaining_count = 0;
   for (y = 0; y < v->ny; ++y) {
      for (x = 0; x < v->nx; ++x) {
         if (v->distances[y][x] <= image_distance)
            v->array[y][x] = v->interior;
         else {
            v->array[y][x] = v->empty;
            remaining_count += 1;
            }
         }
      }
   return remaining_count;
   }


int state(struct fab_vars *v, int y, int x) {
    int nw, nn, ne, ww, cc, ee, sw, ss, se, state;
    //
    // assemble state string
    //   0 = empty
    //   1 = interior
    //   2 = boundary
    //
    if ((y == 0) | (x == 0))
        nw = 2;
    else
        nw = v->array[y-1][x-1] & 1; 
    if (y == 0)
        nn = 2;
    else
        nn = v->array[y-1][x] & 1;
    if ((y == 0) | (x == (v->nx-1)))
        ne = 2;
    else
        ne = v->array[y-1][x+1] & 1;
    if (x == 0)
        ww = 2;
    else
        ww = v->array[y][x-1] & 1;
    cc = v->array[y][x] & 1;
    if (x == (v->nx-1))
        ee = 2;
    else
        ee = v->array[y][x+1] & 1;
    if ((y == (v->ny-1)) | (x == 0))
        sw = 2;
    else
        sw = v->array[y+1][x-1] & 1;
    if (y == (v->ny-1))
        ss = 2;
    else
        ss = v->array[y+1][x] & 1;
    if ((y == (v->ny-1)) | (x == (v->nx-1)))
        se = 2;
    else
        se = v->array[y+1][x+1] & 1;
    state =
        (nw <<  0) + (nn <<  2) + (ne <<  4) +
        (ww <<  6) + (cc <<  8) + (ee << 10) +
        (sw << 12) + (ss << 14) + (se << 16);
    return state;
}

void add_rule(struct fab_vars *v, int *table, int nw, int nn, int ne,
    int ww, int cc, int ee,int sw,int ss, int se, int rule) {
        //
        // add a CA rule, with rotations
        //
        int state;
        //
        // add the rule
        //
        state =
            (nw <<  0) + (nn <<  2) + (ne <<  4) +
            (ww <<  6) + (cc <<  8) + (ee << 10) +
            (sw << 12) + (ss << 14) + (se << 16);
        table[state] = rule;
        //
        // rotate 90 degrees
        //
        state =
            (sw <<  0) + (ww <<  2) + (nw <<  4) +
            (ss <<  6) + (cc <<  8) + (nn << 10) +
            (se << 12) + (ee << 14) + (ne << 16);
        if (rule == v->east)
            table[state] = v->south;
        else if (rule == v->south)
            table[state] = v->west;
        else if (rule == v->west)
            table[state] = v->north;
        else if (rule == v->north)
            table[state] = v->east;
        else if (rule == v->corner)
            table[state] = v->corner;
        else if (rule == v->stop)
            table[state] = v->stop;
        //
        // rotate 180 degrees
        // 
        state =
            (se <<  0) + (ss <<  2) + (sw <<  4) +
            (ee <<  6) + (cc <<  8) + (ww << 10) +
            (ne << 12) + (nn << 14) + (nw << 16);
        if (rule == v->east)
            table[state] = v->west;
        else if (rule == v->south)
            table[state] = v->north;
        else if (rule == v->west)
            table[state] = v->east;
        else if (rule == v->north)
            table[state] = v->south;
        else if (rule == v->corner)
            table[state] = v->corner;
        else if (rule == v->stop)
            table[state] = v->stop;
        //
        // rotate 270 degrees
        //
        state =
            (ne <<  0) + (ee <<  2) + (se <<  4) +
            (nn <<  6) + (cc <<  8) + (ss << 10) +
            (nw << 12) + (ww << 14) + (sw << 16);
        if (rule == v->east)
            table[state] = v->north;
        else if (rule == v->south)
            table[state] = v->east;
        else if (rule == v->west)
            table[state] = v->south;
        else if (rule == v->north)
            table[state] = v->west;
        else if (rule == v->corner)
            table[state] = v->corner;
        else if (rule == v->stop)
            table[state] = v->stop;
}

void fab_directions(struct fab_vars *v) {
   //
   // find directions around interior edges (right-hand rule)
   //
   int x,y,rule,dir,*table;
   table = calloc((pow(2,18)), sizeof(int));

   // 
   // build rule table
   //
   // 0 = empty
   // 1 = interior
   // 2 = boundary 
   //
   // 1 0:
   //
   // 011
   // 111
   // 111
   add_rule(v, table,0,1,1,1,1,1,1,1,1,v->north);
   // 101
   // 111
   // 111
   add_rule(v, table,1,0,1,1,1,1,1,1,1,v->east);
   //
   // 2 0's:
   //
   // 001
   // 111
   // 111
   add_rule(v, table,0,0,1,1,1,1,1,1,1,v->east);
   // 100
   // 111
   // 111
   add_rule(v, table,1,0,0,1,1,1,1,1,1,v->east);
   // 010
   // 111
   // 111
   add_rule(v, table,0,1,0,1,1,1,1,1,1,v->east);
   // 011
   // 110
   // 111
   add_rule(v, table,0,1,1,1,1,0,1,1,1,v->south);
   // 110
   // 011
   // 111
   add_rule(v, table,1,1,0,0,1,1,1,1,1,v->east);
   // 101
   // 011
   // 111
   add_rule(v, table,1,0,1,0,1,1,1,1,1,v->east);
   // 101
   // 110
   // 111
   add_rule(v, table,1,0,1,1,1,0,1,1,1,v->south);
   // 011
   // 111
   // 110
   add_rule(v, table,0,1,1,1,1,1,1,1,0,v->corner);
   // 011
   // 111
   // 101
   add_rule(v, table,0,1,1,1,1,1,1,0,1,v->north);
   // 110
   // 111
   // 101
   add_rule(v, table,1,1,0,1,1,1,1,0,1,v->west);
   // 101
   // 111
   // 110
   add_rule(v, table,1,0,1,1,1,1,1,1,0,v->south);
   // 101
   // 111
   // 011
   add_rule(v, table,1,0,1,1,1,1,0,1,1,v->east);
   //
   // 3 0's:
   //
   // 001
   // 011
   // 111
   add_rule(v, table,0,0,1,0,1,1,1,1,1,v->east);
   // 010
   // 011
   // 111
   add_rule(v, table,0,1,0,0,1,1,1,1,1,v->east);
   // 010
   // 110
   // 111
   add_rule(v, table,0,1,0,1,1,0,1,1,1,v->south);
   // 010
   // 111
   // 011
   add_rule(v, table,0,1,0,1,1,1,0,1,1,v->east);
   // 010
   // 111
   // 110
   add_rule(v, table,0,1,0,1,1,1,1,1,0,v->south);
   // 110
   // 011
   // 011
   add_rule(v, table,1,1,0,0,1,1,0,1,1,v->east);
   // 011
   // 110
   // 110
   add_rule(v, table,0,1,1,1,1,0,1,1,0,v->south);
   // 101
   // 011
   // 011
   add_rule(v, table,1,0,1,0,1,1,0,1,1,v->east);
   // 101
   // 110
   // 110
   add_rule(v, table,1,0,1,1,1,0,1,1,0,v->south);
   // 011
   // 011
   // 011;
   add_rule(v, table,0,1,1,0,1,1,0,1,1,v->north);
   //
   // 4 0's:
   //
   // 001
   // 011
   // 011
   add_rule(v, table,0,0,1,0,1,1,0,1,1,v->east);
   // 100
   // 110
   // 110
   add_rule(v, table,1,0,0,1,1,0,1,1,0,v->south);
   // 010
   // 011
   // 011
   add_rule(v, table,0,1,0,0,1,1,0,1,1,v->east);
   // 010
   // 110
   // 110
   add_rule(v, table,0,1,0,1,1,0,1,1,0,v->south);
   // 001
   // 110
   // 110
   add_rule(v, table,0,0,1,1,1,0,1,1,0,v->south);
   // 100
   // 011
   // 011
   add_rule(v, table,1,0,0,0,1,1,0,1,1,v->east);
   //
   // 5 0's:
   //
   // 000 
   // 011
   // 011
   add_rule(v, table,0,0,0,0,1,1,0,1,1,v->east);
   //
   // boundary states
   //
   // 200
   // 211
   // 211
   add_rule(v, table,2,0,0,2,1,1,2,1,1,v->east);
   // 201
   // 211
   // 211
   add_rule(v, table,2,0,1,2,1,1,2,1,1,v->east);
   // 210
   // 211
   // 211
   add_rule(v, table,2,1,0,2,1,1,2,1,1,v->east);
   // 002
   // 112
   // 112
   add_rule(v, table,0,0,2,1,1,2,1,1,2,v->stop);
   // 102
   // 112
   // 112
   add_rule(v, table,1,0,2,1,1,2,1,1,2,v->stop);
   // 002
   // 112
   // 102
   add_rule(v, table,0,0,2,1,1,2,1,0,2,v->stop);
   // 012
   // 112
   // 112
   add_rule(v, table,0,1,2,1,1,2,1,1,2,v->stop);
   // 012
   // 112
   // 102
   add_rule(v, table,0,1,2,1,1,2,1,0,2,v->stop);
   for (y = 0; y < v->ny; ++y) {
      for (x = 0; x < v->nx; ++x) {
         if (v->array[y][x] & v->interior) {
            v->array[y][x] = v->interior;
            rule = state(v,y,x);
            dir = table[rule];
            v->array[y][x] |= dir;
            }
         }
      }

   free(table); 
   }

//
// mesh operations
//

void fab_mesh_path(struct fab_vars *v, float units, float resolution) {
   //
   // convert mesh to path
   //

   //
   // set up path
   //
   fab_path_start(v,3);
   v->dx = units * (v->mesh->max[0]-v->mesh->min[0]);
   v->dy = units * (v->mesh->max[1]-v->mesh->min[1]);
   v->dz = units * (v->mesh->max[2]-v->mesh->min[2]);
   v->nx = resolution * v->dx;
   v->ny = resolution * v->dy;
   v->nz = resolution * v->dz;
   v->xmin = units * v->mesh->min[0];
   v->ymin = units * v->mesh->min[1];
   v->zmin = units * v->mesh->min[2];
   v->bit_depth = 16;
   //
   // loop over mesh
   //
   v->mesh->triangle = v->mesh->first;
   while (v->mesh->triangle != 0) {
      fab_path_segment(v);
      fab_path_point(v);
      fab_path_axis(v,(v->nx)*(v->mesh->triangle->v0[0] - v->mesh->min[0])/(v->dx));
      fab_path_axis(v,(v->ny)*(v->mesh->max[1] - v->mesh->triangle->v0[1])/(v->dy));
      fab_path_axis(v,(v->nz)*(v->mesh->triangle->v0[2] - v->mesh->min[2])/(v->dz));
      fab_path_point(v);
      fab_path_axis(v,(v->nx)*(v->mesh->triangle->v1[0] - v->mesh->min[0])/(v->dx));
      fab_path_axis(v,(v->ny)*(v->mesh->max[1] - v->mesh->triangle->v1[1])/(v->dy));
      fab_path_axis(v,(v->nz)*(v->mesh->triangle->v1[2] - v->mesh->min[2])/(v->dz));
      fab_path_point(v);
      fab_path_axis(v,(v->nx)*(v->mesh->triangle->v2[0] - v->mesh->min[0])/(v->dx));
      fab_path_axis(v,(v->ny)*(v->mesh->max[1] - v->mesh->triangle->v2[1])/(v->dy));
      fab_path_axis(v,(v->nz)*(v->mesh->triangle->v2[2] - v->mesh->min[2])/(v->dz));
      fab_path_point(v);
      fab_path_axis(v,(v->nx)*(v->mesh->triangle->v0[0] - v->mesh->min[0])/(v->dx));
      fab_path_axis(v,(v->ny)*(v->mesh->max[1] - v->mesh->triangle->v0[1])/(v->dy));
      fab_path_axis(v,(v->nz)*(v->mesh->triangle->v0[2] - v->mesh->min[2])/(v->dz));
      v->mesh->triangle = v->mesh->triangle->next;
      }
   }

//
// path operations
//

void fab_path_start(struct fab_vars *v, int dof) {
   //
   // start a new path
   //
   v->path = malloc(sizeof(struct fab_path_type));
   v->path->first = 0;
   v->path->dof = dof;
   }

void fab_path_segment(struct fab_vars *v) {
   //
   // add a segment to the current path
   //
	v->path->last = malloc(sizeof(struct fab_path_segment_type));
   v->path->last->previous = 0;
   v->path->last->next = 0;
   v->path->last->first = 0;
   if (v->path->first == 0) {
      //
      // first segment
      //
      v->path->first = v->path->last;
      v->path->segment = v->path->last;
      }
   else {
      //
      // not first segment
      //
      v->path->last->previous = v->path->segment;
      v->path->segment->next = v->path->last;
      v->path->segment = v->path->last;
      }
   }

void fab_path_point(struct fab_vars *v) {
   //
   // add a point to the current path segment
   //
   v->path->segment->last =
      malloc(sizeof(struct fab_path_point_type));
   v->path->segment->last->previous = 0;
   v->path->segment->last->next = 0;
   v->path->segment->last->first = 0;
   if (v->path->segment->first == 0) {
      //
      // first point
      //
      v->path->segment->first = v->path->segment->last;
      v->path->segment->point = v->path->segment->last;
      }
   else {
      //
      // not first point
      //
      v->path->segment->last->previous = v->path->segment->point;
      v->path->segment->point->next = v->path->segment->last;
      v->path->segment->point = v->path->segment->last;
      }
   }

void fab_path_axis(struct fab_vars *v, int value) {
   //
   // add an axis to the current path segment point
   //
   v->path->segment->point->last =
      malloc(sizeof(struct fab_path_axis_type));
   v->path->segment->point->last->previous = 0;
   v->path->segment->point->last->next = 0;
   v->path->segment->point->last->value = value;
   if (v->path->segment->point->first == 0) {
      //
      // first axis
      //
      v->path->segment->point->first = v->path->segment->point->last;
      v->path->segment->point->axis = v->path->segment->point->last;
      }
   else {
      //
      // not first axis
      //
      v->path->segment->point->last->previous = v->path->segment->point->axis;
      v->path->segment->point->axis->next = v->path->segment->point->last;
      v->path->segment->point->axis = v->path->segment->point->last;
      }
   }

void fab_path_join(struct fab_vars *vin1, struct fab_vars *vin2, struct fab_vars *vout, float dx, float dy) {
   //
   // join paths, checking limits
   //
   float xf,yf,zf;
   int xi,yi,zi;
   float xmin,ymin,zmin,xmax,ymax,zmax,dpmm;
   xmin = ((dx + vin1->xmin) < vin2->xmin) ? (dx + vin1->xmin) : vin2->xmin;
   ymin = ((-dy + vin1->ymin) < vin2->ymin) ? (-dy + vin1->ymin) : vin2->ymin;
   zmin = (vin1->zmin < vin2->zmin) ? vin1->zmin : vin2->zmin;
   xmax = ((dx + vin1->xmin + vin1->dx) > (vin2->xmin + vin2->dx)) ? (dx + vin1->xmin + vin1->dx) : (vin2->xmin + vin2->dx);
   ymax = ((-dy + vin1->ymin + vin1->dy) > (vin2->ymin + vin2->dy)) ? (-dy + vin1->ymin + vin1->dy) : (vin2->ymin + vin2->dy);
   zmax = ((vin1->zmin + vin1->dz) > (vin2->zmin + vin2->dz)) ? (vin1->zmin + vin1->dz) : (vin2->zmin + vin2->dz);
   dpmm = ((vin1->nx/vin1->dx) > (vin2->nx/vin2->dx)) ? (vin1->nx/vin1->dx) : (vin2->nx/vin2->dx);
   vout->xmin = xmin;
   vout->ymin = ymin;
   vout->zmin = zmin;
   vout->dx = (xmax-xmin);
   vout->dy = (ymax-ymin);
   vout->dz = (zmax-zmin);
   vout->nx = dpmm * vout->dx;
   vout->ny = dpmm * vout->dy;
   vout->nz = dpmm * vout->dz;
   fab_path_start(vout,vin1->path->dof);
   //
   // copy first path
   //
   vin1->path->segment = vin1->path->first;
   while (1) {
      //
      // follow segments
      //
      fab_path_segment(vout);
      vin1->path->segment->point = vin1->path->segment->first;
      while (1) {
         //
         // follow points
         //
         fab_path_point(vout);
         xf = dx + vin1->xmin + vin1->dx * vin1->path->segment->point->first->value / (vin1->nx - 1.0);
         xi =  (vout->nx - 1) * (xf - vout->xmin) / vout->dx;
         fab_path_axis(vout,xi);
         yf = -dy + vin1->ymin + vin1->dy * vin1->path->segment->point->first->next->value / (vin1->ny - 1.0);
         yi =  (vout->ny - 1) * (yf - vout->ymin) / vout->dy;
         fab_path_axis(vout,yi);
         if (vin1->path->dof == 3) {
            if (vin1->nz == 1)
               zi = 0;
            else {
               zf = vin1->zmin + vin1->dz * vin1->path->segment->point->first->next->next->value / (vin1->nz - 1.0);
               zi = (vout->nz - 1) * (zf - vout->zmin) / vout->dz;
               }
            fab_path_axis(vout,zi);
            }
         if (vin1->path->segment->point->next == 0)
            break;
         vin1->path->segment->point = vin1->path->segment->point->next;
         }
      if (vin1->path->segment->next == 0)
         break;
      vin1->path->segment = vin1->path->segment->next;
      }
   //
   // copy second path
   //
   vin2->path->segment = vin2->path->first;
   while (1) {
      //
      // follow segments
      //
      fab_path_segment(vout);
      vin2->path->segment->point = vin2->path->segment->first;
      while (1) {
         //
         // follow points
         //
         fab_path_point(vout);
         xf = vin2->xmin + vin2->dx * vin2->path->segment->point->first->value / (vin2->nx - 1.0);
         xi = (vout->nx - 1) * (xf - vout->xmin) / vout->dx;
         fab_path_axis(vout,xi);
         yf = vin2->ymin + vin2->dy * vin2->path->segment->point->first->next->value / (vin2->ny - 1.0);
         yi = (vout->ny - 1) * (yf - vout->ymin) / vout->dy;
         fab_path_axis(vout,yi);
         if (vin2->path->dof == 3) {
            if (vin2->nz == 1)
               zi = 0;
            else {
               zf = vin2->zmin + vin2->dz * vin2->path->segment->point->first->next->next->value / (vin2->nz - 1.0);
               zi = (vout->nz - 1) * (zf - vout->zmin) / vout->dz;
               }
            fab_path_axis(vout,zi);
            }
         if (vin2->path->segment->point->next == 0)
            break;
         vin2->path->segment->point = vin2->path->segment->point->next;
         }
      if (vin2->path->segment->next == 0)
         break;
      vin2->path->segment = vin2->path->segment->next;
      }
   }

void fab_path_cat(struct fab_vars *vin1, struct fab_vars *vin2, struct fab_vars *vout) {
   //
   // concatenate paths, copying limits
   //
   vout->xmin = vin1->xmin;
   vout->ymin = vin1->ymin;
   vout->zmin = vin1->zmin;
   vout->dx = vin1->dx;
   vout->dy = vin1->dy;
   vout->dz = vin1->dz;
   vout->nx = vin1->nx;
   vout->ny = vin1->ny;
   vout->nz = vin1->nz;
   fab_path_start(vout,vin1->path->dof);
   //
   // copy first path
   //
   vin1->path->segment = vin1->path->first;
   while (1) {
      //
      // follow segments
      //
      fab_path_segment(vout);
      vin1->path->segment->point = vin1->path->segment->first;
      while (1) {
         //
         // follow points
         //
         fab_path_point(vout);
         fab_path_axis(vout,vin1->path->segment->point->first->value);
         fab_path_axis(vout,vin1->path->segment->point->first->next->value);
         if (vin1->path->dof > 2)
            fab_path_axis(vout,vin1->path->segment->point->first->next->next->value);
         if (vin1->path->segment->point->next == 0)
            break;
         vin1->path->segment->point = vin1->path->segment->point->next;
         }
      if (vin1->path->segment->next == 0)
         break;
      vin1->path->segment = vin1->path->segment->next;
      }
   //
   // copy second path
   //
   vin2->path->segment = vin2->path->first;
   while (1) {
      //
      // follow segments
      //
      fab_path_segment(vout);
      vin2->path->segment->point = vin2->path->segment->first;
      while (1) {
         //
         // follow points
         //
         fab_path_point(vout);
         fab_path_axis(vout,vin2->path->segment->point->first->value);
         fab_path_axis(vout,vin2->path->segment->point->first->next->value);
         if (vin2->path->dof > 2)
            fab_path_axis(vout,vin2->path->segment->point->first->next->next->value);
         if (vin2->path->segment->point->next == 0)
            break;
         vin2->path->segment->point = vin2->path->segment->point->next;
         }
      if (vin2->path->segment->next == 0)
         break;
      vin2->path->segment = vin2->path->segment->next;
      }
   }

void fab_path_append(struct fab_vars *vin, struct fab_vars *vout) {
   //
   // append vin to vout, copying limits
   //
   vin->path->segment = vin->path->first;
   vout->path->segment = vout->path->last;
   while (1) {
      //
      // follow segments
      //
      fab_path_segment(vout);
      vin->path->segment->point = vin->path->segment->first;
      while (1) {
         //
         // follow points
         //
         fab_path_point(vout);
         fab_path_axis(vout,vin->path->segment->point->first->value);
         fab_path_axis(vout,vin->path->segment->point->first->next->value);
         if (vin->path->dof > 2)
            fab_path_axis(vout,vin->path->segment->point->first->next->next->value);
         if (vin->path->segment->point->next == 0)
            break;
         vin->path->segment->point = vin->path->segment->point->next;
         }
      if (vin->path->segment->next == 0)
         break;
      vin->path->segment = vin->path->segment->next;
      }
   }

void fab_path_array(struct fab_vars *vin, struct fab_vars *vout, int nx, int ny, float dx, float dy) {
   //
   // array path
   //
   int i,j,x,y,z,dnx,dny;
   dnx = dx * vin->nx / vin->dx;
   dny = dy * vin->ny / vin->dy;
   vout->dx = nx * vin->dx + (nx-1) * dx;
   vout->dy = ny * vin->dy + (ny-1) * dy;
   vout->dz = vin->dz;
   vout->nx = nx * vin->nx + (nx-1) * dnx;
   vout->ny = ny * vin->ny + (ny-1) * dny;
   vout->nz = vin->nz;
   vout->xmin = vin->xmin;
   vout->ymin = vin->ymin;
   vout->zmin = vin->zmin;
   fab_path_start(vout,vin->path->dof);
   for (i = 0; i < nx; ++i) {
      for (j = 0; j < ny; ++j) {
         vin->path->segment = vin->path->first;
         while (1) {
            //
            // follow segments
            //
            fab_path_segment(vout);
            vin->path->segment->point = vin->path->segment->first;
            while (1) {
               //
               // follow points
               //
               fab_path_point(vout);
               x = i*(vin->nx + dnx) + vin->path->segment->point->first->value;
               fab_path_axis(vout,x);
               y = j*(vin->ny + dny) + vin->path->segment->point->first->next->value;
               fab_path_axis(vout,y);
               if (vin->path->dof == 3) {
                  z = vin->path->segment->point->first->next->next->value;
                  fab_path_axis(vout,z);
                  }
               if (vin->path->segment->point->next == 0)
                  break;
               vin->path->segment->point = vin->path->segment->point->next;
               }
            if (vin->path->segment->next == 0)
               break;
            vin->path->segment = vin->path->segment->next;
            }
         }
      }
   }

void fab_follow(struct fab_vars *v, float error, int y, int x, int z) {
    //
    // create path by following edge directions at x,y
    //   error is allowable deviation before stroking (in pixel units)
    //   z is layer height (ignored for 2D paths)
    //
    int xnew=0,ynew=0;
    float xmean,ymean;
    float dx,dy,nx,ny,d;
    int xcur,ycur,xstart,ystart,xseg,yseg,xsum,ysum,nsum;
    char last_direction;
    //
    // don't start on a corner state
    //
    if (((v->array[y][x] & v->direction) == v->corner)
        || ((v->array[y][x] & v->direction) == v->corner2))
        return;
    //
    // otherwise start segment
    //
    xstart = x;
    ystart = y;
    xcur = x;
    ycur = y;
    xseg = x;
    yseg = y;
    xsum = 0;
    ysum = 0;
    nsum = 0;
    last_direction = '?';
    fab_path_segment(v);
    //
    // add first point
    //
    fab_path_point(v);
    fab_path_axis(v,xstart);
    fab_path_axis(v,ystart);
    if (v->path->dof > 2)
       fab_path_axis(v,z);
    while (1) {
		//
		// accumulate current point
		//
		xsum += xcur;
		ysum += ycur;
		nsum += 1;
		xmean = xsum / ((float) nsum);
		ymean = ysum / ((float) nsum);
		dx = (xmean - xseg);
		dy = (ymean - yseg);
		d = sqrt(dx*dx + dy*dy);
		nx = dy/d;
		ny = -dx/d;
		//
		// follow current direction
		//
		if ((v->array[ycur][xcur] & v->direction) == v->north) {
			xnew = xcur;
			ynew = ycur - 1;
			last_direction = 'n'; // remember direction
			v->array[ycur][xcur] &= ~v->direction; // clear direction
		}
		else if ((v->array[ycur][xcur] & v->direction) == v->south) {
			xnew = xcur;
			ynew = ycur + 1;
			last_direction = 's'; // remember direction
			v->array[ycur][xcur] &= ~v->direction; // clear direction
		}
		else if ((v->array[ycur][xcur] & v->direction) == v->east) {
			xnew = xcur + 1;
			ynew = ycur;
			last_direction = 'e'; // remember direction
			v->array[ycur][xcur] &= ~v->direction; // clear direction
		}
		else if ((v->array[ycur][xcur] & v->direction) == v->west) {
			xnew = xcur - 1;
			ynew = ycur;
			last_direction = 'w'; // remember direction
			v->array[ycur][xcur] &= ~v->direction; // clear direction
		}
		else if ((v->array[ycur][xcur] & v->direction) == v->corner) {
			if (last_direction == 'n') {
				xnew = xcur - 1;
				ynew = ycur;
				last_direction = 'w';
			}
			else if (last_direction == 'e') {
				xnew = xcur;
				ynew = ycur - 1;
				last_direction = 'n';
			}
			else if (last_direction == 's') {
				xnew = xcur + 1;
				ynew = ycur;
				last_direction = 'e';
			}
			else if (last_direction == 'w') {
				xnew = xcur;
				ynew = ycur + 1;
				last_direction = 's';
			}
			v->array[ycur][xcur] &= ~v->direction; // set direction to corner2
			v->array[ycur][xcur] |= v->corner2;
		}
		else if ((v->array[ycur][xcur] & v->direction) == v->corner2) {
			if (last_direction == 'n') {
				xnew = xcur - 1;
				ynew = ycur;
				last_direction = 'w';
			}
			else if (last_direction == 'e') {
				xnew = xcur;
				ynew = ycur - 1;
				last_direction = 'n';
			}
			else if (last_direction == 's') {
				xnew = xcur + 1;
				ynew = ycur;
				last_direction = 'e';
			}
			else if (last_direction == 'w') {
				xnew = xcur;
				ynew = ycur + 1;
				last_direction = 's';
			}
			v->array[ycur][xcur] &= ~v->direction; // clear direction
		}
		else if ((v->array[ycur][xcur] & v->direction) == 0) {
			printf("fab.c: oops -- no direction\n");
			exit(-1);
		}
		else {
			printf("fab.c: oops -- unknown direction = %d\n",
				v->array[ycur][xcur] & v->direction);
			exit(-1);
		}
		//
		// finish segment and return if back to start, or stop reached
		//
		if (((xnew == xstart) && (ynew == ystart)) ||
			((v->array[ynew][xnew] & v->direction) == v->stop)) {
				fab_path_point(v);
				fab_path_axis(v,xnew);
				fab_path_axis(v,ynew);
            if (v->path->dof > 2)
				   fab_path_axis(v,z);
				break;
		}
		//
		// check segment at new point
		//
		d = fabs(nx*(xnew-xseg) + ny*(ynew-yseg));
		if (d >= error) {
			//
			// exceeds error, add current point to path
			//
			fab_path_point(v);
			fab_path_axis(v,xcur);
			fab_path_axis(v,ycur);
         if (v->path->dof > 2)
			   fab_path_axis(v,z);
			xsum = 0;
			ysum = 0;
			nsum = 0;
			xseg = xcur;
			yseg = ycur;
		}
		//
		// move to new point
		//
		xcur = xnew;
		ycur = ynew;
	}
}

void fab_vectorize(struct fab_vars *v, float error, int z) {
   //
   // vectorize xy edge directions
   //    error is transverse distance from segment mean
   //       in lattice units
   //    z is layer height (ignored for 2D paths)
   //
   int x, y;
   //
   // find edge starts
   //
   for (x = 0; x < v->nx; ++x)
      if ((v->array[0][x] & v->direction) == v->south)
         fab_follow(v, error, 0,x, z);
   for (y = 0; y < v->ny; ++y)
      if ((v->array[y][v->nx-1] & v->direction) == v->west)
         fab_follow(v, error, y,v->nx-1, z);
   for (x = (v->nx - 1); x >= 0 ; --x)
      if ((v->array[v->ny-1][x] & v->direction) == v->north)
         fab_follow(v, error, v->ny-1,x, z);
   for (y = (v->ny - 1); y >= 0; --y)
      if ((v->array[y][0] & v->direction) == v->east)
         fab_follow(v, error, y,0, z);
   //
   // find remaining interior paths
   //
   for (y = 1; y < (v->ny - 1); ++y) {
      if ((y % 2) == 1) {
         for (x = 1; x < (v->nx - 1); ++x) {
            if (v->array[y][x] & v->direction) {
               fab_follow(v,error,y,x,z);
               }
            }
         }
      else {
         for (x = (v->nx - 2); x > 0; --x) {
            if (v->array[y][x] & v->direction) {
               fab_follow(v,error,y,x,z);
               }
            }
         }
      }
   }

void fab_halftone(struct fab_vars *v, float threshold, int points, float size, float spacing, float offset, int invert) {
   //
   // halftone array to path
   //
   int xmin,ymin,x,y,n,i,space,shift,top;
   float pi,angle,r;
   top = pow(2,v->bit_depth) - 1;
   //
   // start 2D path
   //
   fab_path_start(v,2);
   //
   // loop over spots
   //
   pi = 4.0*atan(1.0);
   angle = 2.0*pi/(points-1.0);
   n = v->nx * (0.5*size/v->dx);
   space = spacing*2*n;
   shift = 0;
   for (ymin = n; ymin < (v->ny - n); ymin += space) {
      for (xmin = n; xmin < (v->nx - n); xmin += space) {
         r = n * (invert*top + (1-2*invert)* v->array[ymin][xmin]) / ((float) top);
         //
         // if spot radius larger than threshold, add to path
         //
         if (r >= threshold) {
            fab_path_segment(v);
            for (i = 0; i < points; ++i) {
               x = shift + 0.5 + xmin + r*cos(i*angle);
               y = 0.5 + ymin + r*sin(i*angle);
               fab_path_point(v);
               fab_path_axis(v,x);
               fab_path_axis(v,y);
               }
            }
         }
      shift += offset * space;
      if (shift >= space)
         shift -= space;
      }
   }

