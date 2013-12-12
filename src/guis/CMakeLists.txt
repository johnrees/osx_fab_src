cmake_minimum_required(VERSION 2.6)

set(GUIs fab fab.html fabserver
         make_cad_png make_cad_eps make_cad_stl make_cad_camm make_cad_rml
         make_cad_epi make_cad_uni make_cad_sbp make_cad_g make_cad_ord
         make_cad_grb make_cad_drl
         make_math_camm make_math_epi make_math_g make_math_ord make_math_eps
         make_math_uni make_math_rml make_math_sbp make_math_grb
         make_math_drl
         make_png_png make_png_eps make_png_epi make_png_uni make_png_grb
         make_png_epi_halftone make_png_uni_halftone make_png_rml make_png_sbp
         make_png_ord make_png_camm make_png_plt make_png_g make_png_drl
         make_png_oms
         make_stl_png make_stl_rml make_stl_sbp make_stl_g
         make_svg_camm make_svg_epi make_svg_uni make_svg_oms
         make_svg_g make_svg_rml make_svg_sbp make_svg_ord make_png_snap
         make_cad_snap make_stl_snap make_svg_snap make_png_eps_halftone
         make_cad_dxf make_math_dxf make_math_stl make_png_dxf
         cad_ui
         rml_send_gui
    CACHE STRING "GUI script list")

if( ${CMAKE_PROJECT_NAME} MATCHES fabmod )
  install(PROGRAMS ${GUIs} DESTINATION ${PROJECT_SOURCE_DIR}/../bin)
endif( ${CMAKE_PROJECT_NAME} MATCHES fabmod )
