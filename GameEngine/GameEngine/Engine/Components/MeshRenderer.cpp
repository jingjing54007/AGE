#include "MeshRenderer.hh"
#include "Core/Engine.hh"
#include <Core/AScene.hh>	

namespace Component
{
	MeshRenderer::MeshRenderer() :
		Component::ComponentBase<MeshRenderer>(),
		_shaderShadow("ShadowMapping"),
		mesh(nullptr)
	{
	}

	MeshRenderer::~MeshRenderer(void)
	{
	}

	void MeshRenderer::init(std::shared_ptr<AMediaFile> r)
	{
		mesh = std::static_pointer_cast<ObjFile>(r);
	}

	void MeshRenderer::init(std::shared_ptr<ObjFile> r)
	{
		mesh = r;
	}

	std::shared_ptr<ObjFile> const &MeshRenderer::getMesh() const
	{
		return (mesh);
	}

	void MeshRenderer::reset()
	{
		mesh = nullptr;
	}

	void MeshRenderer::render_shadowmap(GLuint shadowTex, glm::mat4 const &lightVP)
	{
		glEnable(GL_DEPTH_TEST);
		glm::mat4 depthMVP = lightVP * _entity->getGlobalTransform();
		std::shared_ptr<OpenGLTools::UniformBuffer> lightMVP = _entity->getScene()->getInstance<Renderer>()->getUniform("Light");
		auto shader = _entity->getScene()->getInstance<Renderer>()->getShader(_shaderShadow);
		if (shader)
			shader->use();
		lightMVP->setUniform("lightMVP", depthMVP);
		lightMVP->flushChanges();
		for (std::size_t i = 0; i < mesh->material->materials.size(); ++i)
		{
			mesh->geometries[i].buffer.draw(GL_TRIANGLES);
		}
	}

	void MeshRenderer::render(bool shadow, GLuint shadowTex, glm::mat4 const &lightVP)
	{
		auto renderer = _entity->getScene()->getInstance<Renderer>();
		if (shadow)
		{
			glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0,
								 0.0, 0.5, 0.0, 0.0,
								 0.0, 0.0, 0.5, 0.0,
								 0.5, 0.5, 0.5, 1.0);
			auto perModelUniform = renderer->getUniform("PerModel");
			auto materialUniform = renderer->getUniform("MaterialBasic");
			auto shadowUniform = renderer->getUniform("LightBias");
			auto s = renderer->getShader(shader);
			if (s)
				s->use();
			glm::mat4 depthMVP = lightVP * _entity->getGlobalTransform();
			perModelUniform->setUniform("model", _entity->getGlobalTransform());
			perModelUniform->flushChanges();
			shadowUniform->setUniform("lightBiasMVP", biasMatrix * depthMVP);
			shadowUniform->flushChanges();
			for (std::size_t i = 0; i < mesh->material->materials.size(); ++i)
			{
				mesh->material->materials[i].setUniforms(materialUniform);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, shadowTex);
				materialUniform->flushChanges();
				mesh->geometries[i].buffer.draw(GL_TRIANGLES);
			}
		}
		else
		{
			std::shared_ptr<OpenGLTools::UniformBuffer> perModelUniform = _entity->getScene()->getInstance<Renderer>()->getUniform("PerModel");
			std::shared_ptr<OpenGLTools::UniformBuffer> materialUniform = _entity->getScene()->getInstance<Renderer>()->getUniform("MaterialBasic");
			auto s = renderer->getShader(shader);
			if (s)
				s->use();
			perModelUniform->setUniform("model", _entity->getGlobalTransform());
			perModelUniform->flushChanges();
			for (std::size_t i = 0; i < mesh->material->materials.size(); ++i)
			{
				mesh->material->materials[i].setUniforms(materialUniform);
				materialUniform->flushChanges();
				mesh->geometries[i].buffer.draw(GL_TRIANGLES);
			}
		}
	}

	std::string const &MeshRenderer::getShaderShadowMap() const
	{
		return (_shaderShadow);
	}

	void MeshRenderer::setShaderShadowMap(std::string const &shaderShadow)
	{
		_shaderShadow = shaderShadow;
	}
}
