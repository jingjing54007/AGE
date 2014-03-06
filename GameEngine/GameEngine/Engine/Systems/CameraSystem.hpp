#ifndef   __CAMERA_SYSTEM_HPP__
# define  __CAMERA_SYSTEM_HPP__

#include <Utils/MatrixConversion.hpp>
#include "System.h"
#include <Components/MeshRenderer.hh>
#include <Components/CameraComponent.hpp>
#include <Entities/EntityData.hh>
#include <Core/SceneManager.hh>
#include <Core/Renderer.hh>
#include <Systems/MeshRenderSystem.h>
#include <Utils/ScreenPosToWorldRay.hpp>
#include <Context/IRenderContext.hh>
#include <Utils/MatrixConversion.hpp>
#include <Text/FontManager.hh>

class CameraSystem : public System
{
public:
	CameraSystem(AScene *scene)
		: System(scene)
		, _filter(scene)
		, _renderDebugMethod(false)
	{}

	virtual ~CameraSystem(){}

	void setRenderDebugMode(bool t)
	{
		_renderDebugMethod = t;
	}

	bool getRenderDebugMode() const
	{
		return _renderDebugMethod;
	}

	void getRayFromMousePosOnScreen(glm::vec3 &from, glm::vec3 &to)
	{
		if (_filter.getCollection().size() == 0)
			return;
		auto mousePos = _scene->getEngine().getInstance<Input>()->getMousePosition();
		auto screenSize = _scene->getEngine().getInstance<IRenderContext>()->getScreenSize();
		auto cameraCpt = _filter.getCollection().begin()->get()->getComponent<Component::CameraComponent>();
		screenPosToWorldRay(mousePos.x, mousePos.y, screenSize.x, screenSize.y, cameraCpt->lookAtTransform, cameraCpt->projection, from, to);
	}

	void getRayFromCenterOfScreen(glm::vec3 &from, glm::vec3 &to)
	{
		if (_filter.getCollection().size() == 0)
			return;
		auto screenSize = _scene->getEngine().getInstance<IRenderContext>()->getScreenSize();
		auto centerPos = glm::vec2(screenSize) * glm::vec2(0.5f);
		auto cameraCpt = _filter.getCollection().begin()->get()->getComponent<Component::CameraComponent>();
		screenPosToWorldRay(centerPos.x, centerPos.y, screenSize.x, screenSize.y, cameraCpt->lookAtTransform , cameraCpt->projection, from, to);
	}


protected:
	EntityFilter _filter;

	bool _renderDebugMethod;

	virtual void updateBegin(double time)
	{
	}

	virtual void updateEnd(double time)
	{
//		_scene->getInstance<Renderer>()->getShader("2DText")->use();
		//_scene->getInstance<Renderer>()->getUniform("PerModel")->setUniform("model", glm::mat4(1));
		//_scene->getInstance<Renderer>()->getUniform("PerModel")->flushChanges();

		_scene->getInstance<FontManager>()->draw2DString("On s'en fout", "myFont", 70, glm::ivec2(0), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("De Ta Vielle vie de Merde", "myFont", 65, glm::ivec2(50, 50), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("Tu ne vaux meme pas 3 sous", "myFont", 60, glm::ivec2(100, 100), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("Rentre chez ta mere !!!", "myFont", 55, glm::ivec2(200, 200), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("Faire de l'ASCII ART avec ta bite !", "myFont", 50, glm::ivec2(300, 300), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("CeCi EsT uNe BiTe 8=====o", "myFont", 45, glm::ivec2(400, 400), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("Et ca un smyley :-D", "myFont", 40, glm::ivec2(600, 600), "2DText");
		_scene->getInstance<FontManager>()->draw2DString("Kill me please...", "myFont", 35, glm::ivec2(700, 700), "2DText");
		glFinish();
	}

	virtual void mainUpdate(double time)
	{
		static double totalTime = 0;
		unsigned int textureOffset = 0;
		auto &renderer = _scene->getEngine().getInstance<Renderer>();
		OpenGLTools::UniformBuffer *perFrameBuffer = _scene->getEngine().getInstance<Renderer>()->getUniform("PerFrame");

		for (auto e : _filter.getCollection())
		{
			auto camera = e->getComponent<Component::CameraComponent>();
			auto skybox = camera->getSkybox();

			auto cameraPosition = camera->getLookAtTransform();

			if (skybox != nullptr)
			{
				OpenGLTools::Shader *s = _scene->getEngine().getInstance<Renderer>()->getShader(camera->getSkyboxShader());
				assert(s != NULL && "Skybox does not have a shader associated");

				_scene->getEngine().getInstance<Renderer>()->getUniform("cameraUniform")->setUniform("projection", camera->getProjection());

				glm::mat4 t = cameraPosition;
				t[3][0] = 0;
				t[3][1] = 0;
				t[3][2] = 0;
				t[3][3] = 1;

				_scene->getEngine().getInstance<Renderer>()->getUniform("cameraUniform")->setUniform("view", t);
				_scene->getEngine().getInstance<Renderer>()->getUniform("cameraUniform")->flushChanges();

//				_engine.getInstance<Renderer>()->getFbo().bindDrawTargets(s->getTargets(), s->getTargetsNumber());

				s->use();


				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getId());
				glDepthMask(0);
				skybox->draw();
				glDepthMask(1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}

			totalTime += time;

			// Set les uniforms du block PerFrame
			perFrameBuffer->setUniform("projection", camera->getProjection());
			perFrameBuffer->setUniform("view", cameraPosition);
			perFrameBuffer->setUniform("time", (float)totalTime);
			perFrameBuffer->flushChanges();
			_scene->getSystem<MeshRendererSystem>()->render(time);
		}
	}

	virtual void initialize()
	{
		_filter.requireComponent<Component::CameraComponent>();
	}
};

#endif    //__CAMERA_SYSTEM_HPP__