# cad_shapes.py
# Standard library of shapes and operations

# Assembled by Matt Keeter (with code from Neil Gershenfeld)
# matt.keeter@cba.mit.edu

# kokompe.cba.mit.edu

###############################################################################

# 2D shapes:
#   circle(x0, y0, r)
#   rectangle(x0, x1, y0, y1)
#   right_triangle(x0, y0, l)
#   triangle(x0, y0, x1, y1, x2, y2) [clockwise order]
#   polygon(x, y, r, n)
#   tab(x, y, width, height, angle = 0, chamfer = 0.2)

# 3D shapes:
#   extrusion(part, z0, z1)
#   cylinder(x0, y0, z0, z1, r)
#   sphere(x0, y0, z0, r)
#   cube(x0, x1, y0, y1, z0, z1)
#   cone(x0, y0, z0, z1, r0)
#   pyramid(x0, x1, y0, y1, z0, z1)

# Logic operations:
#   add(part1, part2)
#   subtract(part1, part2)
#   intersect(part1, part2)
#   invert(part)

# Translation:
#   move(part, dx, dy, dz = 0)

# Rotation:
#   rotate(part, angle)
#   rotate_90(part)
#   rotate_180(part)
#   rotate_270(part)
#   rotate_x(part, angle)
#   rotate_y(part, angle)
#   rotate_z(part, angle)

# Reflection:
#   reflect_x(part,x0 = 0)
#   reflect_y(part,y0 = 0)
#   reflect_z(part,z0 = 0)
#   reflect_xy(part)
#   reflect_xz(part)
#   reflect_yz(part)

# Scaling:
#   scale_x(part, x0, sx)
#   scale_y(part, y0, sy)
#   scale_z(part, z0, sz)
#   scale_xy(part, x0, y0, sxy)
#   scale_xyz(part, x0, y0, z0, sxyz)

# Distortion:
#   attract(part, value, x0, y0, z0=0)

# Coscaling:
#   coscale_x_y(part, x0, y0, y1, angle0, angle1, amplitude, offset)
#   coscale_x_z(part, x0, z0, z1, angle0, angle1, amplitude, offset)
#   coscale_xy_z(part, x0, y0, z0, z1, angle0, angle1, amplitude, offset)

# Tapering:
#   taper_x_y(part, x0, y0, y1, s0, s1)
#   taper_x_z(part, x0, z0, z1, s0, s1)
#   taper_xy_z(part, x0, y0, z0, z1, s0, s1)

# Shearing:
#   shear_x_y(part, y0, y1, dx0, dx1)
#   shear_x_z(part, z0, z1, dx0, dx1)
#   coshear_x_z(part, z0, z1, angle0, angle1, amplitude, offset)

# Color: 
#   color(color,part)
#
#   Colors are implemented as an integer bit-field:
#       R = bits 0 through 7
#       G = bits 8 through 15
#       B = bits 16 through 23
#
#   The following colors are pre-defined:
#       red, green, blue, gray, white, teal, pink,
#       yellow, brown, navy, black

###############################################################################
from math_string import MString

from math import pi, sin, cos
import re

###############################################################################
#   2D Shapes
###############################################################################
def circle(x0, y0, r):
    return move(MString("(pow(X,2) + pow(Y,2) <= %f)" % (r*r)), x0, y0)

def rectangle(x0, x1, y0, y1):
    return MString("((X >= %f) && (X <= %f) && " % (x0, x1) +
                   "(Y >= %f) && (Y <= %f))"     % (y0, y1))

def right_triangle(x, y, L):
    tri = MString("(X < -Y) && (X > -{0}) && (Y > -{0})".format(L))
    return move(tri, x + L, y + L)
    
def triangle(x0, y0, x1, y1, x2, y2): # points in clockwise order
    tri = ("((((({y1})-({y0}))*(X-({x0}))-(({x1})-({x0}))*(Y-({y0}))) >= 0) && " + \
           "(((({y2})-({y1}))*(X-({x1}))-(({x2})-({x1}))*(Y-({y1}))) >= 0) && " + \
           "(((({y0})-({y2}))*(X-({x2}))-(({x0})-({x2}))*(Y-({y2}))) >= 0))").format(
                x0 = x0, y0 = y0, x1 = x1, y1 = y1, x2 = x2, y2 = y2)
    return MString(tri)

def polygon(x, y, r, n):
    if n <= 2:
        return '0'
    part = circle(0, 0, r)
    cutoff = 'Y > -%f' % (cos(pi/n) * r)
    for i in range(n):
        part = intersect(part, rotate(cutoff, i*360./n))
    return move(MString(part), x, y)
    
    
def tab(x, y, width, height, angle = 0, chamfer = 0.2):
    tab = rectangle(-width/2, width/2, 0, height)
    cutout = triangle(width/2 - chamfer*height, height,
                      width/2, height,
                      width/2, height - chamfer*height)
    cutout = add(cutout, reflect_x(cutout))
    tab = subtract(tab, cutout)
    
    tab = rotate(tab, angle)
    tab = move(tab, x, y)
    return MString(tab)

def slot(x, y, width, height, angle = 0, chamfer = 0.2):
    slot = rectangle(-width/2, width/2, -height, 0)
    inset = triangle(width/2, 0,
                     width/2 + height * chamfer, 0,
                     width/2, -chamfer*height)
    inset = add(inset, reflect_x(inset))
    slot = add(slot, inset)
    
    slot = rotate(slot, angle)
    slot = move(slot, x, y)
    return MString(slot)

###############################################################################
#   3D Shapes
###############################################################################
def extrusion(part, z0, z1):
    return MString("((%s) && (Z >= %f) && (Z <= %f))" % (part, z0, z1))

def cylinder(x0, y0, z0, z1, r):
    return extrusion(circle(x0, y0, r), z0, z1)

def sphere(x0, y0, z0, r):
    return move("(pow(X,2) + pow(Y,2) + pow(Z,2) <= %f)" % (r * r), x0, y0, z0)

def cube(x0, x1, y0, y1, z0, z1):
    return MString("((X >= %f) && (X <= %f) && " % (x0, x1) +
                   " (Y >= %f) && (Y <= %f) && " % (y0, y1) +\
                   " (Z >= %f) && (Z <= %f))"    % (z0, z1))

def cone(x0, y0, z0, z1, r0):
    cyl = cylinder(x0, y0, z0, z1, r0)
    return taper_xy_z(cyl, x0, y0, z0, z1, 1.0, 0.0)

def pyramid(x0, x1, y0, y1, z0, z1):
   c = cube(x0, x1, y0, y1, z0, z1)
   return taper_xy_z(c, (x0+x1)/2., (y0+y1)/2., z0, z1, 1.0, 0.0)

###############################################################################
#   Logic Operations
###############################################################################
def add(part1, part2):
    return MString("(%s) || (%s)" % (part1, part2))

def subtract(part1, part2):
    return MString("(%s) && !(%s)" % (part1, part2))

def intersect(part1, part2):
    return MString("(%s) && (%s)" % (part1, part2))
    
def invert(part):
    return MString("!(%s)" % part)

###############################################################################
#   Translation
###############################################################################
def move(part,dx,dy,dz = 0):
    if dx:
        part = part.replace('X','(X-%f)' % dx)
    if dy:
        part = part.replace('Y','(Y-%f)' % dy)
    if dz:
        part = part.replace('Z','(Z-%f)' % dz)
    return part    

###############################################################################
#   Rotation
###############################################################################
def rotate(part, angle):
    if angle == 90:
        return rotate_90(part)
    elif angle == 180:
        return rotate_180(part)
    elif angle == 270:
        return rotate_270(part)
    elif angle == 0:
        return part

    angle = str(angle*pi/180)
    
    # find all already-rotated points and increment that
    # rotation instead of stacking more and more trig
    # functions
    xrot_regex = re.compile(r'\( cos\(([\-\d.]+)\)\*X\+sin\(\1\)\*Y\)')
    yrot_regex = re.compile(r'\(-sin\(([\-\d.]+)\)\*X\+cos\(\1\)\*Y\)')
    
    def update_angle_x(match, angle):
        match = match.groups()
        angle = str(float(angle) + float(match[0]))
        return '(cos({0})*x+sin({0})*y)'.format(angle)

    def update_angle_y(match, angle):
        match = match.groups()
        angle = str(float(angle) + float(match[0]))
        return '(-sin({0})*x+cos({0})*y)'.format(angle)

    part = xrot_regex.sub(lambda x: update_angle_x(x, angle), str(part))
    part = yrot_regex.sub(lambda x: update_angle_y(x, angle), str(part))
    
    # replace all bare X, Y as before
    # move all upper to lower case
    part = part.replace('X','(cos({0})*X+sin({0})*y)'.format(angle))
    part = part.replace('Y','(-sin({0})*X+cos({0})*y)'.format(angle))
    part = part.replace('y','Y')
    part = part.replace('x','X')
    
    return MString(part)

def rotate_90(part):
    part = reflect_y(part)
    part = reflect_xy(part)
    return part

def rotate_180(part):
    part = rotate_90(part)
    part = rotate_90(part)
    return part

def rotate_270(part):
    part = rotate_90(part)
    part = rotate_90(part)
    part = rotate_90(part)
    return part

def rotate_x(part, angle):
    angle = angle*pi/180
    part = part.replace('Y','(cos(angle)*Y+sin(angle)*z)')
    part = part.replace('Z','(-sin(angle)*Y+cos(angle)*z)')
    part = part.replace('z','Z')
    part = part.replace('angle',str(angle))
    return part

def rotate_y(part, angle):
    angle = angle*pi/180
    part = part.replace('X','(cos(angle)*X+sin(angle)*z)')
    part = part.replace('Z','(-sin(angle)*X+cos(angle)*z)')
    part = part.replace('z','Z')
    part = part.replace('angle',str(angle))
    return part
   
def rotate_z(part, angle):
    return rotate(part, angle)
###############################################################################
#   Reflection
###############################################################################
def reflect_x(part, x0 = 0):
    return part.replace('X','(%f-X)' % x0)

def reflect_y(part, y0 = 0):
    return part.replace('Y','(%f-Y)' % y0)

def reflect_z(part,z0):
    return part.replace('Z','(%f-Z)' % z0)
    return part

def reflect_xy(part):
    part = part.replace('X','temp')
    part = part.replace('Y','X')
    part = part.replace('temp','Y')
    return part

def reflect_xz(part):
    part = part.replace('X','temp')
    part = part.replace('Z','X')
    part = part.replace('temp','Z')
    return part

def reflect_yz(part):
    part = part.replace('Y','temp')
    part = part.replace('Z','Y')
    part = part.replace('temp','Z')
    return part
###############################################################################
#   Scaling
###############################################################################
def scale_x(part, x0, sx):
   part = part.replace('X','((x0) + (X-(x0))/(sx))')
   part = part.replace('x0',str(x0))
   part = part.replace('sx',str(sx))
   return part

def scale_y(part, y0, sy):
   part = part.replace('Y','((y0) + (Y-(y0))/(sy))')
   part = part.replace('y0',str(y0))
   part = part.replace('sy',str(sy))
   return part

def scale_z(part, z0, sz):
   part = part.replace('Z','((z0) + (Z-(z0))/(sz))')
   part = part.replace('z0',str(z0))
   part = part.replace('sz',str(sz))
   return part

def scale_xy(part, x0, y0, sxy):
   part = part.replace('X','((x0) + (X-(x0))/(sxy))')
   part = part.replace('Y','((y0) + (Y-(y0))/(sxy))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('sxy',str(sxy))
   return part

def scale_xyz(part, x0, y0, z0, sxyz):
   part = part.replace('X','((x0) + (X-(x0))/(sxyz))')
   part = part.replace('Y','((y0) + (Y-(y0))/(sxyz))')
   part = part.replace('Z','((z0) + (Z-(z0))/(sxyz))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('z0',str(z0))
   part = part.replace('sxyz',str(sxyz))
   return part

###############################################################################
#   Distortion:
###############################################################################

def attract(part, radius, x0, y0, z0=0):
    part = part.replace('X',
    ('({x0}+(X-{x0})*(1+{r}*exp(-pow(X-{x0},2)+'+
    'pow(yt-{y0},2)+pow(zt-{z0},2)/{r})))').format(
        x0=x0,y0=y0,z0=z0,r=radius))
    part = part.replace('Y',
    ('({y0}+(Y-{y0})*(1+{r}*exp(-pow(xt-{x0},2)+'+
    'pow(Y-{y0},2)+pow(zt-{z0},2)/{r})))').format(
        x0=x0,y0=y0,z0=z0,r=radius))
    part = part.replace('Z',
    ('({z0}+(Z-{z0})*(1+{r}*exp(-pow(xt-{x0},2)+'+
    'pow(yt-{y0},2)+pow(Z-{z0},2)/{r})))').format(
        x0=x0,y0=y0,z0=z0,r=radius))
    part = part.replace('xt','X')
    part = part.replace('yt','Y')
    part = part.replace('zt','Z')
    return part

###############################################################################
#   Coscaling
###############################################################################

def coscale_x_y(part, x0, y0, y1, angle0, angle1, amplitude, offset):
   phase0 = pi*angle0/180.
   phase1 = pi*angle1/180.
   part = part.replace('X','((x0) + (X-(x0))/((offset) + (amplitude)*cos((phase0) + ((phase1)-(phase0))*(Y-(y0))/((y1)-(y0)))))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('y1',str(y1))
   part = part.replace('phase0',str(phase0))
   part = part.replace('phase1',str(phase1))
   part = part.replace('amplitude',str(amplitude))
   part = part.replace('offset',str(offset))
   return part

def coscale_x_z(part, x0, z0, z1, angle0, angle1, amplitude, offset):
   phase0 = pi*angle0/180.
   phase1 = pi*angle1/180.
   part = part.replace('X','((x0) + (X-(x0))/((offset) + (amplitude)*cos((phase0) + ((phase1)-(phase0))*(Z-(z0))/((z1)-(z0)))))')
   part = part.replace('x0',str(x0))
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('phase0',str(phase0))
   part = part.replace('phase1',str(phase1))
   part = part.replace('amplitude',str(amplitude))
   part = part.replace('offset',str(offset))
   return part

def coscale_xy_z(part, x0, y0, z0, z1, angle0, angle1, amplitude, offset):
   phase0 = pi*angle0/180.
   phase1 = pi*angle1/180.
   part = part.replace('X','((x0) + (X-(x0))/((offset) + (amplitude)*cos((phase0) + ((phase1)-(phase0))*(Z-(z0))/((z1)-(z0)))))')
   part = part.replace('Y','((y0) + (Y-(y0))/((offset) + (amplitude)*cos((phase0) + ((phase1)-(phase0))*(Z-(z0))/((z1)-(z0)))))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('phase0',str(phase0))
   part = part.replace('phase1',str(phase1))
   part = part.replace('amplitude',str(amplitude))
   part = part.replace('offset',str(offset))
   return part

###############################################################################
#   Tapering
###############################################################################
def taper_x_y(part, x0, y0, y1, s0, s1):
   part = part.replace('X','((x0) + (X-(x0))*((y1)-(y0))/((s1)*(Y-(y0)) + (s0)*((y1)-Y)))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('y1',str(y1))
   part = part.replace('s0',str(s0))
   part = part.replace('s1',str(s1))
   return part

def taper_x_z(part, x0, z0, z1, s0, s1):
   part = part.replace('X','((x0) + (X-(x0))*((z1)-(z0))/((s1)*(Z-(z0)) + (s0)*((z1)-Z)))')
   part = part.replace('x0',str(x0))
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('s0',str(s0))
   part = part.replace('s1',str(s1))
   return part

def taper_xy_z(part, x0, y0, z0, z1, s0, s1):
   part = part.replace('X','((x0) + (X-(x0))*((z1)-(z0))/((s1)*(Z-(z0)) + (s0)*((z1)-Z)))')
   part = part.replace('Y','((y0) + (Y-(y0))*((z1)-(z0))/((s1)*(Z-(z0)) + (s0)*((z1)-Z)))')
   part = part.replace('x0',str(x0))
   part = part.replace('y0',str(y0))
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('s0',str(s0))
   part = part.replace('s1',str(s1))
   return part

###############################################################################
#   Shearing
###############################################################################
def shear_x_y(part, y0, y1, dx0, dx1):
   part = part.replace('X','(X - (dx0) - ((dx1)-(dx0))*(Y-(y0))/((y1)-(y0)))')
   part = part.replace('y0',str(y0))
   part = part.replace('y1',str(y1))
   part = part.replace('dx0',str(dx0))
   part = part.replace('dx1',str(dx1))
   return part

def shear_x_z(part, z0, z1, dx0, dx1):
   part = part.replace('X','(X - (dx0) - ((dx1)-(dx0))*(Z-(z0))/((z1)-(z0)))')
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('dx0',str(dx0))
   part = part.replace('dx1',str(dx1))
   return part

def coshear_x_z(part, z0, z1, angle0, angle1, amplitude, offset):
   phase0 = pi*angle0/180.
   phase1 = pi*angle1/180.
   part = part.replace('X','(X - (offset) - (amplitude)*cos((phase0) + ((phase1)-(phase0))*(Z-(z0))/((z1)-(z0))))')
   part = part.replace('z0',str(z0))
   part = part.replace('z1',str(z1))
   part = part.replace('phase0',str(phase0))
   part = part.replace('phase1',str(phase1))
   part = part.replace('amplitude',str(amplitude))
   part = part.replace('offset',str(offset))
   return part
    

###############################################################################
#   Color
###############################################################################
red = (225 << 0)
green = (225 << 8)
blue = (225 << 16)
gray = (128 << 16) + (128 << 8) + (128 << 0)
white = (255 << 16) + (255 << 8) + (255 << 0)
teal = (255 << 16) + (255 << 8)
pink = (255 << 16) + (255 << 0)
yellow = (255 << 8) + (255 << 0)
brown = (45 << 16) + (82 << 8) + (145 << 0)
navy = (128 << 16) + (0 << 8) + (0 << 0)
tan = (60 << 16) + (90 << 8) + (125 << 0)
black = 0

def color(color, part):
    return MString('(%s * (%s))' % (color, part))