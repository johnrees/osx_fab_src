#
# panel_cad_stl.py
#    make .stl from .cad
#
# Neil Gershenfeld
# CBA MIT 3/22/11
#
# (c) Massachusetts Institute of Technology 2011
# Permission granted for experimental and personal use;
# license for commercial sale available from MIT.
#
# imports
#
import wx,os,string
#
# panel class
#
class cad_stl_panel(wx.Panel):
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
         tmp_cad_file = self.parent.tmp+self.parent.rootname+'.cad'
         cad_file = open(tmp_cad_file,'w')
         cad_file.write(self.parent.cad_panel.text.GetValue())
         cad_file.close()
         cad_info_name = self.parent.tmp+'cad_info'
         math_file = self.parent.tmp+self.parent.rootname+'.math'
         command = 'cad_math '+'\"'+tmp_cad_file+'\"'+' '+'\"'+math_file+'\"'+' > '+'\"'+cad_info_name+'\"'
         print command
         os.system(command)
         cad_info_file = open(cad_info_name,'r')
         cad_info = cad_info_file.read()
         cad_info_file.close()
         start = string.find(cad_info,"write")
         cad_info = cad_info[start:]
         start = 7+string.find(cad_info,'units:')
         end = string.find(cad_info,'\n',start)
         units = float(cad_info[start:end])
         start = 4+string.find(cad_info,'dz:')
         end = string.find(cad_info,'\n',start)
         dz = float(cad_info[start:end])
         start = 6+string.find(cad_info,'zmin:')
         end = string.find(cad_info,'\n',start)
         zmin = float(cad_info[start:end])
         parent.zmin = zmin
         parent.zmax = zmin+dz
         parent.units = units
         self.parent.stl_file = self.parent.tmp+self.parent.rootname+'.stl'
         resolution = self.parent.stl_panel.resolution.GetValue()
         command = 'math_stl '+'\"'+math_file+'\"'+' '+'\"'+self.parent.stl_file+'\"'+' '+resolution
         print command
         os.system(command)
         info = stl_info(self.parent.stl_file)
         self.info.SetLabel(cad_info+info)
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
