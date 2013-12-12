#
# panel_stl.py
#    read .stl
#
# Neil Gershenfeld
# CBA MIT 3/6/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,string,os,sys
from panel_path import path_panel
#
# panel class
#
class stl_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.stl_file = ''
      #
      # get stl info
      #
      def stl_info(name):
         #
         # construct command
         #
         temp_name = self.parent.tmp+'stl_info'
         command = 'stl_info '+'\"'+name+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         output = output_file.read()
         output_file.close()
         print output
         if (string.find(output,'must be binary') != -1):
            sys.exit(-1)
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         #
         # read limits
         #
         start = 6+string.find(output,'xmax:')
         space = string.find(output,' ',start)
         end = string.find(output,'ymin',space)-4
         self.parent.xmin = float(output[start:space])
         self.parent.xmax = float(output[1+space:end])
         start = 6+string.find(output,'ymax:')
         space = string.find(output,' ',start)
         end = string.find(output,'zmin',space)-4
         self.parent.ymin = float(output[start:space])
         self.parent.ymax = float(output[1+space:end])
         start = 6+string.find(output,'zmax:')
         space = string.find(output,' ',start)
         self.parent.zmin = float(output[start:space])
         self.parent.zmax = float(output[space+1:-1])
         #
         # move origin to top corner
         #
         #
         self.parent.xmax = self.parent.xmax - self.parent.xmin
         self.parent.xmin = 0
         self.parent.ymax = self.parent.ymax - self.parent.ymin
         self.parent.ymin = 0
         self.parent.zmin = self.parent.zmin - self.parent.zmax
         self.parent.zmax = 0
         #
         return output
      #
      # load file
      #
      def load_file(event):
         #
         # get file name
         #
         if (self.parent.basename == ""):
            return
         pos = string.find(self.parent.basename,".stl")
         if (pos == -1):
            pos = string.find(self.parent.basename,".STL")
            if (pos == -1):
               print 'stl_panel: oops -- must be .stl'
               sys.exit()
         self.parent.rootname = self.parent.basename[:pos]
         self.parent.stl_file = self.parent.filename
         #
         # get file info
         #
         info = stl_info(self.parent.filename)
         self.info.SetLabel(info)
         temp_name = self.parent.tmp+'stl.path'
         #
         # draw
         #
         if (self.path_viewer.view_type != "none"):
            units = 1.0
            resolution = 100
            command = 'stl_path '+'\"'+self.parent.stl_file+'\"'+' '+'\"'+temp_name+'\"'+' '+str(units)+' '+str(resolution)
            print command
            ret = os.system(command)
            if (ret == 0):
               self.path_viewer.draw(temp_name)
         #
         # fit
         #
         self.parent.Layout()
         self.parent.Fit()
      #
      # select file
      #
      def select_file(event):
         dialog = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.stl;*.STL", wx.OPEN)
         if (dialog.ShowModal() == wx.ID_OK):
            self.parent.filename = dialog.GetPath()
            self.parent.basename = os.path.basename(self.parent.filename)
            load_file(0)
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label 
      #
      label = wx.StaticText(self,label='from: stl')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # image
      #
      self.path_viewer = path_panel(self)
      self.sizer.Add(self.path_viewer,(1,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      self.path_viewer.view_type = "segments"
      #
      # controls
      #
      load = wx.Button(self,label='load .stl')
      load.Bind(wx.EVT_BUTTON,select_file)
      self.sizer.Add(load,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info = wx.StaticText(self,label="")
      self.sizer.Add(self.info,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # file
      #
      load_file(0)
      #
      # fit
      #
      self.Fit()
