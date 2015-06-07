#pragma once

# include <Render/Pipelining/Render/ARender.hh>
#include <glm/glm.hpp>

namespace AGE
{

	class ScreenRender : public ARender
	{
	public:
		ScreenRender(glm::uvec2 const &screenSize, std::shared_ptr<PaintingManager> painterManager);
		ScreenRender(ScreenRender &&move);
		virtual ~ScreenRender() {}

	public:
		virtual IRender &render(std::list<BFCCullableObject*> const &meshs, RenderLightList &lights, CameraInfos const &infos) override final;
	
	protected:
		virtual void renderPass(std::list<BFCCullableObject*> const &, RenderLightList &, CameraInfos const &) = 0;
		glm::uvec2 viewport;
	};
}