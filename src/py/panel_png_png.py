#
# panel_png_png.py
#    make .pngfrom .png
#
# Neil Gershenfeld
# CBA MIT 7/24/11
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
class png_png_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.png_file = ''
      #
      # make png
      #
      def make_path(event):
         if (self.parent.png_file == ''):
            print "png_png: oops -- must make .png first"
            return
         png_png_file = self.parent.tmp+self.parent.rootname+'.png'
         threshold = self.threshold.GetValue()
         if self.distances.GetValue():
            distances = '1'
         else:
            distances = '0'
         command = 'png_distances '+'\"'+self.parent.png_file+'\"'+' '+'\"'+png_png_file+'\"'+' '+threshold+' '+distances
         print command
         os.system(command)
         png_image = wx.Image(png_png_file)
         png_image = wx.Image.Blur(png_image,1)
         (nx,ny) = png_image.GetSize()
         ratio = float(ny)/float(nx)
         if (ratio > 1):
            self.parent.ysize = self.parent.size
            self.parent.xsize = self.parent.size/ratio
         else:
            self.parent.ysize = self.parent.size*ratio
            self.parent.xsize = self.parent.size
         wx.Image.Rescale(png_image,self.parent.xsize,self.parent.ysize,quality=wx.IMAGE_QUALITY_HIGH)
         path_bitmap = png_image.ConvertToBitmap()
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
      #
      # controls
      #
      make = wx.Button(self,label='make .png')
      make.Bind(wx.EVT_BUTTON,make_path)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      threshold_panel = wx.Panel(self)
      threshold_sizer = wx.GridBagSizer(10,10)
      threshold_panel.SetSizer(threshold_sizer)
      threshold_sizer.Add(wx.StaticText(threshold_panel,label='threshold (0-1):'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.threshold = wx.TextCtrl(threshold_panel,-1,'0.5')
      threshold_sizer.Add(self.threshold,(0,1))
      self.sizer.Add(threshold_panel,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      type_panel = wx.Panel(self)
      type_sizer = wx.GridBagSizer(10,10)
      type_panel.SetSizer(type_sizer)
      self.interior = wx.RadioButton(type_panel,label='interior',style=wx.RB_GROUP)
      type_sizer.Add(self.interior,(0,0))
      self.distances = wx.RadioButton(type_panel,label='distances')
      type_sizer.Add(self.distances,(0,1))
      self.sizer.Add(type_panel,(4,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
