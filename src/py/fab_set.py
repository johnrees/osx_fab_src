#
# fab_set.py
#    fab modules frame and set workflow defaults
#
# Neil Gershenfeld 9/1/13
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
import wx,sys,os
#
# frame class
#
class fab_frame(wx.Frame):
   #
   # set .png .epi defaults
   #
   def set_png_epi(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('25');\
         self.epi_panel.speed_2D.SetValue('75');\
         self.epi_panel.min_power_3D.SetValue('5');\
         self.epi_panel.max_power_3D.SetValue('25');\
         self.epi_panel.speed_3D.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('75');\
         self.epi_panel.speed_2D.SetValue('25');\
         self.epi_panel.min_power_3D.SetValue('25');\
         self.epi_panel.max_power_3D.SetValue('75');\
         self.epi_panel.speed_3D.SetValue('25');"
   #
   # set .png .epi halftone defaults
   #
   def set_png_epi_halftone(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.epi_panel.power_2D.SetValue('25');\
         self.epi_panel.speed_2D.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.epi_panel.power_2D.SetValue('75');\
         self.epi_panel.speed_2D.SetValue('25');"
   #
   # set .cad .epi defaults
   #
   def set_cad_epi(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.cad_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('25');\
         self.epi_panel.speed_2D.SetValue('75');\
         self.epi_panel.min_power_3D.SetValue('5');\
         self.epi_panel.max_power_3D.SetValue('25');\
         self.epi_panel.speed_3D.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.cad_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('75');\
         self.epi_panel.speed_2D.SetValue('25');\
         self.epi_panel.min_power_3D.SetValue('25');\
         self.epi_panel.max_power_3D.SetValue('75');\
         self.epi_panel.speed_3D.SetValue('25');"
   #
   # set .math .epi defaults
   #
   def set_math_epi(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('25');\
         self.epi_panel.speed_2D.SetValue('75');\
         self.epi_panel.min_power_3D.SetValue('5');\
         self.epi_panel.max_power_3D.SetValue('25');\
         self.epi_panel.speed_3D.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.diameter_finish.SetValue('0.25');\
         self.png_path_panel.clearance_diameter_finish.SetValue('0.25');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.error_2D.SetValue('1.5');\
         self.epi_panel.power_2D.SetValue('75');\
         self.epi_panel.speed_2D.SetValue('25');\
         self.epi_panel.min_power_3D.SetValue('25');\
         self.epi_panel.max_power_3D.SetValue('75');\
         self.epi_panel.speed_3D.SetValue('25');"
   #
   # set .svg .epi defaults
   #
   def set_svg_epi(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.epi_panel.power_2D.SetValue('25');\
         self.epi_panel.speed_2D.SetValue('75');\
         self.epi_panel.min_power_3D.SetValue('5');\
         self.epi_panel.max_power_3D.SetValue('25');\
         self.epi_panel.speed_3D.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.epi_panel.power_2D.SetValue('75');\
         self.epi_panel.speed_2D.SetValue('25');\
         self.epi_panel.min_power_3D.SetValue('25');\
         self.epi_panel.max_power_3D.SetValue('75');\
         self.epi_panel.speed_3D.SetValue('25');"
   #
   # set .png .uni defaults
   #
   def set_png_uni(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('25');\
         self.uni_panel.speed.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('75');\
         self.uni_panel.speed.SetValue('25');"
   #
   # set .png .uni halftone defaults
   #
   def set_png_uni_halftone(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.uni_panel.power.SetValue('25');\
         self.uni_panel.speed.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.uni_panel.power.SetValue('75');\
         self.uni_panel.speed.SetValue('25');"
   #
   # set .cad .uni defaults
   #
   def set_cad_uni(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('25');\
         self.uni_panel.speed.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('75');\
         self.uni_panel.speed.SetValue('25');"
   #
   # set .math .uni defaults
   #
   def set_math_uni(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('25');\
         self.uni_panel.speed.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.uni_panel.power.SetValue('75');\
         self.uni_panel.speed.SetValue('25');"
   #
   # set .svg .uni defaults
   #
   def set_svg_uni(self):
      self.defaults = {}
      self.control_panel.defaults.Append('cardboard')
      self.defaults["cardboard"]\
      = "self.uni_panel.power.SetValue('25');\
         self.uni_panel.speed.SetValue('75');"
      self.control_panel.defaults.Append('acrylic')
      self.defaults["acrylic"]\
      = "self.uni_panel.power.SetValue('75');\
         self.uni_panel.speed.SetValue('25');"
   #
   # set .png .rml defaults
   #
   def set_png_rml(self):
      self.defaults = {}
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_path_panel.diameter_plane.SetValue('0.4');\
         self.png_path_panel.number_plane.SetValue('4');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('mill traces (0.010)')
      self.defaults["mill traces (0.010)"]\
      = "self.png_path_panel.diameter_plane.SetValue('0.254');\
         self.png_path_panel.number_plane.SetValue('1');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('2');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_path_panel.diameter_rough.SetValue('0.79');\
         self.png_path_panel.number_rough.SetValue('1');\
         self.png_path_panel.itop_rough.SetValue('0.5');\
         self.png_path_panel.ibot_rough.SetValue('0.5');\
         self.png_path_panel.ztop_rough.SetValue('-0.6');\
         self.png_path_panel.zbot_rough.SetValue('-1.7');\
         self.png_path_panel.zstep_rough.SetValue('0.6');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('-10');\
         self.png_path_panel.ztop_rough.SetValue('0');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('-10');\
         self.png_path_panel.ztop_finish.SetValue('0');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.path_type = '3D finish';\
         self.update_panels();"
   #
   # set .cad .rml defaults
   #
   def set_cad_rml(self):
      self.defaults = {}
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_plane.SetValue('0.4');\
         self.png_path_panel.number_plane.SetValue('4');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('mill traces (0.010)')
      self.defaults["mill traces (0.010)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_plane.SetValue('0.254');\
         self.png_path_panel.number_plane.SetValue('1');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('2');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_rough.SetValue('0.79');\
         self.png_path_panel.number_rough.SetValue('1');\
         self.png_path_panel.itop_rough.SetValue('0.5');\
         self.png_path_panel.ibot_rough.SetValue('0.5');\
         self.png_path_panel.ztop_rough.SetValue('-0.6');\
         self.png_path_panel.zbot_rough.SetValue('-1.7');\
         self.png_path_panel.zstep_rough.SetValue('0.6');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.path_type = '3D finish';\
         self.update_panels();"
   #
   # set .math .rml defaults
   #
   def set_math_rml(self):
      self.defaults = {}
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter.SetValue('0.4');\
         self.png_path_panel.number.SetValue('4');\
         self.png_path_panel.z.SetValue('-0.1');\
         self.png_path_panel.frame3_ztop.SetValue('');\
         self.png_path_panel.frame3_zbot.SetValue('');\
         self.png_path_panel.frame3_zstep.SetValue('');\
         self.png_path_panel.frame3_xy.SetValue(False);\
         self.png_path_panel.frame3_xz.SetValue(False);\
         self.png_path_panel.frame3_yz.SetValue(False);\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');"
      self.control_panel.defaults.Append('mill traces (0.010)')
      self.defaults["mill traces (0.010)"]\
      = "self.png_path_panel.diameter.SetValue('0.254');\
         self.png_path_panel.number.SetValue('1');\
         self.png_path_panel.z.SetValue('-0.1');\
         self.png_path_panel.frame3_ztop.SetValue('');\
         self.png_path_panel.frame3_zbot.SetValue('');\
         self.png_path_panel.frame3_zstep.SetValue('');\
         self.png_path_panel.frame3_xy.SetValue(False);\
         self.png_path_panel.frame3_xz.SetValue(False);\
         self.png_path_panel.frame3_yz.SetValue(False);\
         self.rml_panel.speed.SetValue('2');\
         self.rml_panel.zjog.SetValue('1.0');"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter.SetValue('0.79');\
         self.png_path_panel.number.SetValue('1');\
         self.png_path_panel.frame3_itop.SetValue('0.5');\
         self.png_path_panel.frame3_ibot.SetValue('0.5');\
         self.png_path_panel.frame3_ztop.SetValue('-0.6');\
         self.png_path_panel.frame3_zbot.SetValue('-1.7');\
         self.png_path_panel.frame3_zstep.SetValue('0.6');\
         self.png_path_panel.frame3_xy.SetValue(True);\
         self.png_path_panel.frame3_xz.SetValue(False);\
         self.png_path_panel.frame3_yz.SetValue(False);\
         self.rml_panel.speed.SetValue('4');\
         self.rml_panel.zjog.SetValue('1.0');"
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter.SetValue('3.175');\
         self.png_path_panel.number.SetValue('-1');\
         self.png_path_panel.overlap.SetValue('0.25');\
         self.png_path_panel.error.SetValue('1.5');\
         self.png_path_panel.frame3_xy.SetValue(True);\
         self.png_path_panel.frame3_xz.SetValue(False);\
         self.png_path_panel.frame3_yz.SetValue(False);\
         self.png_path_panel.frame3_zstep.SetValue('0.5');\
         self.rml_panel.speed.SetValue('20');"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter.SetValue('3.175');\
         self.png_path_panel.number.SetValue('1');\
         self.png_path_panel.overlap.SetValue('0.5');\
         self.png_path_panel.error.SetValue('1.5');\
         self.png_path_panel.frame3_xy.SetValue(False);\
         self.png_path_panel.frame3_xz.SetValue(True);\
         self.png_path_panel.frame3_yz.SetValue(True);\
         self.rml_panel.speed.SetValue('20');"
   #
   # set .stl .rml defaults
   #
   def set_stl_rml(self):
      self.defaults = {}
      self.control_panel.defaults.Append('inches, 1/8, wax, rough')
      self.defaults["inches, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('inches, 1/8, wax, finish')
      self.defaults["inches, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, rough')
      self.defaults["mm, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, finish')
      self.defaults["mm, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.rml_panel.button.Hide();\
         self.rml_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .svg .rml defaults
   #
   def set_svg_rml(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax')
      self.defaults["wax"]\
      = "self.rml_panel.speed.SetValue('20');"
   #
   # set .png .sbp defaults
   #
   def set_png_sbp(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('-10');\
         self.png_path_panel.ztop_rough.SetValue('0');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('-10');\
         self.png_path_panel.ztop_finish.SetValue('0');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.path_type = '3D finish';\
         self.update_panels();"
   #
   # set .png .plt defaults
   #  
   def set_png_plt(self):
      self.defaults = {}
   #
   # set .cad .sbp defaults
   #
   def set_cad_sbp(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .math .sbp defaults
   #
   def set_math_sbp(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.math_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.math_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .stl .sbp defaults
   #
   def set_stl_sbp(self):
      self.defaults = {}
      self.control_panel.defaults.Append('inches, 1/8, wax, rough')
      self.defaults["inches, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('inches, 1/8, wax, finish')
      self.defaults["inches, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, rough')
      self.defaults["mm, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, finish')
      self.defaults["mm, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.sbp_panel.button.Hide();\
         self.sbp_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .svg .sbp defaults
   #
   def set_svg_sbp(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax')
      self.defaults["wax"]\
      = "self.sbp_panel.speed.SetValue('20');\
         self.sbp_panel.button.Hide();\
         self.svg_path_panel.zmin.SetValue('-10.0');\
         self.svg_path_panel.zmax.SetValue('0.0');\
         self.path_type='3D';\
         self.update_panels();"
   #
   # set .png .g defaults
   #
   def set_png_g(self):
      self.defaults = {}
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_path_panel.diameter_plane.SetValue('0.4');\
         self.png_path_panel.number_plane.SetValue('4');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_path_panel.diameter_rough.SetValue('0.79');\
         self.png_path_panel.number_rough.SetValue('1');\
         self.png_path_panel.itop_rough.SetValue('0.5');\
         self.png_path_panel.ibot_rough.SetValue('0.5');\
         self.png_path_panel.ztop_rough.SetValue('-0.6');\
         self.png_path_panel.zbot_rough.SetValue('-1.7');\
         self.png_path_panel.zstep_rough.SetValue('0.6');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('-10');\
         self.png_path_panel.ztop_rough.SetValue('0');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('-10');\
         self.png_path_panel.ztop_finish.SetValue('0');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.path_type = '3D finish';\
         self.update_panels();"
   #
   # set .cad .g defaults
   #
   def set_cad_g(self):
      self.defaults = {}
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_plane.SetValue('0.4');\
         self.png_path_panel.number_plane.SetValue('4');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('mill traces (0.010)')
      self.defaults["mill traces (0.010)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_plane.SetValue('0.254');\
         self.png_path_panel.number_plane.SetValue('1');\
         self.png_path_panel.z_plane.SetValue('-0.1');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D plane';\
         self.update_panels();"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.diameter_rough.SetValue('0.79');\
         self.png_path_panel.number_rough.SetValue('1');\
         self.png_path_panel.itop_rough.SetValue('0.5');\
         self.png_path_panel.ibot_rough.SetValue('0.5');\
         self.png_path_panel.ztop_rough.SetValue('-0.6');\
         self.png_path_panel.zbot_rough.SetValue('-1.7');\
         self.png_path_panel.zstep_rough.SetValue('0.6');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.cad_png_panel.resolution.SetValue('25');\
         self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.path_type = '3D finish';\
         self.update_panels();"
   #
   # set .math .g defaults
   #
   def set_math_g(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax rough cut (1/8)')
      self.defaults["wax rough cut (1/8)"]\
      = "self.math_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.units = 25.4;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('wax finish cut (1/8)')
      self.defaults["wax finish cut (1/8)"]\
      = "self.math_png_panel.resolution.SetValue('10');\
         self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('4');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .stl .g defaults
   #
   def set_stl_g(self):
      self.defaults = {}
      self.control_panel.defaults.Append('inches, 1/8, wax, rough')
      self.defaults["inches, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type = '3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('inches, 1/8, wax, finish')
      self.defaults["inches, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.units = 25.4;\
         self.path_type='3D finish';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, rough')
      self.defaults["mm, 1/8, wax, rough"]\
      = "self.png_path_panel.diameter_rough.SetValue('3.175');\
         self.png_path_panel.number_rough.SetValue('-1');\
         self.png_path_panel.overlap_rough.SetValue('0.25');\
         self.png_path_panel.error_rough.SetValue('1.5');\
         self.png_path_panel.zstep_rough.SetValue('1');\
         self.png_path_panel.ibot_rough.SetValue('0');\
         self.png_path_panel.itop_rough.SetValue('1');\
         self.png_path_panel.zbot_rough.SetValue('');\
         self.png_path_panel.ztop_rough.SetValue('');\
         self.stl_png_panel.units.SetValue('1');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D rough';\
         self.update_panels();"
      self.control_panel.defaults.Append('mm, 1/8, wax, finish')
      self.defaults["mm, 1/8, wax, finish"]\
      = "self.png_path_panel.diameter_finish.SetValue('3.175');\
         self.png_path_panel.clearance_diameter_finish.SetValue('3.175');\
         self.png_path_panel.overlap_finish.SetValue('0.5');\
         self.png_path_panel.error_finish.SetValue('1.5');\
         self.png_path_panel.xz_finish.SetValue(True);\
         self.png_path_panel.yz_finish.SetValue(True);\
         self.png_path_panel.ibot_finish.SetValue('0');\
         self.png_path_panel.itop_finish.SetValue('1');\
         self.png_path_panel.zbot_finish.SetValue('');\
         self.png_path_panel.ztop_finish.SetValue('');\
         self.stl_png_panel.units.SetValue('1');\
         self.stl_png_panel.resolution.SetValue('25');\
         self.g_panel.button.Hide();\
         self.g_panel.speed.SetValue('20');\
         self.units = 1;\
         self.path_type='3D finish';\
         self.update_panels();"
   #
   # set .svg .g defaults
   #
   def set_svg_g(self):
      self.defaults = {}
      self.control_panel.defaults.Append('wax')
      self.defaults["wax"]\
      = "self.g_panel.speed.SetValue('20');"
   #
   # set .png .eps defaults
   #
   def set_png_eps(self):
      self.defaults = {}
      self.control_panel.defaults.Append('outline')
      self.defaults["outline"]\
      = "self.png_path_panel.error_plane.SetValue('1');\
         self.png_path_panel.diameter_plane.SetValue('0');\
         self.png_path_panel.set_path_type('3D plane');\
         self.png_path_panel.number_plane.SetValue('1');"
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_path_panel.error_plane.SetValue('1');\
         self.png_path_panel.diameter_plane.SetValue('0.4');\
         self.png_path_panel.set_path_type('3D plane');\
         self.png_path_panel.number_plane.SetValue('4');"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_path_panel.error_plane.SetValue('1');\
         self.png_path_panel.diameter_plane.SetValue('0.8');\
         self.png_path_panel.set_path_type('3D plane');\
         self.png_path_panel.number_plane.SetValue('1');"
   #
   # set .cad .eps defaults
   #
   def set_cad_eps(self):
      self.defaults = {}
      self.control_panel.defaults.Append('outline')
      self.defaults["outline"]\
      = "self.cad_png_panel.resolution.SetValue('50');\
         self.png_path_panel.error_plane.SetValue('1');\
         self.png_path_panel.diameter_plane.SetValue('0');\
         self.png_path_panel.number_plane.SetValue('1');\
         self.path_type = '3D plane';\
         self.update_panels();"
   #
   # set .math .eps defaults
   #
   def set_math_eps(self):
      self.defaults = {}
      self.control_panel.defaults.Append('outline')
      self.defaults["outline"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0');\
         self.png_path_panel.number.SetValue('1');"
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0.4');\
         self.png_path_panel.number.SetValue('4');"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0.8');\
         self.png_path_panel.number.SetValue('1');"
   #
   # set .svg .eps defaults
   #
   def set_svg_eps(self):
      self.defaults = {}
      self.control_panel.defaults.Append('outline')
      self.defaults["outline"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0');\
         self.png_path_panel.number.SetValue('1');"
      self.control_panel.defaults.Append('mill traces (1/64)')
      self.defaults["mill traces (1/64)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0.4');\
         self.png_path_panel.number.SetValue('4');"
      self.control_panel.defaults.Append('cut out board (1/32)')
      self.defaults["cut out board (1/32)"]\
      = "self.png_panel.resolution.SetValue('50');\
         self.png_path_panel.error.SetValue('1');\
         self.png_path_panel.diameter.SetValue('0.8');\
         self.png_path_panel.number.SetValue('1');"
   #
   # set .png .camm defaults
   #
   def set_png_camm(self):
      self.defaults = {}
      self.control_panel.defaults.Append('vinyl')
      self.defaults["vinyl"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.camm_panel.force.SetValue('45');\
         self.camm_panel.velocity.SetValue('5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('copper')
      self.defaults["copper"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.camm_panel.force.SetValue('55');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('epoxy')
      self.defaults["epoxy"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('0.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.camm_panel.force.SetValue('90');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
   #
   # set .cad .camm defaults
   #
   def set_cad_camm(self):
      self.defaults = {}
      self.control_panel.defaults.Append('vinyl')
      self.defaults["vinyl"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('45');\
         self.camm_panel.velocity.SetValue('5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('copper')
      self.defaults["copper"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('55');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('epoxy')
      self.defaults["epoxy"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('90');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
   #
   # set .math .camm defaults
   #
   def set_math_camm(self):
      self.defaults = {}
      self.control_panel.defaults.Append('vinyl')
      self.defaults["vinyl"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('45');\
         self.camm_panel.velocity.SetValue('5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('copper')
      self.defaults["copper"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('55');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('epoxy')
      self.defaults["epoxy"]\
      = "self.png_path_panel.error_2D.SetValue('1.5');\
         self.png_path_panel.diameter_2D.SetValue('.25');\
         self.png_path_panel.number_2D.SetValue('1');\
         self.png_panel.resolution.SetValue('10');\
         self.camm_panel.force.SetValue('90');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
   #
   # set .svg .camm defaults
   #
   def set_svg_camm(self):
      self.defaults = {}
      self.control_panel.defaults.Append('vinyl')
      self.defaults["vinyl"]\
      = "self.camm_panel.force.SetValue('45');\
         self.camm_panel.velocity.SetValue('5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('copper')
      self.defaults["copper"]\
      = "self.camm_panel.force.SetValue('55');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
      self.control_panel.defaults.Append('epoxy')
      self.defaults["epoxy"]\
      = "self.camm_panel.force.SetValue('90');\
         self.camm_panel.velocity.SetValue('2.5');\
         self.path_type = '2D';\
         self.update_panels();"
   #
   # set .png .ord defaults
   #
   def set_png_ord(self):
      self.defaults = {}
   #
   # set .cad .ord defaults
   #
   def set_cad_ord(self):
      self.defaults = {}
   #
   # set .math .ord defaults
   #
   def set_math_ord(self):
      self.defaults = {}
   #
   # set .svg .ord defaults
   #
   def set_svg_ord(self):
      self.defaults = {}
   #
   # set .png .oms defaults
   #
   def set_png_oms(self):
      self.defaults = {}
   #
   # set .svg .oms defaults
   #
   def set_svg_oms(self):
      self.defaults = {}
   #
   # init
   #
   def __init__(self,title,argv):
      self.tmp = "fab_mod_" # default temporary file prefix
      #
      # set default panel size from screen size
      #
      (screen_x,screen_y) = wx.DisplaySize()
      self.size = screen_y/2.1
      #
      # frame
      #
      wx.Frame.__init__(self,None,title=title)
      self.sizer = wx.GridBagSizer(10,10)
      self.SetSizer(self.sizer)
      #
      # resize event
      #
      self.Bind(wx.EVT_SIZE,self.resize_handler)
      #
      # arguments
      #
      self.rootname = ""
      self.filename = ""
      self.basename = ""
      if (len(argv) > 1):
         if (argv[1] != '""'):
            self.filename = sys.argv[1]
            self.basename = os.path.basename(self.filename)
      if (len(argv) > 2):
         self.size = int(sys.argv[2])
   #
   # defaults handler
   #
   def defaults_handler(self,event):
      value = self.control_panel.defaults.GetValue()
      string = self.defaults[value]
      exec(string)
   #
   # resize handler
   #
   def resize_handler(self,event):
      (sizex,sizey) = self.GetSize()
      self.update_sizes(sizex,sizey)
   #
   # update children size
   #
   def update_sizes(self,sizex,sizey):
      for child in self.GetChildren():
         if hasattr(child,"update_size"):
            child.update_size(sizex,sizey)
      self.Layout()
      self.Fit()
   #
   # update child panels
   #
   def update_panels(self):
      for child in self.GetChildren():
         if hasattr(child,"update_panel"):
            child.update_panel()
      self.Layout()
      self.Fit()
