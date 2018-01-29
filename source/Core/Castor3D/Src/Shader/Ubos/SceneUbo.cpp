#include "SceneUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const SceneUbo::BufferScene = cuT( "Scene" );
	String const SceneUbo::AmbientLight = cuT( "c3d_ambientLight" );
	String const SceneUbo::BackgroundColour = cuT( "c3d_backgroundColour" );
	String const SceneUbo::CameraPos = cuT( "c3d_cameraPosition" );
	String const SceneUbo::CameraNearPlane = cuT( "c3d_cameraNearPlane" );
	String const SceneUbo::CameraFarPlane = cuT( "c3d_cameraFarPlane" );
	String const SceneUbo::FogType = cuT( "c3d_fogType" );
	String const SceneUbo::FogDensity = cuT( "c3d_fogDensity" );
	String const SceneUbo::LightsCount = cuT( "c3d_lightsCount" );

	SceneUbo::SceneUbo( Engine & engine )
		: m_engine{ engine }
	{
	}

	SceneUbo::~SceneUbo()
	{
	}

	void SceneUbo::initialise()
	{
		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		m_ubo = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
	}

	void SceneUbo::cleanup()
	{
		m_ubo.reset();
	}

	void SceneUbo::updateCameraPosition( Camera const & camera )const
	{
		auto & configuration = m_ubo->getData( 0u );
		configuration.cameraPos = camera.getParent()->getDerivedPosition();
		configuration.ambientLight = toRGBAFloat( camera.getScene()->getAmbientLight() );
		configuration.backgroundColour = toRGBAFloat( camera.getScene()->getBackgroundColour() );
		m_ubo->upload();
	}

	void SceneUbo::update( Camera const & camera
		, Fog const & fog )const
	{
		auto & configuration = m_ubo->getData( 0u );
		configuration.fogType = int( fog.getType() );
		configuration.fogDensity = fog.getDensity();
		configuration.cameraNearPlane = camera.getViewport().getNear();
		configuration.cameraFarPlane = camera.getViewport().getFar();
		updateCameraPosition( camera );
	}

	void SceneUbo::update( Scene const & scene
		, Camera const & camera
		, bool lights )const
	{
		auto & configuration = m_ubo->getData( 0u );

		if ( lights )
		{
			auto & cache = scene.getLightCache();
			auto lock = makeUniqueLock( cache );
			configuration.lightsCount[size_t( LightType::eSpot )] = cache.getLightsCount( LightType::eSpot );
			configuration.lightsCount[size_t( LightType::ePoint )] = cache.getLightsCount( LightType::ePoint );
			configuration.lightsCount[size_t( LightType::eDirectional )] = cache.getLightsCount( LightType::eDirectional );
		}

		update( camera, scene.getFog() );
	}
}
