//
// svg_path.c
//    convert SVG to path
//
// Neil Gershenfeld 9/21/13
// (c) Massachusetts Institute of Technology 2013
//
// This work may be reproduced, modified, distributed,
// performed, and displayed for any purpose, but must
// acknowledge the fab modules project. Copyright is
// retained and must be preserved. The work is provided
// as is; no warranty is provided, and users accept all 
// liability.
//
// todo
//    gradient intensity
//

#include "fab.h"

#define SVG_MAX_FILE 1000000
#define SVG_MAX_TRANSFORM 100

double angle(double ux, double uy, double vx, double vy) {
   double sign = (((ux*vy-uy*vx) > 0) ? 1 : -1);
   return (sign*acos((ux*vx+uy*vy)/(sqrt(ux*ux+uy*uy)*sqrt(vx*vx+vy*vy))));
   }
   
void parse_number_units(char *ptr, double *number, double *unit_scale, char *units) {
   //
   // parse quoted number and units
   // return scale in mm/unit
   // 
   char *start,*end;
   start = strstr(ptr,"\"");
   end = strstr(start+1,"\"");
   char c1,c2;
   c1 = *(end-1);
   c2 = *(end-2);
   if ((((c2 >= '0') && (c2 <= '9')) || (c2 == '.'))
   && (((c1 >= '0') && (c1 <= '9')) || (c1 == '.'))) {
      *number = strtod(start+1,NULL);
      strcpy(units,"px");
      }
   else {
      units[0] = *(end-2);
      units[1] = *(end-1);
      units[2] = 0;
      *(end-2) = ' ';
      *(end-1) = ' ';
      *number = strtod(start+1,NULL);
      }
   if (0 == strncmp(units,"px",2))
      *unit_scale = 25.4*1.0/90.0; // Inkscape px default 90/inch
   else if (0 == strncmp(units,"pt",2))
      *unit_scale = 25.4*1.0/72.0;
   else if (0 == strncmp(units,"in",2))
      *unit_scale = 25.4;
   else if (0 == strncmp(units,"mm",2))
      *unit_scale = 1.0;
   else if (0 == strncmp(units,"cm",2))
      *unit_scale = 10.0;
   else {
      printf("svg_path: oops -- don't recognize unit %s\n",units);
      exit(-1);
      }
   }

void next_number(char **ptr, double *number) {
   //
   // return next number after pointer
   //
   char haystack[] = "0123456789.-+";
   char *end;
   //
   // find start
   //
   while (1) {
      if (strchr(haystack,**ptr) != NULL)
         break;
      *ptr += 1;
      }
   //
   // find end
   //
   end = *ptr;
   while (1) {
      if (strchr(haystack,*end) == NULL)
         break;
      end += 1;
      }
   //
   // move pointer to end and return number
   //
   *number = strtod(*ptr,&end);
   *ptr = end;
   }

char next_element(char **ptr, char current_element) {
   //
   // return next path element after pointer
   //
   char number_haystack[] = "0123456789.-+";
   char element_haystack[] = "mMlLhHvVcCsSaAzZ\"";
   while (1) {
      if (strchr(element_haystack,**ptr) != NULL)
         return(**ptr);
      else if (strchr(number_haystack,**ptr) != NULL)
         return(current_element);
      else
         *ptr += 1;
      }
   }

void clear_transform(int *transform) {
   //
   // pop current transform from stack
   //
   *transform -= 1;
   }

int hex_int(char chr) {
   //
   // return hex for char
   //
   if (chr == '0') return 0;
   else if (chr == '1') return 1;
   else if (chr == '2') return 2;
   else if (chr == '3') return 3;
   else if (chr == '4') return 4;
   else if (chr == '5') return 5;
   else if (chr == '6') return 6;
   else if (chr == '7') return 7;
   else if (chr == '8') return 8;
   else if (chr == '9') return 9;
   else if ((chr == 'a') || (chr == 'A')) return 10;
   else if ((chr == 'b') || (chr == 'B')) return 11;
   else if ((chr == 'c') || (chr == 'C')) return 12;
   else if ((chr == 'd') || (chr == 'D')) return 13;
   else if ((chr == 'e') || (chr == 'e')) return 14;
   else if ((chr == 'f') || (chr == 'F')) return 15;
   printf("svg_path: oops -- non-hex char\n");
   exit(-1);
   }

void set_intensity(char *start, int *zn, struct fab_vars *v, int zsign) {
   char *end,*ptr;
   int r,g,b;
   float i;
   //
   // set zn from element intensity
   //
   end = strstr(start+1,">");
   ptr = strstr(start,"stroke:");
   if ((ptr != NULL) && (ptr < end)) {
      //
      // stroke found, check for hex encoding
      //
      ptr = strstr(ptr,"#");
      if ((ptr != NULL) && (ptr < end)) {
         //
         // hex encoding found, set intensity
         //
         r = 16*hex_int(ptr[1]) + hex_int(ptr[2]);
         g = 16*hex_int(ptr[3]) + hex_int(ptr[4]);
         b = 16*hex_int(ptr[5]) + hex_int(ptr[6]);
         if (zsign == 1)
            i = (r+g+b)/(255.0+255.0+255.0);
         else
            i = 1.0 - (r+g+b)/(255.0+255.0+255.0);
         *zn = i * (v->nz - 1);
         }
      else {
         //
         // hex encoding not found, set to bottom
         //
         *zn = 0;
         }
      }
   else {
      //
      // stroke not found, set to bottom
      //
      *zn = 0;
      }
   }

void set_transform(char *start, int *transform, double transforms[SVG_MAX_TRANSFORM][6]) {
   double a,b,c,d,e,f;
   double pi = 4*atan(1.0);
   char *end,*next,*ptr;
   //
   // push transform for current element onto stack
   //
   end = strstr(start+1,">");
   next = strstr(start+1,"<");
   if ((next != NULL) && (next < end))
      end = next;
   //
   // check for transform
   //
   ptr = strstr(start,"transform");
   if ((ptr != NULL) && (ptr < end)) {
      //
      // found, check for matrix
      //
      ptr = strstr(start,"matrix(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: matrix\n");
         next_number(&ptr,&a);
         next_number(&ptr,&b);
         next_number(&ptr,&c);
         next_number(&ptr,&d);
         next_number(&ptr,&e);
         next_number(&ptr,&f);
         transforms[*transform][0] = a;
         transforms[*transform][1] = b;
         transforms[*transform][2] = c;
         transforms[*transform][3] = d;
         transforms[*transform][4] = e;
         transforms[*transform][5] = f;
         *transform += 1;
         return;
         }
      //
      // check for translate
      //
      ptr = strstr(start,"translate(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: translate\n");
         next_number(&ptr,&e);
         next_number(&ptr,&f);
         transforms[*transform][0] = 1;
         transforms[*transform][1] = 0;
         transforms[*transform][2] = 0;
         transforms[*transform][3] = 1;
         transforms[*transform][4] = e;
         transforms[*transform][5] = f;
         *transform += 1;
         return;
         }
      //
      // check for scale
      //
      ptr = strstr(start,"scale(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: scale\n");
         next_number(&ptr,&a);
         transforms[*transform][0] = a;
         transforms[*transform][1] = 0;
         transforms[*transform][2] = 0;
         transforms[*transform][3] = a;
         transforms[*transform][4] = 0;
         transforms[*transform][5] = 0;
         *transform += 1;
         return;
         }
      //
      // check for rotate
      //
      ptr = strstr(start,"rotate(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: rotate\n");
         next_number(&ptr,&a);
         transforms[*transform][0] = cos(a*pi/180.0);
         transforms[*transform][1] = sin(a*pi/180.0);
         transforms[*transform][2] = -sin(a*pi/180.0);
         transforms[*transform][3] = cos(a*pi/180.0);
         transforms[*transform][4] = 0;
         transforms[*transform][5] = 0;
         *transform += 1;
         return;
         }
      //
      // check for skewX
      //
      ptr = strstr(start,"skewX(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: skewX\n");
         next_number(&ptr,&a);
         transforms[*transform][0] = 1;
         transforms[*transform][1] = 0;
         transforms[*transform][2] = tan(a*pi/180.0);
         transforms[*transform][3] = 1;
         transforms[*transform][4] = 0;
         transforms[*transform][5] = 0;
         *transform += 1;
         return;
         }
      //
      // check for skewY
      //
      ptr = strstr(start,"skewY(");
      if ((ptr != NULL) && (ptr < end)) {
         //printf("trans: skewY\n");
         next_number(&ptr,&a);
         transforms[*transform][0] = 1;
         transforms[*transform][1] = tan(a*pi/180.0);
         transforms[*transform][2] = 0;
         transforms[*transform][3] = 1;
         transforms[*transform][4] = 0;
         transforms[*transform][5] = 0;
         *transform += 1;
         return;
         }
      //
      // didn't find transform
      //
      printf("svg_path: oops -- didn't find transform\n");
      transforms[*transform][0] = 1;
      transforms[*transform][1] = 0;
      transforms[*transform][2] = 0;
      transforms[*transform][3] = 1;
      transforms[*transform][4] = 0;
      transforms[*transform][5] = 0;
      *transform += 1;
      return;
      }
   else {
      //
      // transform not found, set unit transform
      //
      transforms[*transform][0] = 1;
      transforms[*transform][1] = 0;
      transforms[*transform][2] = 0;
      transforms[*transform][3] = 1;
      transforms[*transform][4] = 0;
      transforms[*transform][5] = 0;
      *transform += 1;
      return;
      }
   }

void path_point(int *xn, int *yn, double x, double y, int transform, double transforms[SVG_MAX_TRANSFORM][6],
   double xnscale, double ynscale, double xnmid, double vxmid, double ynmid, double vymid) {
   double xt,yt,xtemp,ytemp;
   int t;
   //
   // return path point
   //
   xt = x;
   yt = y;
   for (t = (transform-1); t >= 0; --t) {
      xtemp = transforms[t][0]*xt + transforms[t][2]*yt + transforms[t][4];
      ytemp = transforms[t][1]*xt + transforms[t][3]*yt + transforms[t][5];
      xt = xtemp;
      yt = ytemp;
      }
   *xn = xnmid + xnscale*(xt - vxmid);
   *yn = ynmid + ynscale*(yt - vymid);
   }

void fab_read_svg(struct fab_vars *v, char *input_file_name, float scale, int points, int resolution, float zmin, float zmax) {
   //
   // read SVG into fab_vars
   //
	FILE *input_file;
   char buf[SVG_MAX_FILE];
   char units[3];
   char current_element,last_element;
   int point,ret;
   char *ptr,*start,*stop,*endptr;
   int transform;
   double transforms[SVG_MAX_TRANSFORM][6];
   double unit_scale;
   double xnscale,ynscale,xnmid,ynmid;
   double vxmin,vymin,vxmid,vymid,vwidth,vheight,vaspect;
   double width,height,aspect;
   double x,y,z,x0,y0,x1,y1,x2,y2;
   double rx,ry,theta,theta_0,theta_1,theta_diff,rotation,large_arc,sweep;
   double phi,x1p,y1p,sign,cxp,cyp;
   double ax,bx,cx,ay,by,cy,xt,yt,t,r;
   double pi;
   int xn,yn,zn;
   int zsign;
   int count;
   pi = 4*atan(1.0);
   //
   // read SVG file
   //
   input_file = fopen(input_file_name, "rb");
   if (input_file == 0) {
      printf("svg_path.c: oops -- can't open %s\n",input_file_name);
      exit(-1);
      }
   ret = fread(buf,1,SVG_MAX_FILE,input_file);
   if (ret >= SVG_MAX_FILE) {
      printf("svg_path: oops -- exceeded SVG buffer size\n");
      exit(-1);
      }
   endptr = buf + ret;
   fclose(input_file);
   printf("read %d bytes from %s\n",ret,input_file_name);
   //
   // find SVG element
   //
   ptr = strstr(buf,"<svg");
   if (ptr == NULL) {
      printf("svg_path: oops -- no SVG element\n");
      exit(-1);
      }
   stop = strstr(ptr,">");
   //
   // check for width and height
   //
   start = strstr(ptr,"width=");
   if ((start == NULL) || (start > stop)) {
      printf("svg_path: oops -- no width\n");
      exit(-1);
      }
   parse_number_units(start,&width,&unit_scale,units);
   printf("   width %f %s\n",width,units);
   start = strstr(ptr,"height=");
   if ((start == NULL) || (start > stop)) {
      printf("svg_path: oops -- no height\n");
      exit(-1);
      }
   parse_number_units(start,&height,&unit_scale,units);
   printf("   height %f %s\n",height,units);
   //
   // check for viewBox
   //
   start = strstr(ptr,"viewBox=");
   if ((start != NULL) && (start < stop)) {
      next_number(&start,&vxmin);
      next_number(&start,&vymin);
      next_number(&start,&vwidth);
      next_number(&start,&vheight);
      printf("   view x min %f\n",vxmin);
      printf("   view y min %f\n",vymin);
      printf("   view width %f\n",vwidth);
      printf("   view height %f\n",vheight);
      aspect = height/width;
      vaspect = vheight/vwidth;
      vxmid = vxmin + vwidth/2.0;
      vymid = vymin + vheight/2.0;
      // assume xMidYMid meet scaling
      if (vaspect > aspect) {
         ynscale = resolution*aspect/vheight;
         xnscale = ynscale;
         }
      else {
         xnscale = resolution/vwidth;
         ynscale = xnscale;
         }
      xnmid = resolution/2.0;
      ynmid = aspect*xnmid;
      }
   else {
      vxmid = width/2.0;
      vymid = height/2.0;
      xnscale = resolution/width;
      ynscale = xnscale;
      xnmid = xnscale*width/2.0;
      ynmid = ynscale*height/2.0;
      }
   //
   // start path
   //
   v->nx = 2*xnmid;
   v->ny = 2*ynmid;
   v->dx = scale*width*unit_scale;
   v->dy = scale*height*unit_scale;
   v->xmin = 0;
   v->ymin = 0;
   zsign = 1;
   if (v->path->dof > 2) {
      if (zmin > zmax) {
         z = zmin;
         zmin = zmax;
         zmax = z;
         zsign = -1;
         }
      v->nz = 1+xnscale*(zmax-zmin);
      v->dz = zmax-zmin;
      v->zmin = zmin;
      }
   //
   // find graphic elements
   //
   ptr = buf;
   transform = 0;
   do {
      ptr += 1;
      if (strncmp(ptr,"<g",2) == 0) {
         //
         // opening g, check for transform and intensity
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         }
      else if (strncmp(ptr,"/g>",2) == 0) {
         //
         // closing g, clear transform
         //
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<defs",5) == 0) {
         //
         // defs
         //
         while (1) {
            //
            // find closure
            //
            ptr +=1;
            if (strncmp(ptr,"/>",2) == 0)
               //
               // self-closing
               //
               break;
            else if (strncmp(ptr,">",1) == 0) {
               //
               // not self-closing
               //
               while (1) {
                  ptr += 1;
                  if (strncmp(ptr,"</defs>",7) == 0)
                     break;
                  }
               break;
               }
            }
         printf("   svg_path: defs not yet implemented\n");
         }
      else if (strncmp(ptr,"<image",6) == 0) {
         //
         // defs
         //
         while (1) {
            //
            // find closure
            //
            ptr +=1;
            if (strncmp(ptr,"/>",2) == 0)
               //
               // self-closing
               //
               break;
            else if (strncmp(ptr,">",1) == 0) {
               //
               // not self-closing
               //
               while (1) {
                  ptr += 1;
                  if (strncmp(ptr,"</image>",8) == 0)
                     break;
                  }
               break;
               }
            }
         printf("   svg_path: image not yet implemented\n");
         }
      else if (strncmp(ptr,"<path",5) == 0) {
         //
         // path
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         ptr = 4+strstr(ptr," d=");
         current_element = 0;
         last_element = 0;
         x0 = 0;
         y0 = 0;
         do {
            last_element = current_element;
            current_element = next_element(&ptr, current_element);
            if (current_element == 'm') {
               //
               // relative moveto
               //
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               x0 = x0 + x;
               y0 = y0 + y;
               //printf("   path m: %f %f\n",x0,y0);
               current_element = 'l';
               fab_path_segment(v);
               fab_path_point(v);
               path_point(&xn,&yn,x0,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               }
            else if (current_element == 'M') {
               //
               // absolute moveto
               //
               next_number(&ptr,&x0);
               next_number(&ptr,&y0);
               //printf("  path M: %f %f\n",x0,y0);
               current_element = 'L';
               fab_path_segment(v);
               fab_path_point(v);
               path_point(&xn,&yn,x0,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               }
            else if (current_element == 'l') {
               //
               // relative lineto
               //
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               //printf("   path l: %f %f\n",x,y);
               current_element = 'l';
               fab_path_point(v);
               path_point(&xn,&yn,x0+x,y0+y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               x0 = x0+x;
               y0 = y0+y;
               }
            else if (current_element == 'L') {
               //
               // absolute lineto
               //
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               //printf("   path L: %f %f\n",x,y);
               current_element = 'L';
               fab_path_point(v);
               path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               x0 = x;
               y0 = y;
               }
            else if (current_element == 'h') {
               //
               // relative horizontal
               //
               next_number(&ptr,&x);
               //printf("   path h: %f\n",x);
               current_element = 'j';
               fab_path_point(v);
               path_point(&xn,&yn,x0+x,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               x0 = x0+x;
               }
            else if (current_element == 'H') {
               //
               // absolute horizontal
               //
               next_number(&ptr,&x);
               //printf("   path H: %f\n",x);
               current_element = 'H';
               fab_path_point(v);
               path_point(&xn,&yn,x,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               x0 = x;
               }
            else if (current_element == 'v') {
               //
               // relative vertical
               //
               next_number(&ptr,&y);
               //printf("   path v: %f\n",y);
               current_element = 'v';
               fab_path_point(v);
               path_point(&xn,&yn,x0,y0+y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               y0 = y0+y;
               }
            else if (current_element == 'V') {
               //
               // absolute vertical
               //
               next_number(&ptr,&y);
               //printf("   path V: %f\n",y);
               current_element = 'V';
               fab_path_point(v);
               path_point(&xn,&yn,x0,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               y0 = y;
               }
            else if (current_element == 'c') {
               //
               // relative curveto
               //
               next_number(&ptr,&x1);
               x1 += x0;
               next_number(&ptr,&y1);
               y1 += y0;
               next_number(&ptr,&x2);
               x2 += x0;
               next_number(&ptr,&y2);
               y2 += y0;
               next_number(&ptr,&x);
               x += x0;
               next_number(&ptr,&y);
               y += y0;
               //printf("   path c: %f %f %f %f %f %f\n",x1,y1,x2,y2,x,y);
               current_element = 'c';
               cx = 3 * (x1 - x0);
               bx = 3 * (x2 - x1) - cx;
               ax = x - x0 - cx - bx;
               cy = 3 * (y1 - y0);
               by = 3 * (y2 - y1) - cy;
               ay = y - y0 - cy - by;
               for (point = 0; point < points; ++point) {
                  t = point / (points - 1.0);
                  xt = ax*t*t*t + bx*t*t + cx*t + x0;
                  yt = ay*t*t*t + by*t*t + cy*t + y0;
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if (current_element == 'C') {
               //
               // absolute curveto
               //
               next_number(&ptr,&x1);
               next_number(&ptr,&y1);
               next_number(&ptr,&x2);
               next_number(&ptr,&y2);
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               //printf(" path C: %f %f %f %f %f %f\n",x1,y1,x2,y2,x,y);
               current_element = 'C';
               cx = 3 * (x1 - x0);
               bx = 3 * (x2 - x1) - cx;
               ax = x - x0 - cx - bx;
               cy = 3 * (y1 - y0);
               by = 3 * (y2 - y1) - cy;
               ay = y - y0 - cy - by;
               for (point = 0; point < points; ++point) {
                  t = point / (points - 1.0);
                  xt = ax*t*t*t + bx*t*t + cx*t + x0;
                  yt = ay*t*t*t + by*t*t + cy*t + y0;
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if (current_element == 's') {
               //
               // relative smooth curveto
               //
               if ((last_element == 'c') || (last_element == 'C') || (last_element == 's') || (last_element == 'S')) {
                  x1 = x0 + (x0-x2);
                  y1 = y0 + (y0-y2);
                  }
               else {
                  x1 = x0;
                  y1 = y0;
                  }
               next_number(&ptr,&x2);
               x2 += x0;
               next_number(&ptr,&y2);
               y2 += y0;
               next_number(&ptr,&x);
               x += x0;
               next_number(&ptr,&y);
               y += y0;
               //printf("   path s: %f %f %f %f\n",x2,y2,x,y);
               current_element = 'c';
               cx = 3 * (x1 - x0);
               bx = 3 * (x2 - x1) - cx;
               ax = x - x0 - cx - bx;
               cy = 3 * (y1 - y0);
               by = 3 * (y2 - y1) - cy;
               ay = y - y0 - cy - by;
               for (point = 0; point < points; ++point) {
                  t = point / (points - 1.0);
                  xt = ax*t*t*t + bx*t*t + cx*t + x0;
                  yt = ay*t*t*t + by*t*t + cy*t + y0;
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if (current_element == 'S') {
               //
               // absolute smooth curveto
               //
               if ((last_element == 'c') || (last_element == 'C') || (last_element == 's') || (last_element == 'S')) {
                  x1 = x0 + (x0-x2);
                  y1 = y0 + (y0-y2);
                  }
               else {
                  x1 = x0;
                  y1 = y0;
                  }
               next_number(&ptr,&x2);
               next_number(&ptr,&y2);
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               //printf(" path S: %f %f %f %f\n",x2,y2,x,y);
               current_element = 'C';
               cx = 3 * (x1 - x0);
               bx = 3 * (x2 - x1) - cx;
               ax = x - x0 - cx - bx;
               cy = 3 * (y1 - y0);
               by = 3 * (y2 - y1) - cy;
               ay = y - y0 - cy - by;
               for (point = 0; point < points; ++point) {
                  t = point / (points - 1.0);
                  xt = ax*t*t*t + bx*t*t + cx*t + x0;
                  yt = ay*t*t*t + by*t*t + cy*t + y0;
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if (current_element == 'a') {
               //
               // relative arc
               //
               next_number(&ptr,&rx);
               next_number(&ptr,&ry);
               next_number(&ptr,&rotation);
               next_number(&ptr,&large_arc);
               next_number(&ptr,&sweep);
               next_number(&ptr,&x);
               x += x0;
               next_number(&ptr,&y);
               y += y0;
               //printf("   path a: %f %f %f %f %f %f %f\n",rx,ry,rotation,large_arc,sweep,x,y);
               current_element = 'a';
               phi = rotation*pi/180.0;
               x1 = x0;
               x2 = x;
               y1 = y0;
               y2 = y;
               x1p = cos(phi)*(x1-x2)/2.0 + sin(phi)*(y1-y2)/2.0;
               y1p = -sin(phi)*(x1-x2)/2.0 + cos(phi)*(y1-y2)/2.0;
               sign = ((large_arc == sweep) ? -1 : 1);
               if ((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p) < 0) {
                  cxp = 0;
                  cyp = 0;
                  }
               else {
                  cxp = sign * sqrt((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p)/(rx*rx*y1p*y1p + ry*ry*x1p*x1p)) * rx*y1p/ry;
                  cyp = -1 * sign * sqrt((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p)/(rx*rx*y1p*y1p + ry*ry*x1p*x1p)) * ry*x1p/rx;
                  }
               cx = cos(phi)*cxp - sin(phi)*cyp + (x1+x2)/2.0;
               cy = sin(phi)*cxp + cos(phi)*cyp + (y1+y2)/2.0;
               theta_0 = angle(1,0,(x1p-cxp)/rx,(y1p-cyp)/ry);
               theta_diff = angle ((x1p-cxp)/rx,(y1p-cyp)/ry,(-x1p-cxp)/rx,(-y1p-cyp)/ry);
               theta_1 = theta_0 + theta_diff;
               if (large_arc == 1) {
                  if (sweep == 0)
                     theta_1 -= 2*pi;
                  else
                     theta_1 += 2*pi;
                  }
               for (point = 0; point < points; ++point) {
                  theta = theta_0 + (theta_1-theta_0) * point / (points - 1.0);
                  xt = cx + rx*cos(theta);
                  yt = cy + ry*sin(theta);
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if (current_element == 'A') {
               //
               // absolute arc
               //
               next_number(&ptr,&rx);
               next_number(&ptr,&ry);
               next_number(&ptr,&rotation);
               next_number(&ptr,&large_arc);
               next_number(&ptr,&sweep);
               next_number(&ptr,&x);
               next_number(&ptr,&y);
               //printf("   path A: %f %f %f %f %f %f %f\n",rx,ry,rotation,large_arc,sweep,x,y);
               current_element = 'A';
               phi = rotation*pi/180.0;
               x1 = x0;
               x2 = x;
               y1 = y0;
               y2 = y;
               x1p = cos(phi)*(x1-x2)/2.0 + sin(phi)*(y1-y2)/2.0;
               y1p = -sin(phi)*(x1-x2)/2.0 + cos(phi)*(y1-y2)/2.0;
               sign = ((large_arc == sweep) ? -1 : 1);
               if ((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p) < 0) {
                  cxp = 0;
                  cyp = 0;
                  }
               else {
                  cxp = sign * sqrt((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p)/(rx*rx*y1p*y1p + ry*ry*x1p*x1p)) * rx*y1p/ry;
                  cyp = -1 * sign * sqrt((rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p)/(rx*rx*y1p*y1p + ry*ry*x1p*x1p)) * ry*x1p/rx;
                  }
               cx = cos(phi)*cxp - sin(phi)*cyp + (x1+x2)/2.0;
               cy = sin(phi)*cxp + cos(phi)*cyp + (y1+y2)/2.0;
               theta_0 = angle(1,0,(x1p-cxp)/rx,(y1p-cyp)/ry);
               theta_diff = angle ((x1p-cxp)/rx,(y1p-cyp)/ry,(-x1p-cxp)/rx,(-y1p-cyp)/ry);
               theta_1 = theta_0 + theta_diff;
               if (large_arc == 1) {
                  if (sweep == 0)
                     theta_1 -= 2*pi;
                  else
                     theta_1 += 2*pi;
                  }
               for (point = 0; point < points; ++point) {
                  theta = theta_0 + (theta_1-theta_0) * point / (points - 1.0);
                  xt = cx + rx*cos(theta);
                  yt = cy + ry*sin(theta);
                  fab_path_point(v);
                  path_point(&xn,&yn,xt,yt,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
                  fab_path_axis(v,xn);
                  fab_path_axis(v,yn);
                  if (v->path->dof > 2)
                     fab_path_axis(v,zn);
                  }
               x0 = x;
               y0 = y;
               }
            else if ((current_element == 'z') || (current_element == 'Z')) {
               //
               // closepath
               //
               //printf("   path zZ\n");
               fab_path_point(v);
               fab_path_axis(v,v->path->segment->first->first->value);
               fab_path_axis(v,v->path->segment->first->first->next->value);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
               current_element = 0;
               ptr += 1;
               }
            } while (*ptr != '\"');
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<rect",5) == 0) {
         //
         // rectangle
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         start = strstr(ptr,"width=");
         next_number(&start,&width);
         start = strstr(ptr,"height=");
         next_number(&start,&height);
         start = strstr(ptr,"x=");
         next_number(&start,&x);
         start = strstr(ptr,"y=");
         next_number(&start,&y);
         while (strncmp(ptr,"/>",2) != 0) ptr += 1; // read to end of element
         //printf("   rect: %f %f %f %f\n",width,height,x,y);
         fab_path_segment(v);
            fab_path_point(v);
               path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
            fab_path_point(v);
               path_point(&xn,&yn,x,y+height,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
            fab_path_point(v);
               path_point(&xn,&yn,x+width,y+height,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
            fab_path_point(v);
               path_point(&xn,&yn,x+width,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
            fab_path_point(v);
               path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
               fab_path_axis(v,xn);
               fab_path_axis(v,yn);
               if (v->path->dof > 2)
                  fab_path_axis(v,zn);
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<circle",7) == 0) {
         //
         // circle 
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         start = strstr(ptr,"cx=");
         next_number(&start,&cx);
         start = strstr(ptr,"cy=");
         next_number(&start,&cy);
         start = strstr(ptr,"r=");
         next_number(&start,&r);
         while (strncmp(ptr,"/>",2) != 0) ptr += 1; // read to end of element
         //printf("   circle: %f %f %f\n",cx,cy,r);
         fab_path_segment(v);
         for (point = 0; point < points; ++point) {
            fab_path_point(v);
            x = cx + r*cos(point*2*pi/(points-1.0));
            y = cy + r*sin(point*2*pi/(points-1.0));
            path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
            fab_path_axis(v,xn);
            fab_path_axis(v,yn);
            if (v->path->dof > 2)
               fab_path_axis(v,zn);
            }
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<ellipse",8) == 0) {
         //
         // ellipse 
         //
         while (strncmp(ptr,"/>",2) != 0) ptr += 1; // read to end of element
         printf("   svg_path: ellipse not yet implemented\n");
         }
      else if (strncmp(ptr,"<line",5) == 0) {
         //
         // line
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         start = 3+strstr(ptr,"x1=");
         next_number(&start,&x1);
         start = 3+strstr(ptr,"y1=");
         next_number(&start,&y1);
         start = 3+strstr(ptr,"x2=");
         next_number(&start,&x2);
         start = 3+strstr(ptr,"y2=");
         next_number(&start,&y2);
         while (strncmp(ptr,"/>",2) != 0) ptr += 1; // read to end of element
         //printf("   line: %f %f %f %f\n",x1,y1,x2,y2);
         fab_path_segment(v);
         fab_path_point(v);
         path_point(&xn,&yn,x1,y1,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
         fab_path_axis(v,xn);
         fab_path_axis(v,yn);
         if (v->path->dof > 2)
            fab_path_axis(v,zn);
         fab_path_point(v);
         path_point(&xn,&yn,x2,y2,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
         fab_path_axis(v,xn);
         fab_path_axis(v,yn);
         if (v->path->dof > 2)
            fab_path_axis(v,zn);
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<polyline",9) == 0) {
         //
         // polyline
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         ptr = 8+strstr(ptr,"points=");
         //
         // move to first point
         //
         next_number(&ptr,&x);
         next_number(&ptr,&y);
         fab_path_segment(v);
         fab_path_point(v);
         path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
         fab_path_axis(v,xn);
         fab_path_axis(v,yn);
         if (v->path->dof > 2)
            fab_path_axis(v,zn);
         while (1) {
            //
            // loop over remaining points 
            //
            next_number(&ptr,&x);
            next_number(&ptr,&y);
            fab_path_point(v);
            path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
            fab_path_axis(v,xn);
            fab_path_axis(v,yn);
            if (v->path->dof > 2)
               fab_path_axis(v,zn);
            while ((*ptr == ' ') || (*ptr == '"') || (*ptr == 13) || (*ptr == 10)) ptr += 1; // skip space
            if (strncmp(ptr,"/>",2) == 0) break; // check for end
            }
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<polygon",8) == 0) {
         //
         // polygon
         //
         set_transform(ptr,&transform,transforms);
         set_intensity(ptr,&zn,v,zsign);
         ptr = 8+strstr(ptr,"points=");
         //
         // move to first point
         //
         next_number(&ptr,&x0);
         next_number(&ptr,&y0);
         fab_path_segment(v);
         fab_path_point(v);
         path_point(&xn,&yn,x0,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
         fab_path_axis(v,xn);
         fab_path_axis(v,yn);
         if (v->path->dof > 2)
            fab_path_axis(v,zn);
         while (1) {
            //
            // loop over remaining points 
            //
            next_number(&ptr,&x);
            next_number(&ptr,&y);
            fab_path_point(v);
            path_point(&xn,&yn,x,y,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
            fab_path_axis(v,xn);
            fab_path_axis(v,yn);
            if (v->path->dof > 2)
               fab_path_axis(v,zn);
            while ((*ptr == ' ') || (*ptr == '"') || (*ptr == 13) || (*ptr == 10)) ptr += 1; // skip space
            if (strncmp(ptr,"/>",2) == 0) break; // check for end
            }
         fab_path_point(v);
         path_point(&xn,&yn,x0,y0,transform,transforms,xnscale,ynscale,xnmid,vxmid,ynmid,vymid);
         fab_path_axis(v,xn);
         fab_path_axis(v,yn);
         if (v->path->dof > 2)
            fab_path_axis(v,zn);
         clear_transform(&transform);
         }
      else if (strncmp(ptr,"<text",5) == 0) {
         //
         // text 
         //
         while (strncmp(ptr,"/text>",6) != 0) ptr += 1; // read to end of element
         printf("   svg_path: text not implemented\n");
         }
      } while ((endptr-ptr) > 1);
   }

int main(int argc, char **argv) {
   //
   // local vars
   //
   struct fab_vars v;
   init_vars(&v);
   int points,resolution;
   float scale,zmin,zmax;
   //
   // command line args
   // Largo al Factotum Canadian Brass
   //
   if ((argc != 3) && (argc != 4) && (argc != 5) && (argc != 6) && (argc != 7) && (argc != 8)) {
      printf("command line: svg_path in.svg out.path [scale [points [resolution [zmin [zmax]]]]]\n");
      printf("   in.svg = input binary SVG file\n");
      printf("   out.path = output path file\n");
      printf("   scale = scale factor (optional, default 1.0)\n");
      printf("   points = points per curve segment (optional, default 25)\n");
      printf("   resolution = path x resolution (optional, default 10000)\n");
      printf("   zmin = path min intensity z (optional, mm, default 0)\n");
      printf("   zmax = path max intensity z (optional, mm, default zmin)\n");
      exit(-1);
      }
   if (argc == 3) {
      scale = 1.0;
      points = 25;
      resolution = 10000;
      zmin = 0;
      zmax = 0;
      }
   else if (argc == 4) {
      sscanf(argv[3],"%f",&scale);
      points = 25;
      resolution = 10000;
      zmin = 0;
      zmax = 0;
      }
   else if (argc == 5) {
      sscanf(argv[3],"%f",&scale);
      sscanf(argv[4],"%d",&points);
      resolution = 10000;
      zmin = 0;
      zmax = 0;
      }
   else if (argc == 6) {
      sscanf(argv[3],"%f",&scale);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%d",&resolution);
      zmin = 0;
      zmax = 0;
      }
   else if (argc == 7) {
      sscanf(argv[3],"%f",&scale);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%d",&resolution);
      sscanf(argv[6],"%f",&zmin);
      zmax = zmin;
      }
   else if (argc == 8) {
      sscanf(argv[3],"%f",&scale);
      sscanf(argv[4],"%d",&points);
      sscanf(argv[5],"%d",&resolution);
      sscanf(argv[6],"%f",&zmin);
      sscanf(argv[7],"%f",&zmax);
      }
   //
   //  read SVG
   //
   if (argc <= 6)
      fab_path_start(&v,2);
   else
      fab_path_start(&v,3);
   fab_read_svg(&v,argv[1],scale,points,resolution,zmin,zmax);
   //
   // write path
   //
   fab_write_path(&v,argv[2]);
   //
   // return
   //
   return(0);
   }
