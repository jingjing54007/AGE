#pragma once

#include <Components/Component.hh>
#include <glm/glm.hpp>

namespace AGE
{
	struct SpotLightData
	{
		glm::mat4		lightVP;
		glm::vec4		positionPower;
		glm::vec4		colorRange;
		int				shadowId;
		unsigned int	padding[3];

		SpotLightData() { }

		SpotLightData(SpotLightData const &o)
		{
			*this = o;
		}

		~SpotLightData()
		{
		}

		SpotLightData &operator=(SpotLightData const &o)
		{
			lightVP = o.lightVP;
			positionPower = o.positionPower;
			colorRange = o.colorRange;
			shadowId = o.shadowId;
			return (*this);
		}
	};

	namespace Component
	{
		struct SpotLight : public ComponentBase < SpotLight >
		{
			SpotLightData		lightData;
			glm::mat4			projection;

			SpotLight();

			SpotLight(SpotLight const &o);

			virtual ~SpotLight();

			void		updateLightData(const glm::mat4 &globalTransform);

			virtual void reset(AScene *);

			SpotLight &operator=(SpotLight const &o);

			void	init(AScene *);

			template <typename Archive>
			void serialize(Archive &ar);

		};
	}
}