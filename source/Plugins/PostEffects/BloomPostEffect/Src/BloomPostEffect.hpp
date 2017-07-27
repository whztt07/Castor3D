/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_BloomPostEffect___
#define ___C3D_BloomPostEffect___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Miscellaneous/GaussianBlur.hpp>
#include <PostEffect/PostEffect.hpp>
#include <Texture/TextureUnit.hpp>
#include <Render/Viewport.hpp>
#include <Shader/MatrixUbo.hpp>

namespace Bloom
{
	static const uint32_t FILTER_COUNT = 4;

	class BloomPostEffect
		: public Castor3D::PostEffect
	{
		using SurfaceArray = std::array< PostEffectSurface, FILTER_COUNT >;

	public:
		BloomPostEffect( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & renderSystem, Castor3D::Parameters const & p_param );
		virtual ~BloomPostEffect();
		static Castor3D::PostEffectSPtr Create( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem & renderSystem, Castor3D::Parameters const & p_param );
		/**
		 *\copydoc		Castor3D::PostEffect::Initialise
		 */
		virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::PostEffect::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::PostEffect::Apply
		 */
		virtual bool Apply( Castor3D::FrameBuffer & p_framebuffer );

	private:
		/**
		 *\copydoc		Castor3D::PostEffect::DoWriteInto
		 */
		virtual bool DoWriteInto( Castor::TextFile & p_file );

	public:
		static Castor::String const Type;
		static Castor::String const Name;
		static Castor::String const CombineMapPass0;
		static Castor::String const CombineMapPass1;
		static Castor::String const CombineMapPass2;
		static Castor::String const CombineMapPass3;
		static Castor::String const CombineMapScene;
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		void DoHiPassFilter( Castor3D::TextureLayout const & p_origin );
		void DoDownSample( Castor3D::TextureLayout const & p_origin );
		void DoCombine( Castor3D::TextureLayout const & p_origin );
		Castor3D::SamplerSPtr DoCreateSampler( bool p_linear );
		bool DoInitialiseHiPassProgram();
		bool DoInitialiseCombineProgram();

		Castor3D::SamplerSPtr m_linearSampler;
		Castor3D::SamplerSPtr m_nearestSampler;

		Castor3D::RenderPipelineUPtr m_hiPassPipeline;
		Castor3D::PushUniform1sSPtr m_hiPassMapDiffuse;

		Castor3D::GaussianBlurSPtr m_blur;

		Castor3D::MatrixUbo m_matrixUbo;
		Castor3D::RenderPipelineUPtr m_combinePipeline;

		Castor3D::Viewport m_viewport;
		Castor3D::BufferDeclaration m_declaration;
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_vertices;
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		Castor::real m_buffer[12];
		SurfaceArray m_hiPassSurfaces;
		SurfaceArray m_blurSurfaces;
		uint32_t m_size;
	};
}

#endif
