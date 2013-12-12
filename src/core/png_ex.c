#include <stdlib.h>
#include <stdio.h>
#include <png.h>

int x, y;
int width, height;
png_byte color_type;
png_byte bit_depth;
png_structp png_ptr;
png_infop info_ptr;
png_bytep * row_pointers;

void read_png_file(char* file_name) {
	FILE *fp = fopen(file_name, "rb");
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL,NULL,NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);
	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;
   printf("width %d height %d color %d bit %d\n",
      width,height,color_type,bit_depth);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
	png_read_image(png_ptr, row_pointers);
   fclose(fp);
   }

void write_png_file(char* file_name) {
	FILE *fp = fopen(file_name, "wb");
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL,NULL,NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height,
	   bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
   fclose(fp);
   }

void process_file(void) {
   png_byte *ptr;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {
			ptr = &(row_pointers[y][x*3]);
			ptr[0] = 255;
			ptr[1] = ptr[2];
		   }
   }

int main(int argc, char **argv) {
	if (argc != 3) {
      printf("Usage: program_name <file_in> <file_out>\n");
      exit(0);
      }
	read_png_file(argv[1]);
	process_file();
	write_png_file(argv[2]);
   }
