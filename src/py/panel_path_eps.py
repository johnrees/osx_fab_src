#
# panel_path_eps.py
#    make .eps from .path
#
# Neil Gershenfeld 7/4/13
# (c) Massachusetts Institute of Technology 2013
#
# This work may be reproduced, modified, distributed,
# performed, and displayed for any purpose, but must
# acknowledge the fab modules project. Copyright is
# retained and must be preserved. The work is provided
# as is; no warranty is provided, and users accept all 
# liability.
#
# imports
#
import wx,os
#
# panel class
#
class path_eps_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make eps
      #
      def make_eps(event):
         if (self.parent.path_file == ''):
            print 'make_png_eps: oops -- need path file'
            return
         self.parent.eps_file = self.parent.tmp+self.parent.rootname+'.eps'
         command = 'path_eps '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.eps_file+'\"'
         print command
         os.system(command)
         self.button.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.eps_file+'\"'
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
      label = wx.StaticText(self,label='to: eps')
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
      make = wx.Button(self,label='make .eps')
      make.Bind(wx.EVT_BUTTON,make_eps)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
