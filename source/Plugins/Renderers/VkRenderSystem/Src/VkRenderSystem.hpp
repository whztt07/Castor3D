/*
See LICENSE file in root folder
*/
#ifndef ___VK_RENDER_SYSTEM_H___
#define ___VK_RENDER_SYSTEM_H___

#include "VkRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

namespace VkRender
{
	class RenderSystem
		: public castor3d::RenderSystem
	{
	public:
		explicit RenderSystem( castor3d::Engine & engine );
		virtual ~RenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::RenderSystem::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::RenderSystem::doCleanup
		 */
		void doCleanup()override;

	public:
		C3D_Vk_API static castor::String Name;
		C3D_Vk_API static castor::String Type;
	};
}

#endif