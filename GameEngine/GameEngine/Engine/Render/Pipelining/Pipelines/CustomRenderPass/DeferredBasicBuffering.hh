#pragma once

#include <Render/Pipelining/Render/FrameBufferRender.hh>
#include <glm\glm.hpp>

namespace AGE
{
	class Texture2D;

	class DeferredBasicBuffering : public FrameBufferRender
	{
	public:
		DeferredBasicBuffering(glm::uvec2 const &screenSize, 
								std::shared_ptr<PaintingManager> painterManager,
								std::shared_ptr<Texture2D> diffuse,
								std::shared_ptr<Texture2D> normal,
								std::shared_ptr<Texture2D> specular,
								std::shared_ptr<Texture2D> depth);
		virtual ~DeferredBasicBuffering() = default;

	protected:
		virtual void renderPass(std::list<BFCCullableObject*> const &meshs, RenderLightList &, CameraInfos const &);
		std::shared_ptr<Texture2D> _depth;
		std::shared_ptr<Texture2D> _occlusionDepth;

		std::vector<uint32_t> _depthPixels;
	};
}
