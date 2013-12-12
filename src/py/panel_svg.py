#
# panel_svg.py
#    read and edit .svg
#
# Neil Gershenfeld
# CBA MIT 7/13/11
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
class svg_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # load file
      #
      def load_file(event):
         if (self.parent.basename == ""):
            return
         pos = string.find(self.parent.basename,".svg")
         if (pos == -1):
            pos = string.find(self.parent.basename,".SVG")
            if (pos == -1):
               print 'svg_panel: oops -- must be .svg'
               sys.exit()
         self.parent.rootname = self.parent.basename[:pos]
         svg_file = open(self.parent.filename,'r')
         svg_string = svg_file.read()
         svg_file.close()
         self.text.SetValue(svg_string)
      #
      # select file
      #
      def select_file(event):
         dialog = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.svg", wx.OPEN)
         if (dialog.ShowModal() == wx.ID_OK):
            self.parent.filename = dialog.GetPath()
            self.parent.basename = os.path.basename(self.parent.filename)
            pos = string.find(self.parent.basename,".svg")
            if (pos == -1):
               print 'svg_panel: oops -- must be .svg'
               sys.exit()
            else:
               self.parent.rootname = self.parent.basename[:pos]
            svg_file = open(self.parent.filename,'r')
            svg_string = svg_file.read()
            svg_file.close()
            self.text.SetValue(svg_string)
      #
      # save file
      #
      def save_file(event):
         result = wx.SaveFileSelector('.svg','.svg',self.parent.filename)
         if (result != ''):
            svg_file = open(result,'w')
            svg_file.write(self.text.GetValue())
            svg_file.close()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label 
      #
      label = wx.StaticText(self,label='from: svg')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # text
      #
      self.text = wx.TextCtrl(self,-1,'',size=(self.parent.size,self.parent.size),style=wx.TE_MULTILINE)
      self.sizer.Add(self.text,(1,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      #
      # controls
      #
      load = wx.Button(self,label='load .svg')
      load.Bind(wx.EVT_BUTTON,select_file)
      self.sizer.Add(load,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      rload = wx.Button(self,label='reload .svg')
      rload.Bind(wx.EVT_BUTTON,load_file)
      self.sizer.Add(rload,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      save = wx.Button(self,label='save .svg')
      save.Bind(wx.EVT_BUTTON,save_file)
      self.sizer.Add(save,(4,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # file
      #
      load_file(0)
      #
      # fit
      #
      self.Fit()
