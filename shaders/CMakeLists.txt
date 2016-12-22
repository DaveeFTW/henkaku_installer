project(installer_shaders)

set(VERTEX_SHADERS
	"cube.vert.cg"
	"clear.vert.cg"
	"animbg.vert.cg"
	"text.vert.cg"
	"colour.vert.cg"
)

set(FRAGMENT_SHADERS
	"cube.frag.cg"
	"clear.frag.cg"
	"animbg.frag.cg"
	"text.frag.cg"
	"colour.frag.cg"
)

function(CompileShaderResource shader output type)
	add_custom_command(
		OUTPUT ${output}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${shader}
		COMMAND runshacc --${type} ${CMAKE_CURRENT_SOURCE_DIR}/${shader} ${output}
	)
endfunction()

source_group(shaders FILES ${VERTEX_SHADERS} ${FRAGMENT_SHADERS})

set(SHADER_RESOURCES)

foreach (shader ${VERTEX_SHADERS})
	CompileShaderResource(${shader} ${shader}.gxp "vertex")
	set(SHADER_RESOURCES ${SHADER_RESOURCES} ${shader}.gxp)
endforeach(shader)

foreach (shader ${FRAGMENT_SHADERS})
	CompileShaderResource(${shader} ${shader}.gxp "fragment")
	set(SHADER_RESOURCES ${SHADER_RESOURCES} ${shader}.gxp)
endforeach(shader)

add_custom_target(shaders DEPENDS shaders.rsc)

add_custom_command(
	OUTPUT shaders.rsc
	DEPENDS ${SHADER_RESOURCES}
	COMMAND python ${CMAKE_SOURCE_DIR}/tools/resourcepack.py shaders.rsc ${SHADER_RESOURCES}
)
