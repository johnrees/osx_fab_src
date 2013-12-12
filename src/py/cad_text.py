# cad_text.py
# Tools to generate text in the cad format.

# Assembled by Matt Keeter (with code from Neil Gershenfeld)
# matt.keeter@cba.mit.edu

# kokompe.cba.mit.edu

###############################################################################

# Usage:
#   text(text, x, y, height = None, align = 'CC'):
#       text is the desired string
#       x, y are the position
#       height is the glyph height.  Defaults to the current font height.
#       align sets horizontal (C/L/R) and vertical (C/T/B) alignment

#   set_font(height)
#       Pre-renders a set of font glyphs for a given glyph height.
   
#   When text() is called, it uses the precalculated glyph set if the height is
#   correct; otherwise, it renders glyphs of the appropriate height.
#
#   Prerendering glyphs of the appropriate size with set_font() can save time
#   that would be spent regenerating the font with every call to text().

###############################################################################

from cad_shapes import *

def make_glyphs(height = 1.0, width = None, line = None):
    if not width:
        width = 2*height/3
    if not line:
        line = height / 6
    
    glyphs = {}

    shape = triangle(0, 0, width/2, height, width, 0)
    shape = subtract(shape, triangle(line, 0, width/2, height - line*2, width - line, 0))
    shape = add(shape, rectangle(line, width - line, height/4 - line/2, height/4 + line/2))
    glyphs['A'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = add(shape,rectangle(width-line,width,0,height/3))
    glyphs['a'] = shape
    
    shape = rectangle(0, 2*line, 0, height/2 + line/2)
    shape = add(shape, circle(2*line, height/4 + line/4, height/4 + line/4))
    shape = subtract(shape, circle(2*line, height/4 + line/4, height/4 - 3*line/4))
    shape = subtract(shape, rectangle(line, 2*line, line, height/2 - line/2))
    shape = add(shape, move(shape,0, height/2 - line/2))
    glyphs['B'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = add(shape,rectangle(0,line,0,height))
    glyphs['b'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = add(shape,circle(width/2,height-width/2,width/2))
    shape = add(shape,rectangle(0,width,width/2, height-width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = subtract(shape,circle(width/2,height -width/2,width/2 - line))
    shape = subtract(shape,rectangle(line,width,width/2, height-width/2))
    glyphs['C'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = subtract(shape,rectangle(width/2,width,width/2-1/(2*line),width/2+1/(2*line)))
    glyphs['c'] = shape
    
    shape = circle(line,height/2,height/2)
    shape = subtract(shape,circle(line,height/2,height/2 - line))
    shape = subtract(shape,rectangle(line-height/2,line,0,height))
    shape = add(shape,rectangle(0,line,0,height))
    glyphs['D'] = shape
    
    shape = rectangle(width-line,width,0,height)
    shape = add(shape,circle(width/2,width/2,width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    glyphs['d'] = shape
    
    shape = rectangle(0,line,0,height)
    shape = add(shape,rectangle(0,width,height-line,height))
    shape = add(shape,rectangle(0,2*width/3,height/2-line/2,height/2+line/2))
    shape = add(shape,rectangle(0,width,0,line))
    glyphs['E'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,triangle(width,0,width/2,width/2-line/2,width,width/2-line/2))
    shape = add(shape,rectangle(line/2,width-line/2,width/2-line/2,width/2+line/2))
    glyphs['e'] = shape
    
    shape = rectangle(0,line,0,height)
    shape = add(shape,rectangle(0,width,height-line,height))
    shape = add(shape,rectangle(0,2*width/3,height/2-line/2,height/2+line/2))
    glyphs['F'] = shape
    
    shape = circle(width-line/2,height-width/2,width/2)
    shape = subtract(shape,circle(width-line/2,height-width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width-line/2,0,height-width/2))
    shape = subtract(shape,rectangle(width-line/2,2*width,0,height))
    shape = add(shape,rectangle(width/2-line/2,width/2+line/2,0,height-width/2))
    shape = add(shape,rectangle(width/5,4*width/5,height/2-line/2,height/2+line/2))
    glyphs['f'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = add(shape,circle(width/2,height-width/2,width/2))
    shape = add(shape,rectangle(0,width,width/2, height-width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = subtract(shape,circle(width/2,height -width/2,width/2 - line))
    shape = subtract(shape,rectangle(line,width,width/2, height-width/2))
    shape = add(shape, rectangle(width/2, width, width/2, width/2 + line))
    glyphs['G'] = shape
    
    shape = circle(width/2, 0, width/2)
    shape = subtract(shape, circle(width/2, 0, width/2 - line))
    shape = subtract(shape, rectangle(0, width, 0, height))
    shape = add(shape, circle(width/2,width/2,width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = add(shape,rectangle(width-line,width,0,width))
    glyphs['g'] = shape
    
    shape = rectangle(0,line,0,height)
    shape = add(shape,rectangle(width-line,width,0,height))
    shape = add(shape,rectangle(0,width,height/2-line/2,height/2+line/2))
    glyphs['H'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,0,width/2))
    shape = add(shape,rectangle(0,line,0,height))
    shape = add(shape,rectangle(width-line,width,0,width/2))
    glyphs['h'] = shape
    
    shape = rectangle(width/2-line/2,width/2+line/2,0,height)
    shape = add(shape,rectangle(width/5,4*width/5,0,line))
    shape = add(shape,rectangle(width/5,4*width/5,height-line,height))
    glyphs['I'] = shape
    
    shape = rectangle(width/2-line/2,width/2+line/2,0,height/2)
    shape = add(shape,circle(width/2,3*height/4,3*line/5))
    glyphs['i'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,width/2,height))
    shape = add(shape,rectangle(width-line,width,width/2,height))
    glyphs['J'] = shape
    
    shape = circle(width/2, 0, width/2)
    shape = subtract(shape, circle(width/2, 0, width/2 - line))
    shape = subtract(shape, rectangle(0, width, 0, height))
    shape = add(shape,rectangle(width-line,width,0,height/2))
    shape = add(shape,circle(width-line/2,3*height/4,3*line/5))
    glyphs['j'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(line,height,width-line,height,line,height/2+line))
    shape = subtract(shape,triangle(width,0,1.5*line,height/2,width,height))
    shape = subtract(shape,triangle(line,0,line,height/2-line,width-line,0))
    glyphs['K'] = shape
    
    shape = rectangle(0,3*width/4,0,height)
    shape = subtract(shape,triangle(line,height,width-line,height,line,height/3+line))
    shape = subtract(shape,triangle(3*width/4,0,1.5*line,height/3,width,height))
    shape = subtract(shape,triangle(line,0,line,height/2.5-line,3*width/4-line,0))
    shape = subtract(shape, rectangle(line, width, 3*height/4, height))
    glyphs['k'] = shape
    
    shape = rectangle(0,line,0,height)
    shape = add(shape,rectangle(0,width,0,line))
    glyphs['L'] = shape
    
    shape = rectangle(width/2-line/2,width/2+line/2,0,height)
    glyphs['l'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(line,0,line,height/2,width/2-line/3,0))
    shape = subtract(shape,triangle(line,height,width-line,height,width/2,height/4))
    shape = subtract(shape,triangle(width/2+line/3,0,width-line,height/2,width-line,0))
    glyphs['M'] = shape
    
    shape = circle(width/4 + line/4, width/2, width/4 + line/4)
    shape = subtract(shape, circle(width/4 + line/4, width/2, width/4 - 3*line/4))
    shape = subtract(shape, rectangle(0, width, 0, width/2))
    shape = add(shape, move(shape, width/2 - line/2, 0))
    shape = add(shape, rectangle(0, line, 0, width/2 + line))
    shape = add(shape, rectangle(width/2-line/2, width/2+line/2, 0, width/2))
    shape = add(shape, rectangle(width-line, width, 0, width/2))
    glyphs['m'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(4*line/3,height,width-line,height,width-line,height/4))
    shape = subtract(shape,triangle(line,0,line,3*height/4,width - 4*line/3,0))
    glyphs['N'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,0,width/2))
    shape = add(shape,rectangle(0,line,0,width))
    shape = add(shape,rectangle(width-line,width,0,width/2))
    glyphs['n'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = add(shape,circle(width/2,height-width/2,width/2))
    shape = add(shape,rectangle(0,width,width/2, height-width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2 - line))
    shape = subtract(shape,circle(width/2,height -width/2,width/2 - line))
    shape = subtract(shape,rectangle(line,width-line,width/2, height-width/2))
    glyphs['O'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    glyphs['o'] = shape
    
    # P scales a bit strangely
    shape = rectangle(0,line,0,height)
    shape = add(shape,circle(width/2,height-width/2,width/2))
    shape = add(shape,rectangle(0,width/2,height-width,height))
    shape = subtract(shape,circle(width/2,height-width/2,width/2-line))
    shape = subtract(shape,rectangle(line,width/2,height-width+line,height-line))
    glyphs['P'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = add(shape,rectangle(0,line,-height/3,width))
    glyphs['p'] = shape
    
    shape = glyphs['O']
    shape = add(shape,move(rotate(rectangle(-line/2,line/2,-width/4,width/4),30),3*width/4,width/4))
    glyphs['Q'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = add(shape,rectangle(width-line,width,-height/3,width))
    glyphs['q'] = shape
    
    shape = triangle(line,0,line,height,width,0)
    shape = subtract(shape,triangle(line,0,line,height-line*2,width-line,0))
    shape = subtract(shape,rectangle(0,width,height/3 + line/2,height))
    shape = add(glyphs['P'],shape)
    glyphs['R'] = shape
    
    shape = circle(width,0,width)
    shape = subtract(shape,circle(width,0,width-line))
    shape = subtract(shape,rectangle(.8*width,2*width,-width,width))
    shape = subtract(shape,rectangle(0,width,-width,0))
    shape = add(shape,rectangle(0,line,0,width))
    glyphs['r'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width/2,width/2,width))
    shape = add(shape,move(reflect_y(reflect_x(shape,width),width),0,width-line))
    shape = scale_y(shape,0,height/(2*width-line))
    glyphs['S'] = shape
    
    w = width/3
    shape = circle(w,w,w)
    shape = subtract(shape,circle(w,w,w-line*0.9))
    shape = subtract(shape,rectangle(0,w,w,2*w))
    shape = add(shape,move(reflect_y(reflect_x(shape,2*w),2*w),0,2*w-line*.9))
    shape = scale_y(shape,0,(2*height/3)/(4*w-line*.9))
    shape = move(shape,(width/2)-w,0)
    glyphs['s'] = shape
    
    shape = rectangle(width/2-line/2,width/2+line/2,0,height)
    shape = add(shape,rectangle(0,width,height-line,height))
    glyphs['T'] = shape
    
    shape = circle(0,3*width/8,3*width/8)
    shape = subtract(shape,circle(0,3*width/8,3*width/8-line))
    shape = subtract(shape,rectangle(-width,width,3*width/8,height))
    shape = subtract(shape,rectangle(0,width,-height,height))
    shape = move(shape,width/2-line/2+3*width/8,0)
    shape = add(shape,rectangle(width/2-line/2,width/2+line/2,width/4,3*height/4))
    shape = add(shape,rectangle(width/5,4*width/5,height/2-line/2,height/2+line/2))
    glyphs['t'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,width/2,height))
    shape = add(shape,rectangle(0,line,width/2,height))
    shape = add(shape,rectangle(width-line,width,width/2,height))
    glyphs['U'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,width/2,height))
    shape = add(shape,rectangle(0,line,width/2,2*height/3))
    shape = add(shape,rectangle(width-line,width,0,2*height/3))
    glyphs['u'] = shape
    
    shape = triangle(0,height,width,height,width/2,0)
    shape = subtract(shape,triangle(0,height+3*line,width,height+3*line,width/2,3*line))
    glyphs['V'] = shape
    
    w = 2*height/3.0
    shape = triangle(0,w,width,w,width/2,0)
    shape = subtract(shape,triangle(0,w+2*line,width,w+2*line,width/2,2*line))
    glyphs['v'] = shape
    
    shape = triangle(0,height,width,height,width/2,0)
    shape = add(shape,move(shape,.6*width,0))
    cutout = triangle(0,height+4*line,width,height+4*line,width/2,4*line)
    cutout = add(cutout,move(cutout,.6*width,0))
    shape = subtract(shape,cutout)
    shape = scale_x(shape,0,1/1.6)
    glyphs['W'] = shape
    
    glyphs['w'] = scale_y(glyphs['W'],0,width/height)
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(0,0,0,height,width/2-.7*line,height/2))
    shape = subtract(shape,triangle(width,0,width/2+.7*line,height/2,width,height))
    shape = subtract(shape,triangle(line,height,width-line,height,width/2,height/2+line))
    shape = subtract(shape,triangle(line,0,width/2,height/2-line,width-line,0))
    glyphs['X'] = shape
    
    glyphs['x'] =  scale_y(glyphs['X'], 0, width/height)
    
    shape = rectangle(0,width,height/2,height)
    shape = subtract(shape,triangle(0,height/2,0,height,width/2-line/2,height/2))
    shape = subtract(shape,triangle(width/2+line/2,height/2,width,height,width,height/2))
    shape = subtract(shape,triangle(1.1*line,height,width-1.1*line,height,width/2,height/2+1.1*line))
    shape = add(shape,rectangle(width/2-line/2,width/2+line/2,0,height/2))
    glyphs['Y'] = shape
    
    
    shape = rectangle(0,width,-height/3,width)
    shape = subtract(shape,triangle(0,-height/3,0,width,width/2-line*0.8,0))
    shape = subtract(shape,triangle(1.1*line,width,width-1.1*line,width,width/2-width*0.05,1.6*line))
    shape = subtract(shape,triangle(width/3,-height/3,width,width,width,-height/3))
    glyphs['y'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(0,line,0,height-line,width-1.4*line,height-line))
    shape = subtract(shape,triangle(1.4*line,line,width,height-line,width,line))
    glyphs['Z'] = shape
    
    w = 2*height/3
    shape = rectangle(0,width,0,w)
    shape = subtract(shape,triangle(0,line,0,w-line,width-1.6*line,w-line))
    shape = subtract(shape,triangle(width,line,1.6*line,line,width,w-line))
    glyphs['z'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-0.8*line))
    shape = scale_y(shape,0,height/width)
    glyphs['0'] = shape
    
    shape = rectangle(width/2-line/2,width/2+line/2,0,height)
    w = width/2-line/2
    cutout = circle(0,height,w)
    shape = add(shape,rectangle(0,width/2,height-w-line,height))
    shape = subtract(shape,cutout)
    shape = move(shape,(width/2+line/2)/4,0)
    glyphs['1'] = shape
    
    shape = circle(width/2,height-width/2,width/2)
    shape = subtract(shape,circle(width/2,height-width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width/2,0,height-width/2))
    shape = add(shape,rectangle(0,width,0,height-width/2))
    shape = subtract(shape,triangle(0,line,0,height-width/2,width-line,height-width/2))
    shape = subtract(shape,triangle(1.5*line,line,width,height-width/2-.5*line,width,line))
    glyphs['2'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = scale_y(shape,0,(height/2+line/2)/width)
    shape = add(shape,move(shape,0,height/2-line/2))
    shape = subtract(shape,rectangle(0,width/2,height/4,3*height/4))
    glyphs['3'] = shape
    
    shape = rectangle(width-line,width,0,height)
    shape = add(shape,triangle(0,height/3,width-line,height,width-line,height/3))
    shape = subtract(shape,triangle(width/2*line,height/3+line,width-line,height-1.5*line,width-line,height/3+line))
    glyphs['4'] = shape
    
    ########
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width/2,width/2,width))
    shape = add(shape,rectangle(0,width/2,width-line,width))
    shape = add(shape,rectangle(0,line,width-line,height))
    shape = add(shape,rectangle(0,width,height-line,height))
    glyphs['5'] = shape
    
    shape = circle(width/2,height-width/2,width/2)
    shape = subtract(shape,circle(width/2,height-width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,0,height-width/2))
    shape = subtract(shape,triangle(width,height,width,height/2,width/2,height/2))
    shape = add(shape,circle(width/2,width/2,width/2))
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = add(shape,rectangle(0,line,width/2,height-width/2))
    glyphs['6'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(0,0,0,height-line,width-line,height-line))
    shape = subtract(shape,triangle(line,0,width,height-line,width,0))
    glyphs['7'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = scale_y(shape,0,(height/2+line/2)/width)
    shape = add(shape,move(shape,0,height/2-line/2))
    glyphs['8'] = shape
    
    shape = circle(width/2,width/2,width/2)
    shape = subtract(shape,circle(width/2,width/2,width/2-line))
    shape = subtract(shape,rectangle(0,width,width/2,height))
    shape = subtract(shape,triangle(0,0,0,height/2,width/2,height/2))
    shape = add(shape,circle(width/2,height-width/2,width/2))
    shape = subtract(shape,circle(width/2,height-width/2,width/2-line))
    shape = add(shape,rectangle(width-line,width,width/2,height-width/2))
    glyphs['9'] = shape
    
    w = width/2
    shape = circle(w,w,w)
    shape = subtract(shape,circle(w,w,w-line))
    shape = subtract(shape,rectangle(w,width,0,height))
    shape = scale_y(shape,0,height/width)
    shape = move(shape,w/2,0)
    glyphs['('] = shape
    
    shape = reflect_x(glyphs['('],width)
    glyphs[')'] = shape
    
    glyphs[' '] = '0'
    
    shape = rectangle(width/2-width/3,width/2+width/3,height/2-line/2,height/2+line/2)
    shape = add(shape,rectangle(width/2-line/2,width/2+line/2,height/2-width/3,height/2+width/3))
    glyphs['+'] = shape
    
    shape = rectangle(width/2-width/3,width/2+width/3,height/2-line/2,height/2+line/2)
    glyphs['-'] = shape
    
    shape = circle(width/2,line,3*line/4)
    glyphs['.'] = shape
    
    shape = rectangle(0,width,0,height)
    shape = subtract(shape,triangle(4*line/5,0,width,height-4*line/5,width,0))
    shape = subtract(shape,triangle(0,4*line/5,0,height,width-4*line/5,height))
    glyphs['/'] = shape
    
    #
    # to be done
    #
    # glyphs['~'] = False
    # glyphs['!'] = False
    # glyphs['@'] = False
    # glyphs['#'] = False
    # glyphs['$'] = False
    # glyphs['%'] = False
    # glyphs['^'] = False
    # glyphs['&'] = False
    # glyphs['&'] = False
    # glyphs['_'] = False
    # glyphs['='] = False
    # glyphs['['] = False
    # glyphs['{'] = False
    # glyphs[']'] = False
    # glyphs['}'] = False
    # glyphs[';'] = False
    # glyphs[':'] = False
    # glyphs["'"] = False
    # glyphs['"'] = False
    # glyphs[','] = False
    # glyphs['<'] = False
    # glyphs['>'] = False
    # glyphs['?'] = False

    return glyphs, width, height

def set_font(height):
    global glyphs, glyph_width, glyph_height
    glyphs, glyph_width, glyph_height = make_glyphs(height)

def text(text, x, y, height = None, align = 'CC'):
    dx = 0
    dy = 0

    text_shape = '0'
    
    if height is None:
        height = glyph_height
    
    if height == glyph_height:
        width = glyph_width
        glyphs_ = glyphs
    else:
        glyphs_, width, height = make_glyphs(height)

    
    for line in text.split('\n'):
        line_shape = '0'
        for chr in line:
            # Only draw characters that we have glyphs for
            if not chr in glyphs_.keys():
                print "Unknown character:", chr
                continue

            line_shape = add(line_shape, move(glyphs_[chr], dx, dy))
            
            dx += 3*width/2

        # Horizontally align this line of text
        dx -= 3*width/2
        if (align[0] == 'L'):
            text_shape = add(text_shape, line_shape)
        elif (align[0] == 'C'):
            text_shape = add(text_shape, move(line_shape, -dx / 2, 0))
        elif (align[0] == 'R'):
            text_shape = add(text_shape, move(line_shape, -dx, 0))
        else:
            print "Invalid horizontal alignment character."
        
        dx = 0
        dy -= 3*height/2
    
    dy += 3*height/2
    
    # Vertically align the text
    if (align[1] == 'T'):
        pass
    elif (align[1] == 'C'):
        text_shape = move(text_shape, 0, -dy/2)
    elif (align[1] == 'B'):
        text_shape = move(text_shape, 0, -dy)
    else:
        print "Invalid vertical alignment character."
    
    text_shape = move(text_shape, x, y)
    return text_shape

set_font(1.0)