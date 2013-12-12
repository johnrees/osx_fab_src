#
# panel_svg_path.py
#    make .path from .svg
#
# Neil Gershenfeld 9/8/13
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
from panel_path import path_panel
#
# panel class
#
class svg_path_panel(wx.Panel):
   def __init__(self,parent):
      self.parent = parent
      self.parent.zmin = 0
      self.parent.zmax = 0
      self.parent.units = 1
      #
      # make path
      #
      def make_path(event):
         if (self.parent.rootname == ''):
            return
         tmp_svg_file = self.parent.tmp+self.parent.rootname+'.svg'
         svg_file = open(tmp_svg_file,'w')
         svg_file.write(self.parent.svg_panel.text.GetValue())
         svg_file.close()
         self.parent.path_file = self.parent.tmp+self.parent.rootname+'.path'
         path_png = self.parent.tmp+self.parent.rootname+'.path.png'
         if (self.path_type.GetValue() == '3D'):
            scale = self.scale_3D.GetValue()
            points = self.points_3D.GetValue()
            resolution = self.resolution_3D.GetValue()
            zmin = self.zmin.GetValue()
            zmax = self.zmax.GetValue()
            command = 'svg_path '+'\"'+tmp_svg_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+scale+' '+points+' '+resolution+' '+zmin+' '+zmax
            print command
            os.system(command)
            temp_name = self.parent.tmp+'path_info'
            command = 'path_info '+'\"'+self.parent.path_file+'\"'+' > '+'\"'+temp_name+'\"'
            os.system(command)
            output_file = open(temp_name,'r')
            output = output_file.read()
            output_file.close()
            command = 'rm '+'\"'+temp_name+'\"'
            os.system(command)
            self.info_3D.SetLabel(output)
         elif (self.path_type.GetValue() == '2D'):
            scale = self.scale_2D.GetValue()
            points = self.points_2D.GetValue()
            resolution = self.resolution_2D.GetValue()
            command = 'svg_path '+'\"'+tmp_svg_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+scale+' '+points+' '+resolution
            print command
            os.system(command)
            temp_name = self.parent.tmp+'path_info'
            command = 'path_info '+'\"'+self.parent.path_file+'\"'+' > '+'\"'+temp_name+'\"'
            os.system(command)
            output_file = open(temp_name,'r')
            output = output_file.read()
            output_file.close()
            command = 'rm '+'\"'+temp_name+'\"'
            os.system(command)
            self.info_2D.SetLabel(output)
         self.path_viewer.draw(self.parent.path_file)
         self.parent.Layout()
         self.parent.Fit()
      #
      # panel
      #
      wx.Panel.__init__(self,parent)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # label
      #
      label = wx.StaticText(self,label='to: path')
      bold_font = wx.Font(10,wx.DEFAULT,wx.NORMAL,wx.BOLD)
      label.SetFont(bold_font)
      self.sizer.Add(label,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # image
      #
      self.path_viewer = path_panel(self)
      self.sizer.Add(self.path_viewer,(1,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      #
      # controls
      #
      make_panel = wx.Panel(self)
      make_sizer = wx.GridBagSizer(10,10)
      make_panel.SetSizer(make_sizer)
      make_button = wx.Button(make_panel,label='make .path')
      make_button.Bind(wx.EVT_BUTTON,make_path)
      make_sizer.Add(make_button,(0,0),flag=wx.ALIGN_CENTER_HORIZONTAL)
      make_sizer.Add(wx.StaticText(make_panel,label='type:'),(0,1),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.path_types = ["2D","3D"]
      self.path_type = wx.ComboBox(make_panel,size=(100,-1),value="2D",choices=self.path_types,style=wx.CB_READONLY)
      self.path_type.Bind(wx.EVT_COMBOBOX,self.path_type_handler)
      make_sizer.Add(self.path_type,(0,2),flag=wx.ALIGN_LEFT)
      self.sizer.Add(make_panel,(2,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      # 2D panel
      #
      panel_2D = wx.Panel(self)
      sizer_2D = wx.GridBagSizer(10,10)
      panel_2D.SetSizer(sizer_2D)
      #
      sizer_2D.Add(wx.StaticText(panel_2D,label='scale factor'),(0,1),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.scale_2D = wx.TextCtrl(panel_2D,-1,'1.0')
      sizer_2D.Add(self.scale_2D,(0,2),flag=wx.ALIGN_LEFT)
      #
      sizer_2D.Add(wx.StaticText(panel_2D,label='curve points'),(1,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.points_2D = wx.TextCtrl(panel_2D,-1,'10')
      sizer_2D.Add(self.points_2D,(1,1),flag=wx.ALIGN_LEFT)
      self.resolution_2D = wx.TextCtrl(panel_2D,-1,'10000')
      sizer_2D.Add(self.resolution_2D,(1,2),flag=wx.ALIGN_RIGHT)
      sizer_2D.Add(wx.StaticText(panel_2D,label='path resolution'),(1,3),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT))
      #
      self.info_2D = wx.StaticText(panel_2D,label="")
      sizer_2D.Add(self.info_2D,(2,0),span=(1,4),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_CENTER_HORIZONTAL))
      #
      self.sizer.Add(panel_2D,(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      self.type_panel = panel_2D
      self.type_panels = [panel_2D]
      #
      # 3D panel
      #
      panel_3D = wx.Panel(self)
      sizer_3D = wx.GridBagSizer(10,10)
      panel_3D.SetSizer(sizer_3D)
      #
      sizer_3D.Add(wx.StaticText(panel_3D,label='scale factor'),(0,1),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.scale_3D = wx.TextCtrl(panel_3D,-1,'1.0')
      sizer_3D.Add(self.scale_3D,(0,2),flag=wx.ALIGN_LEFT)
      #
      sizer_3D.Add(wx.StaticText(panel_3D,label='curve points'),(1,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.points_3D = wx.TextCtrl(panel_3D,-1,'10')
      sizer_3D.Add(self.points_3D,(1,1),flag=wx.ALIGN_LEFT)
      self.resolution_3D = wx.TextCtrl(panel_3D,-1,'10000')
      sizer_3D.Add(self.resolution_3D,(1,2),flag=wx.ALIGN_RIGHT)
      sizer_3D.Add(wx.StaticText(panel_3D,label='path resolution'),(1,3),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT))
      #
      sizer_3D.Add(wx.StaticText(panel_3D,label='min intensity z (mm)'),(2,0),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_RIGHT))
      self.zmin = wx.TextCtrl(panel_3D,-1,'0')
      sizer_3D.Add(self.zmin,(2,1),flag=wx.ALIGN_LEFT)
      self.zmax = wx.TextCtrl(panel_3D,-1,'0')
      sizer_3D.Add(self.zmax,(2,2),flag=wx.ALIGN_RIGHT)
      sizer_3D.Add(wx.StaticText(panel_3D,label='max intensity z (mm)'),(2,3),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT))
      #
      self.info_3D = wx.StaticText(panel_3D,label="")
      sizer_3D.Add(self.info_3D,(3,0),span=(1,4),flag=(wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_CENTER_HORIZONTAL))
      #
      panel_3D.Hide()
      self.type_panels.append(panel_3D)
      #
      # fit
      #
      self.Fit()
   #
   # path type handler
   #
   def path_type_handler(self,event):
      self.parent.path_type = self.path_types[event.GetSelection()]
      self.parent.update_panels()
   #
   # parent call to update panel
   #
   def update_panel(self):
      index = self.path_types.index(self.parent.path_type)
      self.path_type.SetSelection(index)
      self.sizer.Remove(self.type_panel)
      self.type_panel.Hide()
      self.sizer.Add(self.type_panels[index],(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_TOP))
      self.type_panels[index].Show()
      self.type_panel = self.type_panels[index]
      self.Layout()
      self.Fit()
