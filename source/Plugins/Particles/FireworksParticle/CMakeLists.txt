project( FireworksParticle )

set( CastorBinsDependencies 
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( ${PROJECT_NAME}_DESCRIPTION "Fireworks Particle" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

add_definitions( ${ParticlesDefinitions} )
include_directories( ${ParticlesIncludes} )

add_target(
	${PROJECT_NAME}
	plugin
	"Castor3D"
	"${ParticlesLibraries}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Particles" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
