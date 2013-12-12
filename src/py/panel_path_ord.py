#
# panel_path_ord.py
#    make .ord from .path
#
# Neil Gershenfeld
# CBA MIT 2/26/11
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
class path_ord_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make file
      #
      def make_file(event):
         if (self.parent.path_file == ''):
            print 'panel_path_ord: oops -- need path file'
            return
         self.parent.ord_file = self.parent.tmp+self.parent.rootname+'.ord'
         lead = self.lead.GetValue()
         quality = self.quality.GetValue()
         command = 'path_ord '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.ord_file+'\"'+' '+lead+' '+quality
         print command
         os.system(command)
         self.button.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.ord_file+'\"'
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
      label = wx.StaticText(self,label='to: ord')
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
      make = wx.Button(self,label='make .ord')
      make.Bind(wx.EVT_BUTTON,make_file)
      self.sizer.Add(make,(2,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      row3_panel = wx.Panel(self)
      row3_sizer = wx.GridBagSizer(10,10)
      row3_panel.SetSizer(row3_sizer)
      row3_sizer.Add(wx.StaticText(row3_panel,label='lead in/out (mm)'),(0,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      row3_sizer.Add(wx.StaticText(row3_panel,label='cut quality         '),(0,1),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.sizer.Add(row3_panel,(3,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      row4_panel = wx.Panel(self)
      row4_sizer = wx.GridBagSizer(10,10)
      row4_panel.SetSizer(row4_sizer)
      self.lead= wx.TextCtrl(row4_panel,-1,'2')
      row4_sizer.Add(self.lead,(0,0))
      self.quality= wx.TextCtrl(row4_panel,-1,'-3')
      row4_sizer.Add(self.quality,(0,1))
      self.sizer.Add(row4_panel,(4,0),span=(1,2),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      # fit
      #
      self.Fit()
