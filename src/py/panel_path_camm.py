#
# panel_path_camm.py
#    make .camm from .path
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
import wx,os
#
# panel class
#
class path_camm_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make file
      #
      def make_file(event):
         if (self.parent.path_file == ''):
            print 'panel_path_camm: oops -- need path file'
            return
         self.parent.camm_file = self.parent.tmp+self.parent.rootname+'.camm'
         force = self.force.GetValue()
         velocity = self.velocity.GetValue()
         command = 'path_camm '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.camm_file+'\"'+' '+force+' '+velocity
         print command
         os.system(command)
         self.button.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.camm_file+'\"'
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
      label = wx.StaticText(self,label='to: camm')
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
      make = wx.Button(self,label='make .camm')
      make.Bind(wx.EVT_BUTTON,make_file)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.sizer.Add(wx.StaticText(self,label='force (g)'),(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      self.force = wx.TextCtrl(self,-1,'45')
      self.sizer.Add(self.force,(4,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.sizer.Add(wx.StaticText(self,label='velocity (cm/2)'),(5,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      self.velocity = wx.TextCtrl(self,-1,'2')
      self.sizer.Add(self.velocity,(6,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
