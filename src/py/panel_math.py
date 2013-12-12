#
# panel_math.py
#    read and edit .math
#
# Neil Gershenfeld
# CBA MIT 4/25/12
#
# (c) Massachusetts Institute of Technology 2012
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,string,os,sys
#
# panel class
#
class math_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # load file
      #
      def load_file(event):
         if (self.parent.basename == ""):
            return
         pos = string.find(self.parent.basename,".math")
         if (pos == -1):
            pos = string.find(self.parent.basename,".MATH")
            if (pos == -1):
               print 'math_panel: oops -- must be .math'
               sys.exit()
         self.parent.rootname = self.parent.basename[:pos]
         math_file = open(self.parent.filename,'r')
         math_string = math_file.read()
         math_file.close()
         self.text.SetValue(math_string)
      #
      # select file
      #
      def select_file(event):
         dialog = wx.FileDialog(self, "Choose a file", os.getcwd(), "", "*.math", wx.OPEN)
         if (dialog.ShowModal() == wx.ID_OK):
            self.parent.filename = dialog.GetPath()
            self.parent.basename = os.path.basename(self.parent.filename)
            pos = string.find(self.parent.basename,".math")
            if (pos == -1):
               print 'math_panel: oops -- must be .math'
               sys.exit()
            else:
               self.parent.rootname = self.parent.basename[:pos]
            math_file = open(self.parent.filename,'r')
            math_string = math_file.read()
            math_file.close()
            self.text.SetValue(math_string)
      #
      # save file
      #
      def save_file(event):
         result = wx.SaveFileSelector('.math','.math',self.parent.filename)
         if (result != ''):
            math_file = open(result,'w')
            math_file.write(self.text.GetValue())
            math_file.close()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label 
      #
      label = wx.StaticText(self,label='from: math')
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
      load = wx.Button(self,label='load .math')
      load.Bind(wx.EVT_BUTTON,select_file)
      self.sizer.Add(load,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      rload = wx.Button(self,label='reload .math')
      rload.Bind(wx.EVT_BUTTON,load_file)
      self.sizer.Add(rload,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      save = wx.Button(self,label='save .math')
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
