#!/usr/bin/env python
#
# png_l.py
#    converted a PNG image to a grayscale integer lattice
#    png_l input.png output.l
#
# Neil Gershenfeld
# CBA MIT 7/2/10
#
# (c) Massachusetts Institute of Technology 2010
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# todo
#    C image library port
#    pipe I/O
#    variable bit depth
#    z layers
#

import sys, struct, Image
from numpy import *

#
# read command line
#

if len(sys.argv) == 3:
   infile_name = sys.argv[1]
   outfile_name = sys.argv[2]
else:
   print "command line: png_l input.png output.l"
   sys.exit()


#
# read image
#

image = Image.open(infile_name)
(nx,ny) = image.size
nz = 1
info = image.info
if ('dpi' in info):
   (xdpi,ydpi) = info['dpi']
else:
   (xdpi,ydpi) = (1,1)
print "nx:",nx,"ny:",ny,"xdpi:",xdpi,"ydpi",ydpi

#
# convert to grayscale
#

image = image.convert("L")

#
# set variables
#

x0 = 0
y0 = 0
dx = nx/float(xdpi)
dy = ny/float(ydpi)
nz = 1
z0 = 0
dz = 0
bytes_per_pixel = 1

#
# construct file buffer
#

buf = struct.pack('I',nx)
buf += struct.pack('I',ny)
buf += struct.pack('I',nz)
buf += struct.pack('f',dx)
buf += struct.pack('f',dy)
buf += struct.pack('f',dz)
buf += struct.pack('f',x0)
buf += struct.pack('f',y0)
buf += struct.pack('f',z0)
buf += struct.pack('I',bytes_per_pixel)

for y in range(ny):
   for x in range(nx):
      buf += struct.pack('B',image.getpixel((x,y)))

#
# write to file
#

outfile = open(outfile_name,"wb")
outfile.write(buf)
outfile.close()
