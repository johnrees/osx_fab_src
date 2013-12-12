#
# panel_png_grb.py
#    make .grb from .png
#
# Neil Gershenfeld
# CBA MIT 11/23/12
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
class png_grb_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make grb 
      #
      def make_grb(event):
         if (self.parent.png_file == ''):
            print 'make_png_grb: oops -- need .png file'
            return
         self.parent.grb_file = self.parent.tmp+self.parent.rootname+'.grb'
         command = 'png_grb '+'\"'+self.parent.png_file+'\"'+' '+'\"'+self.parent.grb_file+'\"'
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
         command = 'fab_send '+'\"'+self.parent.grb_file+'\"'
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
      label = wx.StaticText(self,label='to: Gerber')
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
      make = wx.Button(self,label='make .grb')
      make.Bind(wx.EVT_BUTTON,make_grb)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
