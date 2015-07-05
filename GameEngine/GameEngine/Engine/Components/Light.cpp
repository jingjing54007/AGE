#include <Components/Light.hh>
#include <Core/AScene.hh>
#include <Utils/MathematicTools.hh>
#include <Threads/ThreadManager.hpp>
#include <glm/glm.hpp>
#include <AssetManagement/AssetManager.hh>

#include "Render/Properties/AutoProperty.hpp"
#include "Render/ProgramResources/Types/Uniform/Vec4.hh"
#include "Render/ProgramResources/Types/Uniform/Vec3.hh"
#include "Render/ProgramResources/Types/Uniform/Vec1.hh"
#include "Render/ProgramResources/Types/Uniform/Mat4.hh"
#include "Render/Properties/Materials/Color.hh"
#include "Render/Properties/Materials/MapColor.hh"

#include "Graphic/DRBLightElementManager.hpp"
#include "Graphic/DRBData.hpp"
#include "Graphic/DRBPointLightData.hpp"

#ifdef EDITOR_ENABLED
#include <imgui\imgui.h>
#include <glm/gtc/type_ptr.hpp>
#endif

#ifdef max
#undef max
#endif

namespace AGE
{
	
	PointLightComponent::PointLightComponent(PointLightComponent const &o)
		: color(o.color),
		range(o.range),
		map(o.map)
	{
		postUnserialization();
	}

	void PointLightComponent::_copyFrom(const ComponentBase *model)
	{
		auto o = static_cast<const PointLightComponent*>(model);
		range = o->range;
		color = o->color;
		postUnserialization();
	}

	void PointLightComponent::reset()
	{
		color = glm::vec3(1);
		range = glm::vec3(1.0f, 0.01f, 0.001f);
		map = nullptr;
		_mapProp = nullptr;

		_colorProperty = nullptr;
		_ambiantColorProperty = nullptr;

		if (_graphicHandle.invalid() == false)
		{
			auto manager = entity->getScene()->getInstance<DRBLightElementManager>();
			manager->removePointLight(_graphicHandle);
		}
	}

	void PointLightComponent::init()
	{
		_mapProp = std::make_shared<MapColor>("sprite_light");
		_colorProperty = std::make_shared<AutoProperty<glm::vec4, Vec4>>("color_light");
		_ambiantColorProperty = std::make_shared<AutoProperty<glm::vec4, Vec4>>("ambient_color");

		auto manager = entity->getScene()->getInstance<DRBLightElementManager>();
		_graphicHandle = manager->addPointLight();

		_graphicHandle.getPtr()->getDatas()->globalProperties.add_property(_mapProp);
		_graphicHandle.getPtr()->getDatas()->globalProperties.add_property(_colorProperty);
		_graphicHandle.getPtr()->getDatas()->globalProperties.add_property(_ambiantColorProperty);

		map = entity->getScene()->getInstance<AssetsManager>()->getPointLightTexture();

		std::static_pointer_cast<DRBPointLightData>(_graphicHandle.getPtr()->getDatas())->setRange(glm::vec4(range.x, range.y, range.z, 1.0f));
	}

	float		PointLightComponent::computePointLightRange(float minValue, glm::vec3 const &attenuation)
	{
		glm::vec3 equation(attenuation.z, attenuation.y, attenuation.x - minValue);
		float discriminant = Mathematic::secondDegreeDiscriminant(equation);
		if (discriminant == 0)
		{
			return (Mathematic::resolveSecondDegree(equation));
		}
		else if (discriminant > 0)
		{
			glm::vec2	results = Mathematic::resolveSecondDegree(equation, discriminant);
			return (glm::max(results.x, results.y));
		}
		else
		{
			assert(!"The impossible has happenned :/");
			return (0);
		}
	}

	void PointLightComponent::postUnserialization()
	{
		init();
		_colorProperty->autoSet(color);
		std::static_pointer_cast<DRBPointLightData>(_graphicHandle.getPtr()->getDatas())->setRange(glm::vec4(range.x, range.y, range.z, 1.0f));
	}

#ifdef EDITOR_ENABLED
	void PointLightComponent::editorCreate()
	{}

	void PointLightComponent::editorDelete()
	{}

	bool PointLightComponent::editorUpdate()
	{
		bool modified = false;
		if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
		{
			_colorProperty->autoSet(glm::vec4(color.x, color.y, color.z, 1.0f));
			modified = true;
		}
		if (ImGui::SliderFloat3("Range", glm::value_ptr(range), 0.0f, 1.0f))
		{
			std::static_pointer_cast<DRBPointLightData>(_graphicHandle.getPtr()->getDatas())->setRange(glm::vec4(range.x, range.y, range.z, 1.0f));
			modified = true;
		}
		return modified;
	}
#endif
}