#
# panel_math_stl.py
#    make .stl from .math
#
# Neil Gershenfeld
# CBA MIT 9/25/12
#
# (c) Massachusetts Institute of Technology 2012
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os,string
#
# panel class
#
class math_stl_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      #
      # get stl info
      #
      def stl_info(name):
         temp_name = self.parent.tmp+'stl_info'
         command = 'stl_info '+'\"'+name+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         output = output_file.read()
         output_file.close()
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         return output
      #
      # make stl
      #
      def make_stl(event):
         if (self.parent.rootname == ''):
            return

         math_text = self.parent.math_panel.text.GetValue()
         tmp_math_file = self.parent.tmp+self.parent.rootname+'.math'
         math_file = open(tmp_math_file,'w')
         math_file.write(math_text)
         math_file.close()
         start = string.find(math_text,"dx dy dz:")
         if (start == -1):
            start = string.find(math_text,"dx dy:")
            if (start == -1):
               print "panel_math_png: oops, can't find bounding box"
               return
            dz = 0
         else:
            end = string.find(math_text,"\n",start)
            array = [float(s) for s in math_text[(start+10):end].split()]
            dz = array[2]
         start = string.find(math_text,"xmin ymin zmin:")
         if (start == -1):
            start = string.find(math_text,"xmin ymin:")
            if (start == -1):
               print "panel_math_png: oops, can't find origin"
               return
            zmin = 0
         else:
            end = string.find(math_text,"\n",start)
            array = [float(s) for s in math_text[(start+15):end].split()]
            zmin = array[2]
         start = string.find(math_text,"mm per unit:")
         if (start == -1):
            print "panel_math_png: oops, can't find units"
            return
         end = string.find(math_text,"\n",start)
         units = float(math_text[(start+12):end])
         print units
         parent.zmin = zmin
         parent.zmax = zmin+dz
         parent.units = units

         self.parent.stl_file = self.parent.tmp+self.parent.rootname+'.stl'
         resolution = self.parent.stl_panel.resolution.GetValue()
         command = 'math_stl '+'\"'+tmp_math_file+'\"'+' '+'\"'+self.parent.stl_file+'\"'+' '+resolution
         print command
         os.system(command)
         info = stl_info(self.parent.stl_file)
         self.info.SetLabel(info)
         self.button.Show()
         self.parent.Layout()
         self.parent.Fit()
      #
      # send
      #
      def fab_send(event):
         command = 'fab_send '+'\"'+self.parent.stl_file+'\"'
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
      label = wx.StaticText(self,label='to: stl')
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
      make = wx.Button(self,label='make .stl')
      make.Bind(wx.EVT_BUTTON,make_stl)
      self.sizer.Add(make,(2,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      res_panel = wx.Panel(self)
      res_panel_sizer = wx.GridBagSizer(10,10)
      res_panel.SetSizer(res_panel_sizer)
      res_panel_sizer.Add(wx.StaticText(res_panel,label='resolution (pixels/mm):'),(0,0),flag=wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
      self.resolution = wx.TextCtrl(res_panel,-1,'1')
      res_panel_sizer.Add(self.resolution,(0,1),flag=wx.ALIGN_LEFT)
      self.sizer.Add(res_panel,(3,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      self.info = wx.StaticText(self,label="")
      self.sizer.Add(self.info,(5,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # fit
      #
      self.Fit()
