#
# panel_cad.py
#    read and edit .cad
#
# Neil Gershenfeld
# CBA MIT 2/18/11
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
class cad_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # load file
      #
      def load_file(event):
         if (self.parent.basename == ""):
            return
         pos = string.find(self.parent.basename,".cad")
         if (pos == -1):
            pos = string.find(self.parent.basename,".CAD")
            if (pos == -1):
               print 'cad_panel: oops -- must be .cad'
               sys.exit()
         self.parent.rootname = self.parent.basename[:pos]
         cad_file = open(self.parent.filename,'r')
         cad_string = cad_file.read()
         cad_file.close()
         self.text.SetValue(cad_string)
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
      # save file
      #
      def save_file(event):
         result = wx.SaveFileSelector('.cad','.cad',self.parent.filename)
         if (result != ''):
            cad_file = open(result,'w')
            cad_file.write(self.text.GetValue())
            cad_file.close()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label 
      #
      label = wx.StaticText(self,label='from: cad')
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
      load = wx.Button(self,label='load .cad')
      load.Bind(wx.EVT_BUTTON,select_file)
      self.sizer.Add(load,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      rload = wx.Button(self,label='reload .cad')
      rload.Bind(wx.EVT_BUTTON,load_file)
      self.sizer.Add(rload,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      save = wx.Button(self,label='save .cad')
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
