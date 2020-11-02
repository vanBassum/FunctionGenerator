# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.cpp" 
					"Display/ili9340.c" 
					"Display/decode_image.c" 
					"Display/pngle.c" 
					"Display/fontx.c")

set(COMPONENT_ADD_INCLUDEDIRS "" "Display")


                    
register_component()
