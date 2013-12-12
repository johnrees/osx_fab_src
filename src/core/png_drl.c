/*
%  	Rewind and Stop
X#Y# 	Move and Drill
T# 	Tool Selection
M30 	End of Program
M00 	End of Program
R#X#Y# 	Repeat Hole
G05, G81 	Select Drill Mode
G90 	Absolute Mode
G91 	Incremental Mode
G92 X#Y# 	Set Zero
G93 X#Y# 	Set Zero
M48 	Program Header to first "%"
M72 	English-Imperial Mode
*/
/* 
%	Rewind and Stop
X#Y#	Move and Drill
T#	Tool Selection
M30	End of Program
M00	End of Program
M25	Beginning of Pattern
M31	Beginning of Pattern
M01	End of Pattern
M02 X#Y#	Repeat Pattern
R#M02X#Y#	Multiple Repeat Pattern
M02 X#Y# M70	Swap Axis
M02 X#Y# M80	Mirror Image X Axis
M02 X#Y# M90	Mirror Image Y Axis
M08	End of Step and Repeat
N#	Block Sequence Number
/	Block Delete
R#X#Y#	Repeat Hole
G05, G81	Select Drill Mode
G04 X#	Variable Dwell (ignored)
G90	Absolute Mode
G91	Incremental Mode
G92 X#Y#	Set Zero
G93 X#Y#	Set Zero
M48	Program Header to first "%"
M47	Operator Message CRT Display
M71	Metric Mode
M72	English-Imperial Mode
Snn	Spindle Speed (RPM)
Fnn	Z axis feed speed (IPM)
*/
/*
M48	      Program Header
M72, LZ	   English measurement Leading 0's
T03C0.038	T03 .0380"
T04C0.125	T04 .1250"
T06C0.046	T06 .0460"
T12C0.063	T12 .0630"
T16C0.250	T16 .2500"
%	         Rewind and Stop (start of drill data)
T03	      Drill COMMAND GET TOOL 3
X00581Y0122	Drill location with TOOL 3
T06	      Drill COMMAND GET TOOL 6
X01657Y03295	Drill location with TOOL 6
T12	      Drill COMMAND GET TOOL 12
X00585Y03311	Drill location with TOOL 12
T04	      Drill COMMAND GET TOOL 4
X00873Y02691	Drill location with TOOL 4
T16	      Drill COMMAND GET TOOL 16
X00625Y02191	Drill location with TOOL 16
T00	      Drill COMMAND UNLOAD TOOL
M30	      End of Program
*/
//
// png_drl.c
//    PNG to Excellon
//
// Neil Gershenfeld
// CBA MIT 11/22/12
//
// (c) Massachusetts Institute of Technology 2012
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include "fab.h"

#define MAX_HOLES 100000
#define TOLERANCE 0.01 // hole drill match percent tolerance

void fab_write_drl(struct fab_vars *v, char *output_file_name) {
	FILE *output_file;
   int x,y,edges,points,hole,holes,drill,drills,index[MAX_HOLES];
   float units,scale;
   float x0[MAX_HOLES],y0[MAX_HOLES],r[MAX_HOLES],d[MAX_HOLES];
   output_file = fopen(output_file_name,"w");
   //
   // threshold array
   //
   fab_threshold(v,0.5);
   //
   // find edges
   //
   edges = fab_edges(v);
   if (edges == 0) {
      printf("png_drl: oops -- no edges\n");
      exit(-1);
      }
   //
   // set edge directions
   //
   fab_directions(v);
   //
   // follow edges
   //
   fab_path_start(v,2);
   fab_vectorize(v,1,0);
   //
   //
   //
   units = 1.0/25.4; // inches
   scale = units*v->dx/(v->nx-1.0);
   //
   // follow segments to find holes
   //
   v->path->segment = v->path->first;
   hole = 0;
   while (1) {
      //
      // find hole centers
      //
      v->path->segment->point = v->path->segment->first;
      x = v->path->segment->point->first->value;
      y = v->ny - v->path->segment->point->first->next->value;
      x0[hole] = x;
      y0[hole] = y;
      points = 1;
      while (1) {
         if (v->path->segment->point->next == 0) {
            x0[hole] = x0[hole] / points;
            y0[hole] = y0[hole] / points;
            break;
            }
         v->path->segment->point = v->path->segment->point->next;
         x = v->path->segment->point->first->value;
         y = v->ny - v->path->segment->point->first->next->value;
         x0[hole] += x;
         y0[hole] += y;
         points += 1;
         }
      //
      // find hole radii
      //
      v->path->segment->point = v->path->segment->first;
      x = v->path->segment->point->first->value;
      y = v->ny - v->path->segment->point->first->next->value;
      r[hole] = sqrt((x-x0[hole])*(x-x0[hole])+(y-y0[hole])*(y-y0[hole]));
      points = 1;
      while (1) {
         if (v->path->segment->point->next == 0) {
            x0[hole] = scale * x0[hole];
            y0[hole] = scale * y0[hole];
            r[hole] = scale * r[hole] / points;
            break;
            }
         v->path->segment->point = v->path->segment->point->next;
         x = v->path->segment->point->first->value;
         y = v->ny - v->path->segment->point->first->next->value;
         r[hole] += sqrt((x-x0[hole])*(x-x0[hole])+(y-y0[hole])*(y-y0[hole]));
         points += 1;
         }
      hole += 1;
      if (v->path->segment->next == 0) {
         break;
         }
      v->path->segment = v->path->segment->next;
      }
   holes = hole;
   //
   // find unique drills
   //
   drills = 0;
   for (hole = 0; hole < holes; ++hole) {
      drill = 0;
      while (1) {
         if (drill == drills) {
            drills += 1;
            d[drill] = 2*r[hole];
            index[hole] = drill;
            break;
            }
         if ((fabs(d[drill] - 2*r[hole])/d[drill]) < TOLERANCE) {
            index[hole] = drill;
            break;
            }
         drill += 1;
         }
      }
   //
   // write header
   //
   fprintf(output_file,"M48\n"); // program header
   fprintf(output_file,"M72, LZ\n"); // inches, leading zeros
   //
   // write drills
   //
   for (drill = 0; drill < drills; ++drill) {
      fprintf(output_file,"T%.2dC%.3f\n",(1+drill),d[drill]);
      }
   //
   // write holes
   //
   fprintf(output_file,"%%\n"); // rewind and stop (start hole data)
   for (hole = 0; hole < holes; ++hole) {
      fprintf(output_file,"T%.2d\n",(1+index[hole])); // tool
      x = 10000*x0[hole];
      y = 10000*y0[hole];
      fprintf(output_file,"X%.6dY%.6d\n",x,y); // location
      }
   /*
      T06	      Drill COMMAND GET TOOL 6
      X01657Y03295	Drill location with TOOL 6
      T12	      Drill COMMAND GET TOOL 12
      X00585Y03311	Drill location with TOOL 12
      T04	      Drill COMMAND GET TOOL 4
      X00873Y02691	Drill location with TOOL 4
      T16	      Drill COMMAND GET TOOL 16
      X00625Y02191	Drill location with TOOL 16
      T00	      Drill COMMAND UNLOAD TOOL
   */
   //
   // close file
   //
   fprintf(output_file,"M30\n"); // end of program
   fclose(output_file);
   printf("wrote %s\n",output_file_name);
   printf("   holes: %d, drills: %d\n",holes,drills);
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
      printf("command line: png_drl in.png out.drl\n");
      printf("   in.png = input PNG file\n");
      printf("   out.drl = output Excellon file\n");
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
   // write array to Excellon
   //
   fab_write_drl(&v,argv[2]);
   //
   // exit
   //
   exit(0);
   }

