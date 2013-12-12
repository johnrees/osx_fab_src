#
# panel_path_epi.py
#    make .epi from .path
#
# Neil Gershenfeld 8/19/13
# (c) Massachusetts Institute of Technology 2013
#
# This work may be reproduced, modified, distributed,
# performed, and displayed for any purpose, but must
# acknowledge the fab modules project. Copyright is
# retained and must be preserved. The work is provided
# as is; no warranty is provided, and users accept all 
# liability.
#

#
# imports
#
import wx,os,string
#
# panel class
#
class path_epi_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.path_file = ''
      #
      # make epi
      #
      def make_epi(event):
         if (self.parent.path_file == ''):
            print 'panel_path_epi: oops -- need path file'
            return
         self.parent.epi_file = self.parent.tmp+self.parent.rootname+'.epi'
         if (self.path_type == "2D"):
            power = self.power_2D.GetValue()
            if (self.focus_2D.GetValue()):
               focus = '1'
            else:
               focus = '0'
            xmin = self.xmin_2D.GetValue()
            ymin = self.ymin_2D.GetValue()
            speed = self.speed_2D.GetValue()
            rate = self.rate_2D.GetValue()
            command = 'path_epi '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.epi_file+'\"'+' '+power+' '+speed+' '+focus+' '+xmin+' '+ymin+' '+' '+rate
            print command
            os.system(command)
            self.button.Show()
            self.parent.Layout()
            self.parent.Fit()
         elif (self.path_type == "3D"):
            min_power = self.min_power_3D.GetValue()
            max_power = self.max_power_3D.GetValue()
            if (self.focus_3D.GetValue()):
               focus = '1'
            else:
               focus = '0'
            xmin = self.xmin_3D.GetValue()
            ymin = self.ymin_3D.GetValue()
            speed = self.speed_3D.GetValue()
            rate = self.rate_3D.GetValue()
            command = 'path_epi '+'\"'+self.parent.path_file+'\"'+' '+'\"'+self.parent.epi_file+'\"'+' '+min_power+' '+speed+' '+focus+' '+xmin+' '+ymin+' '+' '+rate+' '+max_power
            print command
            os.system(command)
            self.button.Show()
            self.parent.Layout()
            self.parent.Fit()
         else:
            print "panel_path_epi: oops -- don't recognize path type"
            return
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.epi_file+'\"'
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
      label = wx.StaticText(self,label='to: epi')
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
      make = wx.Button(self,label='make .epi')
      make.Bind(wx.EVT_BUTTON,make_epi)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # 2D panel
      #
      self.panel_2D = wx.Panel(self)
      sizer_2D = wx.GridBagSizer(10,10)
      self.panel_2D.SetSizer(sizer_2D)
      #
      sizer_2D.Add(wx.StaticText(self.panel_2D,label=' power (%)'),(0,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.power_2D = wx.TextCtrl(self.panel_2D,-1,'25')
      sizer_2D.Add(self.power_2D,(1,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      sizer_2D.Add(wx.StaticText(self.panel_2D,label='xmin (mm)'),(2,0),flag=wx.ALIGN_RIGHT)
      sizer_2D.Add(wx.StaticText(self.panel_2D,label='ymin (mm)'),(2,1),flag=wx.ALIGN_LEFT)
      #
      self.xmin_2D = wx.TextCtrl(self.panel_2D,-1,'10')
      sizer_2D.Add(self.xmin_2D,(3,0),flag=wx.ALIGN_RIGHT)
      self.ymin_2D = wx.TextCtrl(self.panel_2D,-1,'10')
      sizer_2D.Add(self.ymin_2D,(3,1),flag=wx.ALIGN_LEFT)
      #
      sizer_2D.Add(wx.StaticText(self.panel_2D,label='speed (%)'),(4,0),flag=wx.ALIGN_RIGHT)
      sizer_2D.Add(wx.StaticText(self.panel_2D,label='rate'),(4,1),flag=wx.ALIGN_LEFT)
      #
      self.speed_2D = wx.TextCtrl(self.panel_2D,-1,'75')
      sizer_2D.Add(self.speed_2D,(5,0),flag=wx.ALIGN_RIGHT)
      self.rate_2D = wx.TextCtrl(self.panel_2D,-1,'500')
      sizer_2D.Add(self.rate_2D,(5,1),flag=wx.ALIGN_LEFT)
      #
      self.focus_2D = wx.CheckBox(self.panel_2D,-1,'autofocus')
      sizer_2D.Add(self.focus_2D,(6,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.sizer.Add(self.panel_2D,(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      self.path_type = "2D"
      #
      # 3D panel
      #
      self.panel_3D = wx.Panel(self)
      sizer_3D = wx.GridBagSizer(10,10)
      self.panel_3D.SetSizer(sizer_3D)
      #
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='min z power (%)'),(0,0),flag=wx.ALIGN_RIGHT)
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='max z power (%)'),(0,1),flag=wx.ALIGN_LEFT)
      #
      self.min_power_3D = wx.TextCtrl(self.panel_3D,-1,'25')
      sizer_3D.Add(self.min_power_3D,(1,0),flag=wx.ALIGN_RIGHT)
      self.max_power_3D = wx.TextCtrl(self.panel_3D,-1,'25')
      sizer_3D.Add(self.max_power_3D,(1,1),flag=wx.ALIGN_LEFT)
      #
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='xmin (mm)'),(2,0),flag=wx.ALIGN_RIGHT)
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='ymin (mm)'),(2,1),flag=wx.ALIGN_LEFT)
      #
      self.xmin_3D = wx.TextCtrl(self.panel_3D,-1,'10')
      sizer_3D.Add(self.xmin_3D,(3,0),flag=wx.ALIGN_RIGHT)
      self.ymin_3D = wx.TextCtrl(self.panel_3D,-1,'10')
      sizer_3D.Add(self.ymin_3D,(3,1),flag=wx.ALIGN_LEFT)
      #
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='speed (%)'),(4,0),flag=wx.ALIGN_RIGHT)
      sizer_3D.Add(wx.StaticText(self.panel_3D,label='rate'),(4,1),flag=wx.ALIGN_LEFT)
      #
      self.speed_3D = wx.TextCtrl(self.panel_3D,-1,'75')
      sizer_3D.Add(self.speed_3D,(5,0),flag=wx.ALIGN_RIGHT)
      self.rate_3D = wx.TextCtrl(self.panel_3D,-1,'500')
      sizer_3D.Add(self.rate_3D,(5,1),flag=wx.ALIGN_LEFT)
      #
      self.focus_3D = wx.CheckBox(self.panel_3D,-1,'autofocus')
      sizer_3D.Add(self.focus_3D,(6,0),span=(1,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.panel_3D.Hide()
   #
   # parent call to update panel
   #
   def update_panel(self):
      if (string.find(self.parent.path_type,"3D") != -1):
         self.sizer.Remove(self.panel_2D)
         self.panel_2D.Hide()
         self.sizer.Add(self.panel_3D,(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
         self.panel_3D.Show()
      elif (string.find(self.parent.path_type,"2D") != -1):
         self.sizer.Remove(self.panel_3D)
         self.panel_3D.Hide()
         self.sizer.Add(self.panel_2D,(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
         self.panel_2D.Show()
      self.Layout()
      self.Fit()
