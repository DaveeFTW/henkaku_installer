project(installer_assets)

set(FONTS
	"fonts/DroidSans.ttf"
)

set(TEXTURES
	"textures/checkbox-checked.png"
	"textures/checkbox-unchecked.png"
)

source_group(fonts FILES ${FONTS})

set(ASSET_RESOURCES)

foreach (font ${FONTS})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${font} ${font} COPYONLY)
	set(ASSET_RESOURCES ${ASSET_RESOURCES} ${font})
endforeach(font)

foreach (texture ${TEXTURES})
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${texture} ${texture} COPYONLY)
	set(ASSET_RESOURCES ${ASSET_RESOURCES} ${texture})
endforeach(texture)

add_custom_target(assets DEPENDS assets.rsc)

add_custom_command(
	OUTPUT assets.rsc
	DEPENDS ${ASSET_RESOURCES}
	COMMAND python ${CMAKE_SOURCE_DIR}/tools/resourcepack.py assets.rsc ${ASSET_RESOURCES}
)
