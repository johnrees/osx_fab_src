#
# panel_png_path_halftone.py
#    make .path from .png halftone
#
# Neil Gershenfeld
# CBA MIT 3/12/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os
#
# panel class
#
class png_path_halftone_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.png_file = ''
      #
      # make path
      #
      def make_path(event):
         if (self.parent.png_file == ''):
            print "png_path: oops -- must make .png first"
            return
         self.parent.path_file = self.parent.tmp+self.parent.rootname+'.path'
         path_png = self.parent.tmp+self.parent.rootname+'.path.png'
         threshold = self.threshold.GetValue()
         points = self.points.GetValue()
         spot_size = self.size.GetValue()
         spacing = self.spacing.GetValue()
         offset = self.offset.GetValue()
         invert = self.invert.GetValue()
         if (invert == False):
            invert = '0'
         else:
            invert = '1'
         command = 'png_halftone '+'\"'+self.parent.filename+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+threshold+' '+points+' '+spot_size+' '+spacing+' '+offset+' '+invert
         print command
         os.system(command)
         command = 'path_png '+'\"'+self.parent.path_file+'\"'+' '+'\"'+path_png+'\"'
         print command
         os.system(command)
         command = 'png_scale '+'\"'+path_png+'\"'+' '+'\"'+path_png+'\"'+' 1 0'
         print command
         os.system(command)
         path_image = wx.Image(path_png)
         path_image = wx.Image.Blur(path_image,1)
         (nx,ny) = path_image.GetSize()
         ratio = float(ny)/float(nx)
         if (ratio > 1):
            self.parent.ysize = self.parent.size
            self.parent.xsize = self.parent.size/ratio
         else:
            self.parent.ysize = self.parent.size*ratio
            self.parent.xsize = self.parent.size
         wx.Image.Rescale(path_image,self.parent.xsize,self.parent.ysize,quality=wx.IMAGE_QUALITY_HIGH)
         path_bitmap = path_image.ConvertToBitmap()
         self.bitmap.SetBitmap(path_bitmap)
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
      label = wx.StaticText(self,label='to: halftone path')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),span=(1,4),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # image
      #
      image = wx.ArtProvider.GetBitmap(wx.ART_QUESTION, wx.ART_OTHER, (self.parent.size,self.parent.size))
      self.bitmap = wx.StaticBitmap(self,-1,image)
      self.sizer.Add(self.bitmap,(1,0),span=(1,4),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      self.bitmap.Hide()
      #
      # controls
      #
      make = wx.Button(self,label='make .path')
      make.Bind(wx.EVT_BUTTON,make_path)
      self.sizer.Add(make,(2,0),span=(1,4),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.sizer.Add(wx.StaticText(self,label='minimum spot radius (pixels):'),(3,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.threshold = wx.TextCtrl(self,-1,'1')
      self.sizer.Add(self.threshold,(3,1))
      #
      self.sizer.Add(wx.StaticText(self,label='points per spot:'),(3,2),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.points = wx.TextCtrl(self,-1,'8')
      self.sizer.Add(self.points,(3,3))
      #
      self.sizer.Add(wx.StaticText(self,label='max spot size (mm):'),(4,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.size = wx.TextCtrl(self,-1,'1')
      self.sizer.Add(self.size,(4,1))
      #
      self.sizer.Add(wx.StaticText(self,label='spot spacing (1 = spot size):'),(4,2),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.spacing = wx.TextCtrl(self,-1,'1')
      self.sizer.Add(self.spacing,(4,3))
      #
      self.sizer.Add(wx.StaticText(self,label='row offset (1 = spot size):'),(5,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.offset = wx.TextCtrl(self,-1,'0.5')
      self.sizer.Add(self.offset,(5,1))
      #
      self.invert = wx.CheckBox(self,-1,"invert image")
      self.invert.SetValue(False)
      self.sizer.Add(self.invert,(5,2))
      #
      # fit
      #
      self.Fit()
