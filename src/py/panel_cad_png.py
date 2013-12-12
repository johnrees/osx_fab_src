#
# panel_cad_png.py
#    make .png from .cad
#
# Neil Gershenfeld
# CBA MIT 4/18/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os,string
#
# panel class
#
class cad_png_panel(wx.Panel):
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
         #
         # generate .math file
         #
         tmp_cad_file = self.parent.tmp+self.parent.rootname+'.cad'
         cad_file = open(tmp_cad_file,'w')
         cad_file.write(self.parent.cad_panel.text.GetValue())
         cad_file.close()
         cad_info_name = self.parent.tmp+'cad_info'
         math_file = self.parent.tmp+self.parent.rootname+'.math'
         command = 'cad_math '+'\"'+tmp_cad_file+'\"'+' '+'\"'+math_file+'\"'+' > '+'\"'+cad_info_name+'\"'
         print command
         os.system(command)
         cad_info_file = open(cad_info_name,'r')
         cad_info = cad_info_file.read()
         cad_info_file.close()
         start = string.find(cad_info,"write")
         cad_info = cad_info[start:]
         start = 7+string.find(cad_info,'units:')
         end = string.find(cad_info,'\n',start)
         units = float(cad_info[start:end])
         start = 4+string.find(cad_info,'dz:')
         end = string.find(cad_info,'\n',start)
         dz = float(cad_info[start:end])
         start = 6+string.find(cad_info,'zmin:')
         end = string.find(cad_info,'\n',start)
         zmin = float(cad_info[start:end])
         start = 8+string.find(cad_info,'layers:')
         resolution = self.parent.cad_png_panel.resolution.GetValue()
         if (start > 7):
            end = string.find(cad_info,'\n',start)
            number = int(cad_info[start:end])
         elif (dz > 0):
            number = int(dz*units*float(resolution))
         else:
            number = 1
         parent.zmin = zmin
         parent.zmax = zmin+dz
         parent.units = units
         #
         # generate .png file
         #
         self.parent.png_file = self.parent.tmp+self.parent.rootname+'.png'
         command = 'math_png '+'\"'+math_file+'\"'+' '+'\"'+self.parent.png_file+'\"'+' '+resolution+' '+str(number)
         print command
         os.system(command)
         #
         # update image
         #
         info = png_info(self.parent.png_file)
         self.info.SetLabel(cad_info+info)
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
      self.resolution = wx.TextCtrl(res_panel,-1,'1')
      res_panel_sizer.Add(self.resolution,(0,1),flag=wx.ALIGN_LEFT)
      self.sizer.Add(res_panel,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info = wx.StaticText(self,label="")
      self.sizer.Add(self.info,(5,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
