#
# panel_control.py
#    control panel
#
# Neil Gershenfeld
# CBA MIT 3/6/11
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
class control_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # quit
      #
      def quits(event):
         command = 'ls '+'\"'+self.parent.tmp+'\"'+'*; rm '+'\"'+self.parent.tmp+'\"'+'*'
         print command
         os.system(command)
         sys.exit()
      #
      # controls
      #
      self.defaults = wx.ComboBox(self,value='defaults',style=wx.CB_READONLY)
      self.defaults.Bind(wx.EVT_COMBOBOX,self.parent.defaults_handler)
      self.sizer.Add(self.defaults,(0,0))
      #
      d = 4 
      w = 1 
      l = 6*d+2*w
      def logo_paint(event):
         dc = wx.PaintDC(self.logo_panel)
         dc.SetBrush(wx.Brush('white'))
         dc.SetPen(wx.Pen('white', 0))
         dc.DrawRectangleRect((0, 0, l, l))
         dc.SetBrush(wx.Brush('red'))
         dc.DrawCircle(d,d,d)
         dc.DrawCircle(3*d+w,3*d+w,d)
         dc.SetBrush(wx.Brush('blue'))
         dc.DrawRectangleRect((2*d+w,0,2*d,2*d))
         dc.DrawRectangleRect((4*d+2*w,0,2*d,2*d))
         dc.DrawRectangleRect((0,2*d+w,2*d,2*d))
         dc.DrawRectangleRect((4*d+2*w,2*d+w,2*d,2*d))
         dc.DrawRectangleRect((0,4*d+2*w,2*d,2*d))
         dc.DrawRectangleRect((2*d+w,4*d+2*w,2*d,2*d))
         dc.DrawRectangleRect((4*d+2*w,4*d+2*w,2*d,2*d))
      self.logo_panel = wx.Panel(self,size=(l,l))
      self.logo_panel.Bind(wx.EVT_PAINT,logo_paint)
      self.sizer.Add(self.logo_panel,(0,1))
      #
      quit = wx.Button(self,label='quit')
      quit.Bind(wx.EVT_BUTTON,quits)
      self.sizer.Add(quit,(0,2))
      #
      # fit
      #
      self.Fit()
