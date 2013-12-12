#
# panel_path.py
#    .path viewer
#
# Neil Gershenfeld 10/14/13
# (c) Massachusetts Institute of Technology 2013
#
# This work may be reproduced, modified, distributed,
# performed, and displayed for any purpose, but must
# acknowledge the fab modules project. Copyright is
# retained and must be preserved. The work is provided
# as is; no warranty is provided, and users accept all 
# liability.
#
# imports
#
import wx,math

#
# panel class
#
class path_panel(wx.Panel):
   #
   # constructor
   #
   def __init__(self,parent):
      self.parent = parent
      class path_object(object):
         def __init__(self):
            self.segments = []
      self.path = path_object()
      self.scale = 1.0
      self.zx = 0
      self.zy = 0
      self.px = 0
      self.py = 0
      self.ox = 0
      self.oy = 0
      self.sx = []
      self.sy = []
      self.rx = []
      self.ry = []
      self.theta_x = 0
      self.theta_z = 0
      #
      # drawing panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      self.panel = wx.Panel(self,size=(self.parent.parent.size,self.parent.parent.size)) 
      self.panel.SetBackgroundColour('white')
      self.panel.Bind(wx.EVT_PAINT,self.paint) 
      self.panel.Bind(wx.EVT_MOUSEWHEEL,self.mouse_wheel)
      self.panel.Bind(wx.EVT_LEFT_DOWN,self.mouse_left_down)
      self.panel.Bind(wx.EVT_LEFT_UP,self.mouse_left_up)
      self.panel.Bind(wx.EVT_MOTION,self.mouse_move)
      self.panel.Bind(wx.EVT_RIGHT_DOWN,self.mouse_right_down)
      self.panel.Bind(wx.EVT_RIGHT_UP,self.mouse_right_up)
      self.sizer.Add(self.panel,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # controls
      #
      control_panel = wx.Panel(self)
      control_sizer = wx.GridBagSizer(10,10)
      control_panel.SetSizer(control_sizer)
      self.message = wx.StaticText(control_panel,label='')
      control_sizer.Add(self.message,(0,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      #
      reset_button = wx.Button(control_panel,label='reset view')
      reset_button.Bind(wx.EVT_BUTTON,self.reset_view)
      control_sizer.Add(reset_button,(0,1),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.view_types = ["none","segments","segments+directions","segments+directions+connections"]
      self.view_type_control = wx.ComboBox(control_panel,size=(100,-1),value="view type",choices=self.view_types,style=wx.CB_READONLY)
      self.view_type = "segments+directions+connections"
      self.view_type_control.Bind(wx.EVT_COMBOBOX,self.view_type_handler)
      control_sizer.Add(self.view_type_control,(0,2))
      #
      self.sizer.Add(control_panel,(1,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
   #
   # view type handler
   #
   def view_type_handler(self,event):
      self.view_type = self.view_types[event.GetSelection()]
      self.panel.Refresh()
   #
   # read path
   #
   def read_path(self,path_file_name):
      class path_object(object):
         def __init__(self):
            self.segments = []
      path = path_object()
      path_file = open(path_file_name,'r')
      while 1:
         line = path_file.readline()
         if (line == ''):
            break
         elif (0 == line.find("dof:")):
            path.dof = int(line.split()[1])
         elif (0 == line.find("units:")):
            if (path.dof >= 2):
               path.ux = line.split()[1]
               path.uy = line.split()[2]
            if (path.dof > 2):
               path.uz = line.split()[3]
         elif (0 == line.find("nx ny")):
            if (path.dof == 2):
               path.nx = int(line.split()[2])
               path.ny = int(line.split()[3])
            if (path.dof > 2):
               path.nx = int(line.split()[3])
               path.ny = int(line.split()[4])
               path.nz = int(line.split()[5])
         elif (0 == line.find("dx dy")):
            if (path.dof == 2):
               path.dx = float(line.split()[2])
               path.dy = float(line.split()[3])
            if (path.dof > 2):
               path.dx = float(line.split()[3])
               path.dy = float(line.split()[4])
               path.dz = float(line.split()[5])
         elif (0 == line.find("xmin ymin")):
            if (path.dof == 2):
               path.xmin = float(line.split()[2])
               path.ymin = float(line.split()[3])
            if (path.dof > 2):
               path.xmin = float(line.split()[3])
               path.ymin = float(line.split()[4])
               path.zmin = float(line.split()[5])
         elif (0 == line.find("segment start:")):
            path.segments.append([])
            while 1:
               line = path_file.readline()
               if (0 == line.find("segment end:")):
                  break
               if (path.dof == 2):
                  x = float(line.split()[0])
                  y = float(line.split()[1])
                  path.segments[-1].append([x,y])
               if (path.dof == 3):
                  x = float(line.split()[0])
                  y = float(line.split()[1])
                  z = float(line.split()[2])
                  path.segments[-1].append([x,y,z])
      if (path.dof == 2):
         self.message.SetLabel('left: pan, scroll: zoom')
      elif (path.dof > 2):
         if (path.dz > 0):
            self.message.SetLabel('left: pan, scroll: zoom, right: rotate')
         else:
            self.message.SetLabel('left: pan, scroll: zoom')
      self.parent.Layout()
      self.parent.Fit()
      path_file.close()
      return path
   #
   # draw path
   #
   def draw(self,path_file):
      self.path = self.read_path(path_file)
      self.panel.Refresh()
   #
   # rotate point
   #
   def rotate(self,x,y,z):
      x -= self.path.nx/2.0
      y -= self.path.ny/2.0
      z -= self.path.nz/2.0
      angle = self.theta_z
      xtemp = math.cos(angle)*x - math.sin(angle)*y
      ytemp = math.sin(angle)*x + math.cos(angle)*y
      x = xtemp
      y = ytemp
      angle = self.theta_x
      ytemp = math.cos(angle)*y - math.sin(angle)*z
      ztemp = math.sin(angle)*y + math.cos(angle)*z
      y = ytemp
      z = ztemp
      x += self.path.nx/2.0
      y += self.path.ny/2.0
      z += self.path.nz/2.0
      return(x,y,z)
   #
   # scale and draw line
   #
   def draw_line(self,p0,p1,color,shading='off',end='none'):
      #
      # set aspect ratio
      #
      size = self.parent.parent.size
      ratio = float(self.path.ny)/float(self.path.nx)
      if (ratio > 1):
         ysize = size
         xsize = size/ratio
      else:
         ysize = size*ratio
         xsize = size
      #
      # get points 
      #
      if (len(p0) >= 2):
         x0 = p0[0]
         x1 = p1[0]
         y0 = p0[1]
         y1 = p1[1]
      if (len(p0) >= 3):
         z0 = p0[2]
         z1 = p1[2]
      #
      # set 3D shading
      #
      if (self.path.dof > 2):
         if ((self.path.nz > 1) & (shading == 'z')):
            i = 0.8*((z0+z1)/2.0)/(self.path.nz-1.0)
            color = (255.0*i+color[0]*(1.0-i),255.0*i+color[1]*(1.0-i),255.0*i+color[2]*(1.0-i))
      #
      # rotate
      #
      if (self.path.dof > 2):
         if (self.path.nz > 1):
            (x0,y0,z0) = self.rotate(x0,y0,z0)
            (x1,y1,z1) = self.rotate(x1,y1,z1)
      #
      # pan
      #
      if (self.sx != []):
         self.px = self.mx - self.sx
      if (self.sy != []):
         self.py = self.my - self.sy
      #
      # zoom
      #
      xi0 = self.ox + self.px + self.zx + self.scale*(xsize*(x0/(self.path.nx-1.0))-self.zx)
      xi1 = self.ox + self.px + self.zx + self.scale*(xsize*(x1/(self.path.nx-1.0))-self.zx)
      yi0 = self.oy + self.py + self.zy + self.scale*(ysize*(y0/(self.path.ny-1.0))-self.zy)
      yi1 = self.oy + self.py + self.zy + self.scale*(ysize*(y1/(self.path.ny-1.0))-self.zy)
      #
      # draw line
      #
      self.dc.SetPen(wx.Pen(color,1))
      self.dc.DrawLine(xi0,yi0,xi1,yi1)
      #
      # draw arrow head
      #
      if (end == "arrow"):
         d = math.sqrt((xi1-xi0)*(xi1-xi0)+(yi1-yi0)*(yi1-yi0))
         if (d > 0):
            dx = 4*(xi1-xi0)/d
            dy = 4*(yi1-yi0)/d
            self.dc.DrawLine(xi1,yi1,xi1-dx+dy,yi1-dy-dx)
            self.dc.DrawLine(xi1,yi1,xi1-dx-dy,yi1-dy+dx)
            self.dc.DrawLine(xi1-dx+dy,yi1-dy-dx,xi1-dx-dy,yi1-dy+dx)
   #
   # real to panel coordinates
   #
   def coords(self,p):
      if (len(p) == 2):
         ix = self.path.nx*(p[0]-self.path.xmin)/self.path.dx
         iy = self.path.nx*(self.path.ymin+self.path.dy-p[1])/self.path.dx
         return([ix,iy])
      elif (len(p) >= 3):
         ix = self.path.nx*(p[0]-self.path.xmin)/self.path.dx
         iy = self.path.nx*(self.path.ymin+self.path.dy-p[1])/self.path.dx
         iz = self.path.nx*(p[2]-self.path.zmin)/self.path.dx
         return([ix,iy,iz])
   #
   # paint panel
   #
   def paint(self,event):
      self.dc = wx.PaintDC(self.panel)
      #
      # return if no path
      #
      if (len(self.path.segments) == 0):
         return
      #
      # return if view none
      #
      if (self.view_type == "none"):
         return
      #
      # draw axes
      #
      if (self.path.dof == 2):
         self.draw_line(self.coords([0,0]),self.coords([self.path.dx/10.0,0]),(0,255,0),end="arrow")
         self.draw_line(self.coords([0,0]),self.coords([0,self.path.dx/10.0]),(0,255,255),end="arrow")
      elif (self.path.dof >= 3):
         self.draw_line(self.coords([0,0,0]),self.coords([self.path.dx/10.0,0,0]),(0,255,0),end="arrow")
         self.draw_line(self.coords([0,0,0]),self.coords([0,self.path.dx/10.0,0]),(0,255,255),end="arrow")
         self.draw_line(self.coords([0,0,0]),self.coords([0,0,self.path.dx/10.0]),(255,0,255),end="arrow")
      #
      # loop over segments
      #
      if (self.view_type == "segments"):
         for segment in range(len(self.path.segments)):
            s0 = self.path.segments[segment]
            #
            # loop over points
            #
            for point in range(len(s0)-1):
               self.draw_line(s0[point],s0[point+1],(0,0,255),shading='z',end="none")
      elif (self.view_type == "segments+directions"):
         for segment in range(len(self.path.segments)):
            s0 = self.path.segments[segment]
            #
            # loop over points
            #
            for point in range(len(s0)-1):
               self.draw_line(s0[point],s0[point+1],(0,0,255),shading='z',end="arrow")
      elif (self.view_type == "segments+directions+connections"):
         for segment in range(len(self.path.segments)):
            s0 = self.path.segments[segment]
            #
            # loop over points
            #
            for point in range(len(s0)-1):
               self.draw_line(s0[point],s0[point+1],(0,0,255),shading='z',end="arrow")
            #
            # draw path to next segment
            #
            if (segment < (len(self.path.segments)-1)):
               s1 = self.path.segments[segment+1]
               self.draw_line(s0[-1],s1[0],(255,0,0),shading='z',end="arrow")
   #
   # mouse move: pan
   #
   def mouse_move(self,event):
      self.mx = event.GetX()
      self.my = event.GetY()
      size = self.parent.parent.size
      if (self.sx != []):
         #
         # panning
         #
         self.panel.Refresh()
      if (self.rx != []):
         #
         # rotating
         #
         self.theta_z = self.theta_zs + math.pi*(self.mx-self.rx)/(size-1.0)
         self.theta_x = self.theta_xs + math.pi*(self.ry-self.my)/(size-1.0)
         self.panel.Refresh()
   #
   # mouse left down: start pan
   #
   def mouse_left_down(self,event):
      self.sx = event.GetX()
      self.sy = event.GetY()
   #
   # mouse left up: stop pan
   #
   def mouse_left_up(self,event):
      self.sx = []
      self.sy = []
      self.ox = self.ox + self.px
      self.oy = self.oy + self.py
      self.px = 0
      self.py = 0
   #
   # mouse right down: start rotate
   #
   def mouse_right_down(self,event):
      self.rx = event.GetX()
      self.ry = event.GetY()
      self.theta_xs = self.theta_x
      self.theta_zs = self.theta_z
   #
   # mouse right up: stop rotate
   #
   def mouse_right_up(self,event):
      self.rx = []
      self.ry = []
   #
   # mouse wheel: zoom
   #
   def mouse_wheel(self,event):
      mx = (event.GetX() - self.ox - self.zx)/self.scale + self.zx
      my = (event.GetY() - self.oy - self.zy)/self.scale + self.zy
      self.ox = self.ox + (self.scale - 1.0)*(mx - self.zx)
      self.oy = self.oy + (self.scale - 1.0)*(my - self.zy)
      self.zx = mx
      self.zy = my
      if (event.GetWheelRotation() > 0):
         self.scale *= 1.1
      else:
         self.scale *= 0.9
      self.panel.Refresh()
   #
   # reset view
   #
   def reset_view(self,event):
      self.scale = 1.0
      self.zx = 0
      self.zy = 0
      self.px = 0
      self.py = 0
      self.ox = 0
      self.oy = 0
      self.sx = []
      self.sy = []
      self.theta_x = 0
      self.theta_z = 0
      self.panel.Refresh()
   #
   # parent call to update size
   #
   def update_size(self,sizex,sizey):
      self.Layout()
      self.Fit()
