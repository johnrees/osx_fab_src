#
# panel_path_oms.py
#    make .oms from .path
#
# Neil Gershenfeld
# CBA MIT 5/25/13
#
# (c) Massachusetts Institute of Technology 2013
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os
#
# panel class
#
class path_oms_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make oms
      #
      def make_oms(event):
         if (self.parent.path_file == ''):
            print 'panel_path_oms: oops -- need path file'
            return
         self.parent.oms_file = self.parent.tmp+self.parent.rootname+'.oms'
         velocity = self.velocity.GetValue()
         accel = self.accel.GetValue()
         period = self.period.GetValue()
         command = 'path_oms '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.oms_file+'\"'+' '+velocity+' '+accel+' '+period
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
         command = 'fab_send '+'\"'+self.parent.oms_file+'\"'
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
      label = wx.StaticText(self,label='to: oms')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # send
      #
      self.button = wx.Button(self,label='send it!')
      self.button.Bind(wx.EVT_BUTTON,fab_send)
      self.button.SetFont(bold_font)
      self.sizer.Add(self.button,(1,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL))
      self.button.Hide()
      #
      # controls
      #
      make = wx.Button(self,label='make .oms')
      make.Bind(wx.EVT_BUTTON,make_oms)
      self.sizer.Add(make,(2,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      row3_panel = wx.Panel(self)
      row3_sizer = wx.GridBagSizer(10,10)
      row3_panel.SetSizer(row3_sizer)
      row3_sizer.Add(wx.StaticText(row3_panel,label='velocity'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.sizer.Add(row3_panel,(3,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row4_panel = wx.Panel(self)
      row4_sizer = wx.GridBagSizer(10,10)
      row4_panel.SetSizer(row4_sizer)
      self.velocity = wx.TextCtrl(row4_panel,-1,'0.1')
      row4_sizer.Add(self.velocity,(0,0))
      self.sizer.Add(row4_panel,(4,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row5_panel = wx.Panel(self)
      row5_sizer = wx.GridBagSizer(10,10)
      row5_panel.SetSizer(row5_sizer)
      row5_sizer.Add(wx.StaticText(row5_panel,label='acceleration'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.sizer.Add(row5_panel,(5,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row6_panel = wx.Panel(self)
      row6_sizer = wx.GridBagSizer(10,10)
      row6_panel.SetSizer(row6_sizer)
      self.accel = wx.TextCtrl(row6_panel,-1,'5.0')
      row6_sizer.Add(self.accel,(0,0))
      self.sizer.Add(row6_panel,(6,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row7_panel = wx.Panel(self)
      row7_sizer = wx.GridBagSizer(10,10)
      row7_panel.SetSizer(row7_sizer)
      row7_sizer.Add(wx.StaticText(row7_panel,label='period'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.sizer.Add(row7_panel,(7,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row8_panel = wx.Panel(self)
      row8_sizer = wx.GridBagSizer(10,10)
      row8_panel.SetSizer(row8_sizer)
      self.period = wx.TextCtrl(row8_panel,-1,'10000')
      row8_sizer.Add(self.period,(0,0))
      self.sizer.Add(row8_panel,(8,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
