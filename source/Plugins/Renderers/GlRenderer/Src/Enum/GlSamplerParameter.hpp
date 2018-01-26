/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlSamplerParameter
	{
		GL_SAMPLER_PARAMETER_BORDER_COLOR = 0x1004,
		GL_SAMPLER_PARAMETER_MAG_FILTER = 0x2800,
		GL_SAMPLER_PARAMETER_MIN_FILTER = 0x2801,
		GL_SAMPLER_PARAMETER_WRAP_S = 0x2802,
		GL_SAMPLER_PARAMETER_WRAP_T = 0x2803,
		GL_SAMPLER_PARAMETER_WRAP_R = 0x8072,
		GL_SAMPLER_PARAMETER_MIN_LOD = 0x813A,
		GL_SAMPLER_PARAMETER_MAX_LOD = 0x813B,
		GL_SAMPLER_PARAMETER_COMPARE_MODE = 0x884C,
		GL_SAMPLER_PARAMETER_COMPARE_FUNC = 0x884D,
		GL_SAMPLER_PARAMETER_COMPARE_REF_TO_TEXTURE = 0x884E
	};
	std::string getName( GlSamplerParameter value );
}
