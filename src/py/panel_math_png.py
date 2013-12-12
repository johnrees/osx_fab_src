#
# panel_math_png.py
#    make .png from .math
#
# Neil Gershenfeld
# CBA MIT 5/19/12
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
class math_png_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # get png info
      #
      def png_info(name):
         temp_name = self.parent.tmp+'png_info'
         command = 'png_size '+'\"'+name+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         output = output_file.read()
         output_file.close()
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         return output
      #
      # make png
      #
      def make_png(event):
         if (self.parent.rootname == ''):
            return
         math_text = self.parent.math_panel.text.GetValue()
         tmp_math_file = self.parent.tmp+self.parent.rootname+'.math'
         math_file = open(tmp_math_file,'w')
         math_file.write(math_text)
         math_file.close()
         start = string.find(math_text,"dx dy dz:")
         if (start == -1):
            start = string.find(math_text,"dx dy:")
            if (start == -1):
               print "panel_math_png: oops, can't find bounding box"
               return
            dz = 0
         else:
            end = string.find(math_text,"\n",start)
            array = [float(s) for s in math_text[(start+10):end].split()]
            dz = array[2]
         start = string.find(math_text,"xmin ymin zmin:")
         if (start == -1):
            start = string.find(math_text,"xmin ymin:")
            if (start == -1):
               print "panel_math_png: oops, can't find origin"
               return
            zmin = 0
         else:
            end = string.find(math_text,"\n",start)
            array = [float(s) for s in math_text[(start+15):end].split()]
            zmin = array[2]
         start = string.find(math_text,"mm per unit:")
         if (start == -1):
            print "panel_math_png: oops, can't find units"
            return
         end = string.find(math_text,"\n",start)
         units = float(math_text[(start+12):end])
         print units
         parent.zmin = zmin
         parent.zmax = zmin+dz
         parent.units = units
         resolution = self.parent.math_png_panel.resolution.GetValue()
         self.parent.png_file = self.parent.tmp+self.parent.rootname+'.png'
         command = 'math_png '+'\"'+tmp_math_file+'\"'+' '+'\"'+self.parent.png_file+'\"'+' '+resolution
         print command
         os.system(command)
         info = png_info(self.parent.png_file)
         self.info.SetLabel(info)
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
         # update panels
         #
         parent.update_panels()
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
      make.Bind(wx.EVT_BUTTON,make_png)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      res_panel = wx.Panel(self)
      res_panel_sizer = wx.GridBagSizer(10,10)
      res_panel.SetSizer(res_panel_sizer)
      res_panel_sizer.Add(wx.StaticText(res_panel,label='resolution (pixels/mm):'),(0,0),flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
      self.resolution = wx.TextCtrl(res_panel,-1,'10')
      res_panel_sizer.Add(self.resolution,(0,1),flag=wx.ALIGN_LEFT)
      self.sizer.Add(res_panel,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info = wx.StaticText(self,label="")
      self.sizer.Add(self.info,(5,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
