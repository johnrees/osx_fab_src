#
# panel_png_path.py
#    make .path from .png
#
# Neil Gershenfeld 9/3/13
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
import wx,os,string
from panel_path import path_panel
#
# panel class
#
class png_path_panel(wx.Panel):
   #
   # constructor
   #
   def __init__(self,parent):
      self.parent = parent
      self.parent.png_file = ''
      self.parent.zmin = ''
      self.parent.zmax = ''
      self.parent.units = 1
      #
      # make path
      #
      def make_path(event):
         if (self.parent.png_file == ''):
            print "png_path: oops -- must make .png first"
            return
         self.parent.path_file = self.parent.tmp+self.parent.rootname+'.path'
         path_png = self.parent.tmp+self.parent.rootname+'.path.png'
         selection = self.path_type.GetValue()
         if (selection == "2D"):
            diameter = self.diameter_2D.GetValue()
            number = self.number_2D.GetValue()
            overlap = self.overlap_2D.GetValue()
            error = self.error_2D.GetValue()
            intensity = self.intensity_2D.GetValue()
            command = 'png_path '+'\"'+self.parent.png_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+error+' '+diameter+' '+number+' '+overlap+' '+intensity
         elif (selection == "3D plane"):
            diameter = self.diameter_plane.GetValue()
            number = self.number_plane.GetValue()
            overlap = self.overlap_plane.GetValue()
            error = self.error_plane.GetValue()
            intensity = self.intensity_plane.GetValue()
            z = self.z_plane.GetValue()
            command = 'png_path '+'\"'+self.parent.png_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+error+' '+diameter+' '+number+' '+overlap+' '+intensity+' '+intensity+' '+z
         elif (selection == "3D rough"):
            diameter = self.diameter_rough.GetValue()
            number = self.number_rough.GetValue()
            overlap = self.overlap_rough.GetValue()
            error = self.error_rough.GetValue()
            itop = self.itop_rough.GetValue()
            ibot = self.ibot_rough.GetValue()
            ztop = self.ztop_rough.GetValue()
            zbot = self.zbot_rough.GetValue()
            zstep = self.zstep_rough.GetValue()
            command = 'png_path '+'\"'+self.parent.png_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+error+' '+diameter+' '+number+' '+overlap+' '+itop+' '+ibot+' '+ztop+' '+zbot+' '+zstep
         elif (selection == "3D finish"):
            diameter = self.diameter_finish.GetValue()
            overlap = self.overlap_finish.GetValue()
            error = self.error_finish.GetValue()
            itop = self.itop_finish.GetValue()
            ibot = self.ibot_finish.GetValue()
            ztop = self.ztop_finish.GetValue()
            zbot = self.zbot_finish.GetValue()
            clearance_length = self.clearance_length_finish.GetValue()
            clearance_diameter = self.clearance_diameter_finish.GetValue()
            number = '0'
            zstep = '0'
            if self.xz_finish.GetValue():
               xz = "1"
            else:
               xz = "0"
            if self.yz_finish.GetValue():
               yz = "1"
            else:
               yz = "0"
            xy = "0"
            if self.flat_end_finish.GetValue():
               tool_type = 'f'
            elif self.ball_end_finish.GetValue():
               tool_type = 'b'
            command = 'png_path '+'\"'+self.parent.png_file+'\"'+' '+'\"'+self.parent.path_file+'\"'+' '+error+' '+diameter+' '+number+' '+overlap+' '+itop+' '+ibot+' '+ztop+' '+zbot+' '+zstep+' '+xz+' '+yz+' '+xy+' '+tool_type+' '+clearance_length+' '+clearance_diameter
         print command
         ret = os.system(command)
         if (ret == 0):
            self.path_viewer.draw(self.parent.path_file)
            #self.parent.Layout()
            #self.parent.Fit()
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
      self.path_types = ["2D","3D plane","3D rough","3D finish"]
      self.path_type = wx.ComboBox(make_panel,size=(100,-1),value="2D",choices=self.path_types,style=wx.CB_READONLY)
      self.parent.path_type = "2D"
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
      sizer_2D.Add(wx.StaticText(panel_2D,label='diameter (mm)'),(0,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.diameter_2D = wx.TextCtrl(panel_2D,-1,'0.25')
      sizer_2D.Add(self.diameter_2D,(0,1),flag=(wx.ALIGN_LEFT))
      self.number_2D = wx.TextCtrl(panel_2D,-1,'1')
      sizer_2D.Add(self.number_2D,(0,2),flag=(wx.ALIGN_RIGHT))
      sizer_2D.Add(wx.StaticText(panel_2D,label='offsets (-1 to fill)'),(0,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_2D.Add(wx.StaticText(panel_2D,label='overlap (0-1)'),(1,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.overlap_2D = wx.TextCtrl(panel_2D,-1,'0.5')
      sizer_2D.Add(self.overlap_2D,(1,1),flag=(wx.ALIGN_RIGHT))
      self.error_2D = wx.TextCtrl(panel_2D,-1,'1.1')
      sizer_2D.Add(self.error_2D,(1,2),flag=(wx.ALIGN_RIGHT))
      sizer_2D.Add(wx.StaticText(panel_2D,label='error (pixels)'),(1,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_2D.Add(wx.StaticText(panel_2D,label='intensity (0-1)'),(2,1),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.intensity_2D = wx.TextCtrl(panel_2D,-1,'0.5')
      sizer_2D.Add(self.intensity_2D,(2,2),flag=(wx.ALIGN_RIGHT))
      #
      self.sizer.Add(panel_2D,(3,0),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      self.type_panel = panel_2D
      self.type_panels = [panel_2D]
      #
      # 3D plane panel
      #
      panel_3D_plane = wx.Panel(self)
      sizer_3D_plane = wx.GridBagSizer(10,10)
      panel_3D_plane.SetSizer(sizer_3D_plane)
      #
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='  diameter (mm)'),(0,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.diameter_plane = wx.TextCtrl(panel_3D_plane,-1,'0.25')
      sizer_3D_plane.Add(self.diameter_plane,(0,1),flag=(wx.ALIGN_LEFT))
      self.number_plane = wx.TextCtrl(panel_3D_plane,-1,'1')
      sizer_3D_plane.Add(self.number_plane,(0,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='offsets (-1 to fill)'),(0,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='overlap (0-1)'),(1,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.overlap_plane = wx.TextCtrl(panel_3D_plane,-1,'0.5')
      sizer_3D_plane.Add(self.overlap_plane,(1,1),flag=(wx.ALIGN_LEFT))
      self.error_plane = wx.TextCtrl(panel_3D_plane,-1,'1.1')
      sizer_3D_plane.Add(self.error_plane,(1,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='error (pixels)'),(1,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='intensity (0-1) '),(2,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.intensity_plane = wx.TextCtrl(panel_3D_plane,-1,'0.5')
      sizer_3D_plane.Add(self.intensity_plane,(2,1),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      self.z_plane = wx.TextCtrl(panel_3D_plane,-1,'0')
      sizer_3D_plane.Add(self.z_plane,(2,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_plane.Add(wx.StaticText(panel_3D_plane,label='z (mm)'),(2,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      panel_3D_plane.Hide()
      self.type_panels.append(panel_3D_plane)
      #
      # 3D rough panel
      #
      panel_3D_rough = wx.Panel(self)
      sizer_3D_rough = wx.GridBagSizer(10,10)
      panel_3D_rough.SetSizer(sizer_3D_rough)
      #
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='diameter (mm)'),(0,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.diameter_rough = wx.TextCtrl(panel_3D_rough,-1,'0.25')
      sizer_3D_rough.Add(self.diameter_rough,(0,1),flag=(wx.ALIGN_LEFT))
      self.number_rough = wx.TextCtrl(panel_3D_rough,-1,'-1')
      sizer_3D_rough.Add(self.number_rough,(0,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='offsets (-1 to fill)'),(0,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='overlap (0-1)'),(1,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.overlap_rough = wx.TextCtrl(panel_3D_rough,-1,'0.5')
      sizer_3D_rough.Add(self.overlap_rough,(1,1),flag=(wx.ALIGN_LEFT))
      self.error_rough = wx.TextCtrl(panel_3D_rough,-1,'1.1')
      sizer_3D_rough.Add(self.error_rough,(1,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='error (pixels)'),(1,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='top intensity (0-1)'),(2,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.itop_rough = wx.TextCtrl(panel_3D_rough,-1,'1')
      sizer_3D_rough.Add(self.itop_rough,(2,1),flag=(wx.ALIGN_LEFT))
      self.ztop_rough = wx.TextCtrl(panel_3D_rough,-1,'0')
      sizer_3D_rough.Add(self.ztop_rough,(2,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='top z (mm)'),(2,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='bot intensity (0-1)'),(3,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.ibot_rough = wx.TextCtrl(panel_3D_rough,-1,'0')
      sizer_3D_rough.Add(self.ibot_rough,(3,1),flag=(wx.ALIGN_LEFT))
      self.zbot_rough = wx.TextCtrl(panel_3D_rough,-1,'')
      sizer_3D_rough.Add(self.zbot_rough,(3,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='bot z (mm)'),(3,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_rough.Add(wx.StaticText(panel_3D_rough,label='cut depth (mm)'),(4,1),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      self.zstep_rough = wx.TextCtrl(panel_3D_rough,-1,'1')
      sizer_3D_rough.Add(self.zstep_rough,(4,2),flag=(wx.ALIGN_RIGHT))
      #
      panel_3D_rough.Hide()
      self.type_panels.append(panel_3D_rough)
      #
      # 3D finish panel
      #
      panel_3D_finish = wx.Panel(self)
      sizer_3D_finish = wx.GridBagSizer(10,10)
      panel_3D_finish.SetSizer(sizer_3D_finish)
      #
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='tool diameter (mm)'),(0,1),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.diameter_finish = wx.TextCtrl(panel_3D_finish,-1,'0.25')
      sizer_3D_finish.Add(self.diameter_finish,(0,2),flag=(wx.ALIGN_LEFT))
      #
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='overlap (0-1)'),(1,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.overlap_finish = wx.TextCtrl(panel_3D_finish,-1,'0.5')
      sizer_3D_finish.Add(self.overlap_finish,(1,1),flag=(wx.ALIGN_LEFT))
      self.error_finish = wx.TextCtrl(panel_3D_finish,-1,'1.1')
      sizer_3D_finish.Add(self.error_finish,(1,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='error (pixels)'),(1,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='top intensity (0-1)'),(2,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.itop_finish = wx.TextCtrl(panel_3D_finish,-1,'1')
      sizer_3D_finish.Add(self.itop_finish,(2,1),flag=(wx.ALIGN_LEFT))
      self.ztop_finish = wx.TextCtrl(panel_3D_finish,-1,'0')
      sizer_3D_finish.Add(self.ztop_finish,(2,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='top z (mm)'),(2,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='bot intensity (0-1)'),(3,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.ibot_finish = wx.TextCtrl(panel_3D_finish,-1,'0')
      sizer_3D_finish.Add(self.ibot_finish,(3,1),flag=(wx.ALIGN_LEFT))
      self.zbot_finish = wx.TextCtrl(panel_3D_finish,-1,'')
      sizer_3D_finish.Add(self.zbot_finish,(3,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='bot z (mm)'),(3,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='clearance length (mm)'),(4,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.clearance_length_finish = wx.TextCtrl(panel_3D_finish,-1,'0')
      sizer_3D_finish.Add(self.clearance_length_finish,(4,1),flag=(wx.ALIGN_LEFT))
      self.clearance_diameter_finish = wx.TextCtrl(panel_3D_finish,-1,'0')
      sizer_3D_finish.Add(self.clearance_diameter_finish,(4,2),flag=(wx.ALIGN_RIGHT))
      sizer_3D_finish.Add(wx.StaticText(panel_3D_finish,label='clearance diameter (mm)'),(4,3),flag=(wx.ALIGN_LEFT|wx.ALIGN_CENTER_VERTICAL))
      #
      panel_3D_finish_4 = wx.Panel(panel_3D_finish)
      sizer_3D_finish_4 = wx.GridBagSizer(10,10)
      panel_3D_finish_4.SetSizer(sizer_3D_finish_4)
      sizer_3D_finish_4.Add(wx.StaticText(panel_3D_finish_4,label='direction:'),(0,0),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.xz_finish = wx.CheckBox(panel_3D_finish_4,-1,'xz',(10,10))
      self.xz_finish.SetValue(True)
      sizer_3D_finish_4.Add(self.xz_finish,(0,1),flag=wx.ALIGN_CENTER_HORIZONTAL)
      self.yz_finish = wx.CheckBox(panel_3D_finish_4,-1,'yz',(10,10))
      self.yz_finish.SetValue(True)
      sizer_3D_finish_4.Add(self.yz_finish,(0,2),flag=wx.ALIGN_CENTER_HORIZONTAL)
      sizer_3D_finish_4.Add(wx.StaticText(panel_3D_finish_4,label='type:'),(0,3),flag=(wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL))
      self.flat_end_finish = wx.RadioButton(panel_3D_finish_4,-1,'flat end',(10,10),style=wx.RB_GROUP)
      sizer_3D_finish_4.Add(self.flat_end_finish,(0,4),flag=wx.ALIGN_CENTER_HORIZONTAL)
      self.ball_end_finish = wx.RadioButton(panel_3D_finish_4,-1,'ball end',(10,10))
      #self.ball_end_finish.Enable(False)
      sizer_3D_finish_4.Add(self.ball_end_finish,(0,5),flag=wx.ALIGN_CENTER_HORIZONTAL)
      sizer_3D_finish.Add(panel_3D_finish_4,(5,0),span=(1,4),flag=(wx.ALIGN_CENTER_HORIZONTAL))
      #
      panel_3D_finish.Hide()
      self.type_panels.append(panel_3D_finish)
      #
      # fit
      #
      self.Fit()
      #
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
      #
      # check for 3D info in PNG
      #
      if (self.parent.png_file != ''):
         temp_name = self.parent.tmp+'png_info'
         command = 'png_size '+'\"'+self.parent.png_file+'\"'+' > '+'\"'+temp_name+'\"'
         os.system(command)
         output_file = open(temp_name,'r')
         info = output_file.read()
         command = 'rm '+'\"'+temp_name+'\"'
         os.system(command)
         start = 3+string.find(info,'dz:')
         if (start > 2):
            #
            # 3D info found, update
            #
            end = string.find(info,'mm',start)-1
            self.parent.zmin = -float(info[start:end])/self.parent.units
            self.parent.zmax = 0
            if (self.parent.path_type == "3D rough"):
               self.ztop_rough.SetValue(str(self.parent.units*self.parent.zmax))
               self.zbot_rough.SetValue(str(self.parent.units*self.parent.zmin))
            elif (self.parent.path_type == "3D finish"):
               self.ztop_finish.SetValue(str(self.parent.units*self.parent.zmax))
               self.zbot_finish.SetValue(str(self.parent.units*self.parent.zmin))
      self.Layout()
      self.Fit()
   #
   # parent call to update size
   #
   def update_size(self,sizex,sizey):
      self.Layout()
      self.Fit()
