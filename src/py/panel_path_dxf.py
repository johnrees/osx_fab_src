#
# panel_path_dxf.py
#    make .dxf from .path
#
# Neil Gershenfeld
# CBA MIT 8/25/12
#
# (c) Massachusetts Institute of Technology 2012
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os
#
# panel class
#
class path_dxf_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make dxf
      #
      def make_dxf(event):
         if (self.parent.path_file == ''):
            print 'panel_path_dxf: oops -- need path file'
            return
         self.parent.dxf_file = self.parent.tmp+self.parent.rootname+'.dxf'
         command = 'path_dxf '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.dxf_file+'\"'
         print command
         os.system(command)
         self.button.SetMaxSize((self.parent.xsize,self.parent.ysize))
         self.button.SetMinSize((self.parent.xsize,self.parent.ysize))
         self.button.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.dxf_file+'\"'
         print command
         os.system(command)
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label
      #
      label = wx.StaticText(self,label='to: dxf')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # send
      #
      self.button = wx.Button(self,label='send it!')
      self.button.Bind(wx.EVT_BUTTON,fab_send)
      self.button.SetFont(bold_font)
      self.sizer.Add(self.button,(1,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      self.button.Hide()
      #
      # controls
      #
      make = wx.Button(self,label='make .dxf')
      make.Bind(wx.EVT_BUTTON,make_dxf)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
