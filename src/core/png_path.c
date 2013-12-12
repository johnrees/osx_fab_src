//
// png_path.c
//    slice, offset, and vectorize PNG to path
//
// Neil Gershenfeld 10/14/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//
// todo:

#include "fab.h"

int main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   struct fab_vars vxz;
   init_vars(&vxz);
   struct fab_vars vyz;
   init_vars(&vyz);
   int offset_number,offset_count,
      layer,layer_number,nz,xz,yz,xy,
      ix,ixmin,ixmax,idx,
      iy,iymin,iymax,idy,
      iz,ithreshold,
      *tool_dx,*tool_dy,*tool_iz,tool_pt,tool_n,tool_ir,tool_id,
      *clearance_dx,*clearance_dy,*clearance_iz,clearance_pt,clearance_n,clearance_ir,clearance_id,
      remaining_count,remaining_count_sum,tool_collision;
   float error,offset_diameter,offset_overlap,clearance_length,clearance_diameter,
      intensity_top,intensity_bottom,intensity_layer,
      z_top,z_bottom,z_thickness,z_layer,*z_list,distance;
   char tool_type;
   //
   // command line args
   //
   if ((argc < 3) || (argc > 18)) {
      printf("command line: png_path in.png out.path [error [offset_diameter [offset_number [offset_overlap [intensity_top [intensity_bottom [z_top [z_bottom [z_thickness [xz [yz [xy [type [clearance_length clearance_diameter]]]]]]]]]]]]]]\n");
      printf("   in.png = input PNG file\n");
      printf("   out.path = output path file\n");
      printf("   error = allowable vector fit deviation (optional, pixels, default 1.1)\n");
      printf("   offset_diameter = diameter to offset (optional, mm, default 0)\n");
      printf("   offset_number = number of contours to offset (optional, -1 to fill all, default 1)\n");
      printf("   offset_overlap = tool offset overlap fraction (optional, 0 (no overlap) - 1 (complete overlap, default 0.5))\n");
      printf("   intensity_top = top slice intensity (optional, 0-1, default 0.5)\n");
      printf("   intensity_bottom = bottom slice intensity (optional, 0-1, default intensity_top)\n");
      printf("   z_top = top slice z value (optional, mm, default 0)\n");
      printf("   z_bottom = bottom slice z value (optional, mm, default z_top)\n");
      printf("   z_thickness = slice z thickness (optional, mm, default z_top-z_bottom)\n");
      printf("   xz = xz finish (optional, 1=yes, default 0\n");
      printf("   yz = yz finish (optional, 1=yes, default 0\n");
      printf("   xy = xy path (optional, 1=yes, default 1\n");
      printf("   type = finish tool type (optional, f=flat end, b=ball end, default f\n");
      printf("   clearance_length = finish tool clearance length (optional, mm, 0 = no limit, default 0\n");
      printf("   clearance_diameter = finish tool clearance diameter (optional, mm, default offset_diameter\n");
      exit(-1);
      }
   error = 1.1; 
   offset_diameter = 0;
   offset_number = 1;
   offset_overlap = 0.5;
   intensity_top = 0.5;
   intensity_bottom = intensity_top;
   z_top = 0;
   z_bottom = 0;
   z_thickness = 0;
   xz = 0;
   yz = 0;
   xy = 1;
   tool_type = 'f';
   if (argc >= 4) {
      sscanf(argv[3],"%f",&error);
      }
   if (argc >= 5) {
      sscanf(argv[4],"%f",&offset_diameter);
      }
   if (argc >= 6) {
      sscanf(argv[5],"%d",&offset_number);
      }
   if (argc >= 7) {
      sscanf(argv[6],"%f",&offset_overlap);
      }
   if (argc >= 8) {
      sscanf(argv[7],"%f",&intensity_top);
      intensity_bottom = intensity_top;
      }
   if (argc >= 9) {
      sscanf(argv[8],"%f",&intensity_bottom);
      }
   if (argc >= 10) {
      sscanf(argv[9],"%f",&z_top);
      z_bottom = z_top;
      }
   if (argc >= 11) {
      sscanf(argv[10],"%f",&z_bottom);
      z_thickness = z_top - z_bottom;
      }
   if (argc >= 12) {
      sscanf(argv[11],"%f",&z_thickness);
      }
   if (argc >= 13) {
      sscanf(argv[12],"%d",&xz);
      }
   if (argc >= 14) {
      sscanf(argv[13],"%d",&yz);
      }
   if (argc >= 15) {
      sscanf(argv[14],"%d",&xy);
      }
   if (argc >= 16) {
      sscanf(argv[15],"%c",&tool_type);
      }
   clearance_length = z_top - z_bottom;
   clearance_diameter = offset_diameter;
   if (argc >= 17) {
      sscanf(argv[16],"%f",&clearance_length);
      if (clearance_length == 0)
         clearance_length = z_top - z_bottom;
      }
   if (argc >= 18) {
      sscanf(argv[17],"%f",&clearance_diameter);
      }
   tool_collision = 0;
   //
   // read PNG
   //
   fab_read_png(&v,argv[1]);
   //
   // 2D path?
   //
   if (argc < 9) {
      //
      // start path
      //
      fab_path_start(&v,2);
      printf("png_path: intensity %f\n",intensity_top);
      //
      // copy image to array
      //
      fab_png_array(&v);
      //
      // threshold array
      //
      printf("   threshold\n");
      fab_threshold(&v,intensity_top);
      //
      // find edges
      //
      printf("   find edges\n");
      remaining_count = fab_edges(&v);
      if (remaining_count != 0) {
         //
         // find edge distances
         //
         printf("   find distances\n");
         //test_fab_distances(&v);
         fab_distances(&v);
         //
         // loop over contours
         //
         printf("   offset:\n");
         offset_count = 0;
         remaining_count_sum = 0;
         do {
            //
            // offset
            //
            distance = offset_diameter/2.0 + offset_diameter*(1-offset_overlap)*offset_count;
            remaining_count = fab_offset(&v,distance);
            remaining_count_sum += remaining_count;
            printf("      distance %f, %d exterior points remain\n",
               distance,remaining_count);
            if (remaining_count != 0) {
               //
               // set edge directions
               //
               fab_directions(&v);
               //
               // vectorize edge directions
               //
               fab_vectorize(&v,error,0);
               }
            ++offset_count;
            } while ((offset_count != offset_number) && (remaining_count != 0));
         }
      else {
         printf("path.c: oops -- no layer edges\n");
         exit(-1);
         }
      if (remaining_count_sum == 0) {
         printf("png_path: no offset edges\n");
         exit(1);
         }
      }
   //
   // 3D path
   //
   else {
      //
      // start path
      //
      fab_path_start(&v,3);
      //
      // set path z bounds
      //
      v.zmin = z_bottom;
      v.dz = (z_top - z_bottom);
      if (v.dz == 0)
         v.nz = 1;
      else
         v.nz = v.dz * v.nx / v.dx;
      //
      // find z layer values
      //
      if (xy == 1) {
         if (v.dz == 0)
            nz = 1;
         else {
            nz = 2 + (z_top-z_bottom)/z_thickness;
            z_list = (float*) malloc(nz*sizeof(float));
            z_layer = z_top;
            nz = 0;
            while (1) {
               z_list[nz] = z_layer;
               nz += 1;
               if (z_layer == z_bottom)
                  break;
               z_layer -= z_thickness;
               if (z_layer < z_bottom)
                  z_layer = z_bottom;
               }
            }
         }
      //
      // copy image to array for finish cuts
      //
      fab_png_array(&v);
      //
      // create tool shape offset list
      //
      tool_ir = v.nx*offset_diameter/(2*v.dx);
      tool_id = 2*tool_ir;
      tool_dx = (int*) malloc(tool_id*tool_id*sizeof(int));
      tool_dy = (int*) malloc(tool_id*tool_id*sizeof(int));
      tool_iz = (int*) malloc(tool_id*tool_id*sizeof(int));
      tool_pt = 0;
      tool_n = 0;
      for (ix = 0; ix < tool_id; ++ix)
         for (iy = 0; iy < tool_id; ++iy)
            if (sqrt((ix-tool_ir)*(ix-tool_ir)+(iy-tool_ir)*(iy-tool_ir)) < tool_ir) {
               tool_dx[tool_pt] = ix-tool_ir;
               tool_dy[tool_pt] = iy-tool_ir;
               if (tool_type == 'f')
                  tool_iz[tool_pt] = 0;
               else if (tool_type == 'b') {
                  tool_iz[tool_pt] = pow(2,v.bit_depth)*(tool_ir - sqrt(tool_ir*tool_ir
                     - ((ix-tool_ir)*(ix-tool_ir) + (iy-tool_ir)*(iy-tool_ir))))/v.nz;
                  }
               else {
                  printf("path.c: oops -- unsupported tool type\n");
                  exit(-1);
                  }
               tool_pt += 1;
               tool_n = tool_pt;
               }
      //
      // create tool clearance offset list
      //
      clearance_ir = v.nx*clearance_diameter/(2*v.dx);
      clearance_id = 2*clearance_ir;
      clearance_dx = (int*) malloc(clearance_id*clearance_id*sizeof(int));
      clearance_dy = (int*) malloc(clearance_id*clearance_id*sizeof(int));
      clearance_iz = (int*) malloc(clearance_id*clearance_id*sizeof(int));
      clearance_pt = 0;
      clearance_n = 0;
      for (ix = 0; ix < clearance_id; ++ix)
         for (iy = 0; iy < clearance_id; ++iy)
            if (((sqrt((ix-clearance_ir)*(ix-clearance_ir)+(iy-clearance_ir)*(iy-clearance_ir))) >= tool_ir)
             && ((sqrt((ix-clearance_ir)*(ix-clearance_ir)+(iy-clearance_ir)*(iy-clearance_ir))) < clearance_ir)) {
               clearance_dx[clearance_pt] = ix-clearance_ir;
               clearance_dy[clearance_pt] = iy-clearance_ir;
               clearance_iz[clearance_pt] = (pow(2,v.bit_depth)-1)*clearance_length/v.dz;
               clearance_pt += 1;
               clearance_n = clearance_pt;
               }
      //
      // check for xz cut
      //
      if (xz == 1) {
         //
         // allocate xz finish cut array
         //
         vxz.nx = v.nx;
         vxz.ny = v.nz;
         vxz.nz = 1;
         vxz.dx = v.dx;
         vxz.dy = v.dz;
         vxz.dz = 0;
         vxz.xmin = v.xmin;
         vxz.ymin = v.zmin;
         vxz.zmin = 0;
         vxz.array = malloc(v.nz*sizeof(uint32_t *));
         for (iz = 0; iz < v.nz; ++iz)
            vxz.array[iz] = malloc(v.nx*sizeof(uint32_t));
         //
         // start xz path
         //
         fab_path_start(&vxz,3);
         //
         // y loop
         //
         iymin = v.ny*clearance_diameter/(2.0*v.dy);
         iymax = (v.ny-1) - v.ny*clearance_diameter/(2.0*v.dy);
         idy = v.ny*offset_diameter*(1-offset_overlap)/v.dy;
         printf("   xz finish:\n");
         for (iy = iymin; iy <= iymax; (iy += idy)) {
            printf("      y=%d/%d\n",iy,iymax);
            //
            // x loop
            //
            for (ix = 0; ix < vxz.nx; ++ix) {
               //
               // set z threshold
               //
               if ((ix <= clearance_ir) | (ix >= (vxz.nx-clearance_ir))) 
                  //
                  // too close to edge, set max
                  //
                  ithreshold = pow(2,v.bit_depth);
               else {
                  //
                  // offset for tool shape
                  //
                  ithreshold = 0;
                  for (tool_pt = 0; tool_pt < tool_n; ++tool_pt) {
                     if (v.array[iy+tool_dy[tool_pt]][ix+tool_dx[tool_pt]] > (ithreshold+tool_iz[tool_pt]))
                        ithreshold = v.array[iy+tool_dy[tool_pt]][ix+tool_dx[tool_pt]] - tool_iz[tool_pt];
                     }
                  //
                  // check clearance
                  //
                  for (clearance_pt = 0; clearance_pt < clearance_n; ++clearance_pt) {
                     if (v.array[iy+clearance_dy[clearance_pt]][ix+clearance_dx[clearance_pt]] > (ithreshold+clearance_iz[clearance_pt])) {
                        ithreshold = v.array[iy+clearance_dy[clearance_pt]][ix+clearance_dx[clearance_pt]] - clearance_iz[clearance_pt];
                        tool_collision = 1;
                        }
                     }
                  }
               //
               // convert from image to lattice units
               //
               ithreshold = 1+(v.nz-2.0)*ithreshold/(pow(2,v.bit_depth)-1.0); // +1 for bottom edge
               //
               // z (array y) loop
               //
               for (iz = 0; iz < vxz.ny; ++iz) {
                  if (iz <= ithreshold)
                     vxz.array[iz][ix] = v.interior;
                  else
                     vxz.array[iz][ix] = v.empty;
                  }
               }
            //
            // set edge directions
            //
            fab_directions(&vxz);
            //
            // vectorize edge directions
            //
            fab_vectorize(&vxz,error,iy);
            }
         //
         // copy xz path
         //
         vxz.path->segment = vxz.path->first;
         while (1) {
            //
            // follow segment forward
            //
            vxz.path->segment->point = vxz.path->segment->first;
            fab_path_segment(&v);
            while (1) {
               //
               // follow points
               //
               fab_path_point(&v);
               vxz.path->segment->point->axis = vxz.path->segment->point->first;
               ix = vxz.path->segment->point->axis->value;
               vxz.path->segment->point->axis = vxz.path->segment->point->axis->next;
               iy = vxz.path->segment->point->axis->value;
               vxz.path->segment->point->axis = vxz.path->segment->point->axis->next;
               iz = vxz.path->segment->point->axis->value;
               fab_path_axis(&v,ix);
               fab_path_axis(&v,iz);
               fab_path_axis(&v,iy);
               if (vxz.path->segment->point->next == 0)
                  break;
               vxz.path->segment->point = vxz.path->segment->point->next;
               }
            if (vxz.path->segment->next == 0)
               break;
            vxz.path->segment = vxz.path->segment->next;
            //
            // follow segment backwards
            //
            vxz.path->segment->point = vxz.path->segment->last;
            fab_path_segment(&v);
            while (1) {
               //
               // follow points
               //
               fab_path_point(&v);
               vxz.path->segment->point->axis = vxz.path->segment->point->first;
               ix = vxz.path->segment->point->axis->value;
               vxz.path->segment->point->axis = vxz.path->segment->point->axis->next;
               iy = vxz.path->segment->point->axis->value;
               vxz.path->segment->point->axis = vxz.path->segment->point->axis->next;
               iz = vxz.path->segment->point->axis->value;
               fab_path_axis(&v,ix);
               fab_path_axis(&v,iz);
               fab_path_axis(&v,iy);
               if (vxz.path->segment->point->previous == 0)
                  break;
               vxz.path->segment->point = vxz.path->segment->point->previous;
               }
            if (vxz.path->segment->next == 0)
               break;
            vxz.path->segment = vxz.path->segment->next;
            }
         }
      //
      // check for yz cut
      //
      if (yz == 1) {
         //
         // allocate yz finish cut array
         //
         vyz.nx = v.ny;
         vyz.ny = v.nz;
         vyz.nz = 1;
         vyz.dx = v.dy;
         vyz.dy = v.dz;
         vyz.dz = 0;
         vyz.xmin = v.ymin;
         vyz.ymin = v.zmin;
         vyz.zmin = 0;
         vyz.array = malloc(v.nz*sizeof(uint32_t *));
         for (iz = 0; iz < v.nz; ++iz)
            vyz.array[iz] = malloc(v.ny*sizeof(uint32_t));
         //
         // start yz path
         //
         fab_path_start(&vyz,3);
         //
         // x loop
         //
         ixmin = v.nx*clearance_diameter/(2.0*v.dx);
         ixmax = (v.nx-1) - v.nx*clearance_diameter/(2.0*v.dx);
         idx = v.nx*offset_diameter*(1-offset_overlap)/v.dx;
         printf("   yz finish:\n");
         for (ix = ixmax; ix >= ixmin; (ix -= idx)) {
            printf("      x=%d/%d\n",ix,ixmax);
            //
            // y (array x) loop
            //
            for (iy = 0; iy < vyz.nx; ++iy) {
               //
               // set z threshold
               //
               if ((iy <= clearance_ir) | (iy >= (vyz.nx-clearance_ir))) 
                  //
                  // too close to edge, set max
                  //
                  ithreshold = pow(2,v.bit_depth);
               else {
                  //
                  // offset for tool shape
                  //
                  ithreshold = 0;
                  for (tool_pt = 0; tool_pt < tool_n; ++tool_pt) {
                     if (v.array[iy+tool_dy[tool_pt]][ix+tool_dx[tool_pt]] > (ithreshold+tool_iz[tool_pt]))
                        ithreshold = v.array[iy+tool_dy[tool_pt]][ix+tool_dx[tool_pt]] - tool_iz[tool_pt];
                     }
                  //
                  // check clearance
                  //
                  for (clearance_pt = 0; clearance_pt < clearance_n; ++clearance_pt) {
                     if (v.array[iy+clearance_dy[clearance_pt]][ix+clearance_dx[clearance_pt]] > (ithreshold+clearance_iz[clearance_pt])) {
                        ithreshold = v.array[iy+clearance_dy[clearance_pt]][ix+clearance_dx[clearance_pt]] - clearance_iz[clearance_pt];
                        tool_collision = 1;
                        }
                     }
                  }
               //
               // convert from image to lattice units
               //
               ithreshold = 1+(v.nz-2.0)*ithreshold/(pow(2,v.bit_depth)-1.0); // +1 for bottom edge
               //
               // z (array y) loop
               //
               for (iz = 0; iz < vyz.ny; ++iz) {
                  if (iz <= ithreshold)
                     vyz.array[iz][iy] = v.interior;
                  else
                     vyz.array[iz][iy] = v.empty;
                  }
               }
            //
            // set edge directions
            //
            fab_directions(&vyz);
            //
            // vectorize edge directions
            //
            fab_vectorize(&vyz,error,ix);
            }
         //
         // copy yz path
         //
         vyz.path->segment = vyz.path->first;
         while (1) {
            //
            // follow segment forwards
            //
            vyz.path->segment->point = vyz.path->segment->first;
            fab_path_segment(&v);
            while (1) {
               //
               // follow points
               //
               fab_path_point(&v);
               vyz.path->segment->point->axis = vyz.path->segment->point->first;
               ix = vyz.path->segment->point->axis->value;
               vyz.path->segment->point->axis = vyz.path->segment->point->axis->next;
               iy = vyz.path->segment->point->axis->value;
               vyz.path->segment->point->axis = vyz.path->segment->point->axis->next;
               iz = vyz.path->segment->point->axis->value;
               fab_path_axis(&v,iz);
               fab_path_axis(&v,ix);
               fab_path_axis(&v,iy);
               if (vyz.path->segment->point->next == 0)
                  break;
               vyz.path->segment->point = vyz.path->segment->point->next;
               }
            if (vyz.path->segment->next == 0)
               break;
            vyz.path->segment = vyz.path->segment->next;
            //
            // follow segment backwards
            //
            vyz.path->segment->point = vyz.path->segment->last;
            fab_path_segment(&v);
            while (1) {
               //
               // follow points
               //
               fab_path_point(&v);
               vyz.path->segment->point->axis = vyz.path->segment->point->first;
               ix = vyz.path->segment->point->axis->value;
               vyz.path->segment->point->axis = vyz.path->segment->point->axis->next;
               iy = vyz.path->segment->point->axis->value;
               vyz.path->segment->point->axis = vyz.path->segment->point->axis->next;
               iz = vyz.path->segment->point->axis->value;
               fab_path_axis(&v,iz);
               fab_path_axis(&v,ix);
               fab_path_axis(&v,iy);
               if (vyz.path->segment->point->previous == 0)
                  break;
               vyz.path->segment->point = vyz.path->segment->point->previous;
               }
            if (vyz.path->segment->next == 0)
               break;
            vyz.path->segment = vyz.path->segment->next;
            }
         }
      //
      // check for xy cut
      //
      if (xy == 1) {
         //
         // z loop
         //
         for (layer = (nz-1); layer >= 0; --layer) {
            if (v.dz == 0) {
               z_layer = z_top;
               intensity_layer = intensity_top;
               layer_number = 0;
               }
            else {
               z_layer = z_list[layer];
               intensity_layer = intensity_bottom + (intensity_top-intensity_bottom)*(z_layer-z_bottom)/(z_top-z_bottom);
               layer_number = v.nz*(z_layer-z_bottom)/(z_top-z_bottom);
               }
            printf("png_path: intensity %f, z %f\n",intensity_layer,z_layer);
            //
            // copy image to array
            //
            fab_png_array(&v);
            //
            // threshold array
            //
            printf("   threshold\n");
            fab_threshold(&v,intensity_layer);
            //
            // find edges
            //
            printf("   find edges\n");
            remaining_count = fab_edges(&v);
            if (remaining_count == 0) {
               printf("      no layer edges\n");
               continue;
               }
            //
            // find edge distances
            //
            printf("   find distances\n");
            //test_fab_distances(&v);
            fab_distances(&v);
            //
            // loop over contours
            //
            printf("   offset:\n");
            offset_count = 0;
            remaining_count_sum = 0;
            do {
               //
               // offset
               //
               distance = offset_diameter/2.0 + offset_diameter*(1-offset_overlap)*offset_count;
               remaining_count = fab_offset(&v,distance);
               remaining_count_sum += remaining_count;
               printf("      distance %f, %d exterior points remain\n",
                  distance,remaining_count);
               if (remaining_count != 0) {
                  //
                  // set edge directions
                  //
                  fab_directions(&v);
                  //
                  // vectorize edge directions
                  //
                  fab_vectorize(&v,error,layer_number);
                  }
               ++offset_count;
               } while ((offset_count != offset_number) && (remaining_count != 0));
            }
         if (remaining_count_sum == 0) {
            printf("png_path: no offset edges\n");
            exit(1);
            }
         }
      }
   if (tool_collision == 1)
      printf("   tool collisions\n");
   //
   // write path
   //
   fab_write_path(&v,argv[2]);
   //
   // exit
   //
   exit(0);
   }

