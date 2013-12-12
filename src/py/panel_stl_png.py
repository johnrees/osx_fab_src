#
# panel_stl_png.py
#    make .png from .stl
#
# Neil Gershenfeld
# CBA MIT 3/6/11
#
# (c) Massachusetts Institute of Technology 2012
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os,string
#
# panel class
#
class stl_png_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.png_file = ''
      #
      # get png info 
      #
      def png_info(name):
         temp_name = self.parent.tmp+'png_info'
         command = 'png_size '+'\"'+name+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         output = output_file.read()
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         return output
      #
      # make png
      #
      def make_png(event):
         if (self.parent.stl_file == ''):
            print "panel_stl_png: oops -- must read .stl first"
            return
         #
         # construct command
         #
         self.parent.png_file = self.parent.tmp+self.parent.rootname+'.png'
         units = self.units.GetValue()
         self.parent.units = float(units)
         resolution = self.resolution.GetValue()
         if (self.x.GetValue()):
            axis = 'x'
         elif (self.X.GetValue()):
            axis = 'X'
         elif (self.y.GetValue()):
            axis = 'y'
            self.parent.zmin = 1234
         elif (self.Y.GetValue()):
            axis = 'Y'
         elif (self.z.GetValue()):
            axis = 'z'
         elif (self.Z.GetValue()):
            axis = 'Z'
         command = 'stl_png '+'\"'+self.parent.stl_file+'\"'+' '+'\"'+self.parent.png_file+'\"'+' '+units+' '+resolution+' '+axis
         #
         # generate PNG
         #
         print command
         os.system(command)
         #
         # get image info
         #
         info = png_info(self.parent.png_file)
         self.info.SetLabel(info)
         start = 3+string.find(info,'dx:')
         end = string.find(info,'mm',start)-1
         self.parent.xmin = 0
         self.parent.xmax = float(info[start:end])/self.parent.units
         start = 3+string.find(info,'dy:')
         end = string.find(info,'mm',start)-1
         self.parent.ymin = 0
         self.parent.ymax = float(info[start:end])/self.parent.units
         start = 3+string.find(info,'dz:')
         end = string.find(info,'mm',start)-1
         self.parent.zmin = -float(info[start:end])/self.parent.units
         self.parent.zmax = 0
         #
         # update panels
         #
         parent.update_panels()
         #
         # show image
         #
         png_image = wx.Image(self.parent.png_file)
         (nx,ny) = png_image.GetSize()
         ratio = float(ny)/float(nx)
         if (ratio > 1):
            self.parent.ysize = self.parent.size
            self.parent.xsize = self.parent.size/ratio
         else:
            self.parent.ysize = self.parent.size*ratio
            self.parent.xsize = self.parent.size
         wx.Image.Rescale(png_image,self.parent.xsize,self.parent.ysize)
         png_bitmap = png_image.ConvertToBitmap()
         self.bitmap.SetBitmap(png_bitmap)
         self.bitmap.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label
      #
      label = wx.StaticText(self,label='to: png')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # image
      #
      image = wx.ArtProvider.GetBitmap(wx.ART_QUESTION, wx.ART_OTHER, (self.parent.size,self.parent.size))
      self.bitmap = wx.StaticBitmap(self,-1,image)
      self.sizer.Add(self.bitmap,(1,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      self.bitmap.Hide()
      def mouse_move(event):
         if (self.x.GetValue()):
            y = self.parent.units*(self.parent.ymin + (self.parent.ymax-self.parent.ymin)*event.GetX()/float(self.parent.xsize))
            z = self.parent.units*(self.parent.zmin + (self.parent.zmax-self.parent.zmin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("y: %.3f mm  z: %.3f mm"%(y,z))
         elif (self.X.GetValue()):
            y = self.parent.units*(self.parent.ymin + (self.parent.ymax-self.parent.ymin)*event.GetX()/float(self.parent.xsize))
            z = self.parent.units*(self.parent.zmin + (self.parent.zmax-self.parent.zmin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("y: %.3f mm  z: %.3f mm"%(y,z))
         elif (self.y.GetValue()):
            z = self.parent.units*(self.parent.zmin + (self.parent.zmax-self.parent.zmin)*event.GetX()/float(self.parent.xsize))
            x = self.parent.units*(self.parent.xmin + (self.parent.xmax-self.parent.xmin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("x: %.3f mm  z: %.3f mm"%(x,z))
         elif (self.Y.GetValue()):
            z = self.parent.units*(self.parent.zmin + (self.parent.zmax-self.parent.zmin)*(self.parent.xsize-event.GetX())/float(self.parent.xsize))
            x = self.parent.units*(self.parent.ymin + (self.parent.ymax-self.parent.ymin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("x: %.3f mm  z: %.3f mm"%(x,z))
         elif (self.z.GetValue()):
            x = self.parent.units*(self.parent.xmin + (self.parent.xmax-self.parent.xmin)*event.GetX()/float(self.parent.xsize))
            y = self.parent.units*(self.parent.ymin + (self.parent.ymax-self.parent.ymin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("x: %.3f mm  y: %.3f mm"%(x,y))
         elif (self.Z.GetValue()):
            x = self.parent.units*(self.parent.xmin + (self.parent.xmax-self.parent.xmin)*(self.parent.xsize-event.GetX())/float(self.parent.xsize))
            y = self.parent.units*(self.parent.ymin + (self.parent.ymax-self.parent.ymin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize))
            self.position.SetLabel("x: %.3f mm  y: %.3f mm"%(x,y))
         self.Layout()
         self.Fit()
      self.bitmap.Bind(wx.EVT_MOTION,mouse_move)
      #
      # controls
      #
      make = wx.Button(self,label='make .png')
      make.Bind(wx.EVT_BUTTON,make_png)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      units_panel = wx.Panel(self)
      units_panel_sizer = wx.GridBagSizer(10,10)
      units_panel.SetSizer(units_panel_sizer)
      units_panel_sizer.Add(wx.StaticText(units_panel,label='stl units (mm/unit):'),(0,0),flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
      self.units = wx.TextCtrl(units_panel,-1,'1')
      units_panel_sizer.Add(self.units,(0,1),flag=wx.ALIGN_LEFT)
      self.sizer.Add(units_panel,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      res_panel = wx.Panel(self)
      res_panel_sizer = wx.GridBagSizer(10,10)
      res_panel.SetSizer(res_panel_sizer)
      res_panel_sizer.Add(wx.StaticText(res_panel,label='png resolution (pixels/mm):'),(0,0),flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
      self.resolution = wx.TextCtrl(res_panel,-1,'25')
      res_panel_sizer.Add(self.resolution,(0,1),flag=wx.ALIGN_LEFT)
      self.sizer.Add(res_panel,(4,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      axis_panel = wx.Panel(self)
      axis_sizer = wx.GridBagSizer(10,10)
      axis_panel.SetSizer(axis_sizer)
      axis_sizer.Add(wx.StaticText(axis_panel,label='side:'),(0,0),flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
      self.x = wx.RadioButton(axis_panel,label='x',style=wx.RB_GROUP)
      axis_sizer.Add(self.x,(0,1))
      self.X = wx.RadioButton(axis_panel,label='-x')
      axis_sizer.Add(self.X,(0,2))
      self.y = wx.RadioButton(axis_panel,label='y')
      axis_sizer.Add(self.y,(0,3))
      self.Y = wx.RadioButton(axis_panel,label='-y')
      axis_sizer.Add(self.Y,(0,4))
      self.z = wx.RadioButton(axis_panel,label='z')
      axis_sizer.Add(self.z,(0,5))
      self.Z = wx.RadioButton(axis_panel,label='-z')
      axis_sizer.Add(self.Z,(0,6))
      self.sizer.Add(axis_panel,(5,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      self.z.SetValue(True)
      #
      self.position= wx.StaticText(self,label="")
      self.sizer.Add(self.position,(6,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info = wx.StaticText(self,label="")
      self.sizer.Add(self.info,(7,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
   #
   # parent call to update panel for workflow
   #
   def update_panel(self):
      self.units.SetValue(str(self.parent.units))

