#include "MeshRenderer.hh"

#include "Core/Engine.hh"

namespace Component
{
	MeshRenderer::MeshRenderer() :
		Component::ComponentBase<MeshRenderer>(),
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

	std::shared_ptr<ObjFile> const &MeshRenderer::getMesh() const
	{
		return (mesh);
	}

	void MeshRenderer::reset()
	{
		mesh = nullptr;
	}

	void MeshRenderer::render()
	{
		OpenGLTools::UniformBuffer *perModelUniform = _entity->getScene()->getEngine().getInstance<Renderer>().getUniform("PerModel");
		OpenGLTools::UniformBuffer *materialUniform = _entity->getScene()->getEngine().getInstance<Renderer>().getUniform("MaterialBasic");
		auto s = _entity->getScene()->getEngine().getInstance<Renderer>().getShader(shader);
		if (s)
			s->use();
		perModelUniform->setUniform("model", _entity->getGlobalTransform());
		perModelUniform->flushChanges();
		for (unsigned int i = 0; i < mesh->geometries.size(); ++i)
		{
			std::static_pointer_cast<MaterialFile>(mesh->material)->materials[i].setUniforms(materialUniform);
			materialUniform->flushChanges();
			mesh->geometries[i].buffer.draw(GL_TRIANGLES);
		}
	}
}
