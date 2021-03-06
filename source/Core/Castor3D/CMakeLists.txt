project( Castor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RequiredVersion.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/Src/RequiredVersion.hpp
	NEWLINE_STYLE LF
)

set( CastorBinsDependencies
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )

add_target(
	${PROJECT_NAME}
	api_dll
	"CastorUtils;GlslWriter"
	"${CastorMinLibraries}"
	"Castor3DPch.hpp"
	"Castor3DPch.cpp"
)

file( GLOB ${PROJECT_NAME}_Prerequisites_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.inl
)
file( GLOB ${PROJECT_NAME}_Prerequisites_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.cpp
)
source_group( "Header Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_HEADER_FILES} )
source_group( "Source Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Cache_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.inl
)
file( GLOB ${PROJECT_NAME}_Cache_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.cpp
)
source_group( "Header Files\\Cache" FILES ${${PROJECT_NAME}_Cache_HEADER_FILES} )
source_group( "Source Files\\Cache" FILES ${${PROJECT_NAME}_Cache_SOURCE_FILES} )

parse_subdir_files( Animation "Animation" )
parse_subdir_files( Animation/Skeleton "Animation\\\\Skeleton" )
parse_subdir_files( Animation/Mesh "Animation\\\\Mesh" )
parse_subdir_files( Binary "Binary" )
parse_subdir_files( Cache "Cache" )
parse_subdir_files( EnvironmentMap "EnvironmentMap" )
parse_subdir_files( Event "Event" )
parse_subdir_files( Event/Frame "Event\\\\Frame" )
parse_subdir_files( Event/UserInput "Event\\\\UserInput" )
parse_subdir_files( FrameBuffer "FrameBuffer" )
parse_subdir_files( HDR "HDR" )
parse_subdir_files( Material "Material" )
parse_subdir_files( Mesh "Mesh" )
parse_subdir_files( Mesh/Generator "Mesh\\\\Generator" )
parse_subdir_files( Mesh/Buffer "Mesh\\\\Buffer" )
parse_subdir_files( Mesh/Skeleton "Mesh\\\\Skeleton" )
parse_subdir_files( Mesh/SubmeshComponent "Mesh\\\\SubmeshComponent" )
parse_subdir_files( Miscellaneous "Miscellaneous" )
parse_subdir_files( Overlay "Overlay" )
parse_subdir_files( PBR "PBR" )
parse_subdir_files( Plugin "Plugin" )
parse_subdir_files( Prerequisites "Prerequisites" )
parse_subdir_files( PostEffect "PostEffect" )
parse_subdir_files( RenderToTexture "RenderToTexture" )
parse_subdir_files( Render "Render" )
parse_subdir_files( Render/RenderNode "Render\\\\RenderNode" )
parse_subdir_files( Scene "Scene" )
parse_subdir_files( Scene/Animation "Scene\\\\Animation" )
parse_subdir_files( Scene/Animation/Skeleton "Scene\\\\Animation\\\\Skeleton" )
parse_subdir_files( Scene/Animation/Mesh "Scene\\\\Animation\\\\Mesh" )
parse_subdir_files( Scene/Light "Scene\\\\Light" )
parse_subdir_files( Scene/ParticleSystem "Scene\\\\ParticleSystem" )
parse_subdir_files( Shader "Shader" )
parse_subdir_files( Shader/PassBuffer "Shader\\\\PassBuffer" )
parse_subdir_files( Shader/Shaders "Shader\\\\Shaders" )
parse_subdir_files( Shader/Ubos "Shader\\\\Ubos" )
parse_subdir_files( Shader/Uniform "Shader\\\\Uniform" )
parse_subdir_files( ShadowMap "ShadowMap" )
parse_subdir_files( State "State" )
parse_subdir_files( Technique "Technique" )
parse_subdir_files( Technique/Opaque "Technique\\\\Opaque" )
parse_subdir_files( Technique/Opaque/Ssao "Technique\\\\Opaque\\\\Ssao" )
parse_subdir_files( Technique/Transparent "Technique\\\\Transparent" )
parse_subdir_files( Texture "Texture" )

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

# Copying core zip data files
file(
	GLOB
		CoreZipFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.zip
)
copy_target_files( ${PROJECT_NAME} "" ${CoreZipFiles} )

# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Languages/*.lang
)
copy_target_files( ${PROJECT_NAME} "" ${ShaderLanguagesFiles} )

# Preparing sample scene files installation
file(
	GLOB
		SampleSceneFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cscn
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cmtl
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cmsh
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.zip
)
install(
	FILES ${SampleSceneFiles}
	DESTINATION share/${PROJECT_NAME}/samples
	COMPONENT ${PROJECT_NAME}_Samples
)

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

