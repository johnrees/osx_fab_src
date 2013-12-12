#
# panel_png.py
#    read .png
#
# Neil Gershenfeld
# CBA MIT 2/19/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,string,os,sys
#
# panel class
#
class png_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # get png info 
      #
      def png_info(name):
         temp_name = self.parent.tmp+'png_dimension'
         command = 'png_size '+'\"'+name+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         output = output_file.read()
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         print output
         self.parent.units = 1
         self.parent.xmin = 0
         start = string.find(output,"dx:")
         end = string.find(output,"mm",start)
         self.parent.xmax = float(output[start+4:end])
         self.parent.ymin = 0
         start = string.find(output,"dy:")
         end = string.find(output,"mm",start)
         self.parent.ymax = float(output[start+4:end])
         if "dz:" in output:
            self.parent.zmin = 0
            start = string.find(output,"dz:")
            end = string.find(output,"mm",start)
            self.parent.zmax = float(output[start+4:end])
         return output
      #
      # file open dialog
      #
      def file_dialog(event):
         dialog = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.png", wx.OPEN)
         if (dialog.ShowModal() == wx.ID_OK):
            self.parent.filename = dialog.GetPath()
            self.parent.basename = os.path.basename(self.parent.filename)
            load_file(0)
      #
      # load file
      #
      def load_file(event):
         if (self.parent.basename == ""):
            return
         pos = string.find(self.parent.basename,".png")
         if (pos == -1):
            pos = string.find(self.parent.basename,".PNG")
            if (pos == -1):
               print 'png_panel: oops -- must be .png'
               sys.exit()
         self.parent.rootname = self.parent.basename[:pos]
         self.parent.png_file = self.parent.filename
         info = png_info(self.parent.filename)
         self.info.SetLabel(info)
         png_image = wx.Image(self.parent.filename)
         (nx,ny) = png_image.GetSize()
         pixels = "size: (%d,%d)"%(nx,ny)
         ratio = float(ny)/float(nx)
         if (ratio > 1):
            self.parent.ysize = self.parent.size
            self.parent.xsize = int(self.parent.size/ratio)
         else:
            self.parent.ysize = int(self.parent.size*ratio)
            self.parent.xsize = self.parent.size
         wx.Image.Rescale(png_image,self.parent.xsize,self.parent.ysize)
         png_bitmap = png_image.ConvertToBitmap()
         self.bitmap.SetBitmap(png_bitmap)
         self.bitmap.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # select file
      #
      def select_file(event):
         dialog = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.cad", wx.OPEN)
         if (dialog.ShowModal() == wx.ID_OK):
            self.parent.filename = dialog.GetPath()
            self.parent.basename = os.path.basename(self.parent.filename)
            pos = string.find(self.parent.basename,".cad")
            if (pos == -1):
               print 'cad_panel: oops -- must be .cad'
               sys.exit()
            else:
               self.parent.rootname = self.parent.basename[:pos]
            cad_file = open(self.parent.filename,'r')
            cad_string = cad_file.read()
            cad_file.close()
            self.text.SetValue(cad_string)
      #
      # invert 
      #
      def invert_dialog(event):
         command = 'png_scale '+'\"'+self.parent.png_file+'\" \"'+self.parent.png_file+'\" 1 0'
         print command
         os.system(command)
         load_file(0)
      #
      # resize
      #
      def resize_dialog(event):
         resize.dx = self.parent.xmax
         resize.dy = self.parent.ymax
         resize_width.SetValue(str(resize.dx))
         resize_height.SetValue(str(resize.dy))
         resize.Show()
      def resize_dialog_cancel(event):
         resize.Hide()
      def resize_dialog_resize(event):
         width = resize_width.GetValue()
         height = resize_height.GetValue()
         print width,height
         command = 'png_size '+'\"'+self.parent.png_file+'\" '+width+' '+height
         print command
         os.system(command)
         load_file(0)
         resize.Hide()
      def resize_width_handler(event):
         if resize.prop.GetValue():
            dx = float(resize_width.GetValue())
            dy = dx * resize.dy/resize.dx
            resize_height.Unbind(wx.EVT_TEXT)
            resize_height.SetValue(str(dy))
            resize_height.Bind(wx.EVT_TEXT,resize_height_handler)
      def resize_height_handler(event):
         if resize.prop.GetValue():
            dy = float(resize_height.GetValue())
            dx = dy * resize.dx/resize.dy
            resize_width.Unbind(wx.EVT_TEXT)
            resize_width.SetValue(str(dx))
            resize_width.Bind(wx.EVT_TEXT,resize_width_handler)
      #
      resize = wx.Frame(None, -1, 'resize')
      resize_sizer = wx.GridBagSizer(10,10)
      resize.SetSizer(resize_sizer)
      #
      resize_sizer.Add(wx.StaticText(resize,label='width (mm):'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      resize_width = wx.TextCtrl(resize,-1,'0')
      resize_width.Bind(wx.EVT_TEXT,resize_width_handler)
      resize_sizer.Add(resize_width,(0,1))
      #
      resize_sizer.Add(wx.StaticText(resize,label='height (mm):'),(0,2),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      resize_height = wx.TextCtrl(resize,-1,'0')
      resize_height.Bind(wx.EVT_TEXT,resize_height_handler)
      resize_sizer.Add(resize_height,(0,3))
      #
      resize.prop = wx.CheckBox(resize,label='proportional',style=wx.RB_GROUP)
      resize.prop.SetValue(True)
      resize_sizer.Add(resize.prop,(1,1))
      #
      resize_resize = wx.Button(resize,label='resize')
      resize_resize.Bind(wx.EVT_BUTTON,resize_dialog_resize)
      resize_sizer.Add(resize_resize,(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      resize_cancel = wx.Button(resize,label='cancel')
      resize_cancel.Bind(wx.EVT_BUTTON,resize_dialog_cancel)
      resize_sizer.Add(resize_cancel,(1,3),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      resize.Fit()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label
      #
      label = wx.StaticText(self,label='from: png')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # image
      #
      self.bitmap = wx.StaticBitmap(self)
      self.sizer.Add(self.bitmap,(1,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      self.bitmap.Hide()
      def mouse_move(event):
         x = self.parent.xmin + (self.parent.xmax-self.parent.xmin)*event.GetX()/float(self.parent.xsize)
         y = self.parent.ymin + (self.parent.ymax-self.parent.ymin)*(self.parent.ysize-event.GetY())/float(self.parent.ysize)
         self.position.SetLabel("x: %.3f mm  y: %.3f mm"%(x,y))
         self.Layout()
         self.Fit()
      self.bitmap.Bind(wx.EVT_MOTION,mouse_move)
      #
      # controls
      #
      control_panel = wx.Panel(self)
      control_sizer = wx.GridBagSizer(10,10)
      control_panel.SetSizer(control_sizer)
      #
      button = wx.Button(control_panel,label='load .png')
      button.Bind(wx.EVT_BUTTON,file_dialog)
      control_sizer.Add(button,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      button = wx.Button(control_panel,label='resize .png')
      button.Bind(wx.EVT_BUTTON,resize_dialog)
      control_sizer.Add(button,(0,1),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      button = wx.Button(control_panel,label='invert .png')
      button.Bind(wx.EVT_BUTTON,invert_dialog)
      control_sizer.Add(button,(0,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.sizer.Add(control_panel,(2,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      self.position= wx.StaticText(self,label="")
      self.sizer.Add(self.position,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info= wx.StaticText(self,label="")
      self.sizer.Add(self.info,(4,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # file
      #
      load_file(0)
      #
      # fit
      #
      self.Fit()
   #
   # parent call to update size
   #
   def update_size(self,sizex,sizey):
      self.Layout()
      self.Fit()
