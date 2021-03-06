#include <Render/ProgramResources/Factory/UniformsFactory.hh>
#include <Render/ProgramResources/Types/Uniform/Vec4.hh>
#include <Render/ProgramResources/Types/Uniform/Vec3.hh>
#include <Render/ProgramResources/Types/Uniform/Mat4.hh>
#include <Render/ProgramResources/Types/Uniform/Vec1.hh>
#include <Render/ProgramResources/Types/Uniform/Vec2.hh>
#include <Render/ProgramResources/Types/Uniform/Mat4Array255.hh>
#include <Render/ProgramResources/Types/Uniform/Sampler/Sampler2D.hh>
#include <Render/ProgramResources/Types/Uniform/Sampler/Sampler3D.hh>
#include <Render/ProgramResources/Types/Uniform/Sampler/SamplerBuffer.hh>

#define LAMBDA_PROTO [this](GLint id, std::string &&name, bool isArray)

# define DECLAR_BUILDERS \
std::make_pair(GL_FLOAT_VEC4, LAMBDA_PROTO\
{ \
	return (std::make_shared<Vec4>(glm::vec4(0.0f), id, std::move(name))); \
}), \
std::make_pair(GL_FLOAT_VEC3, LAMBDA_PROTO\
{ \
	return (std::make_shared<Vec3>(glm::vec3(0.0f), id, std::move(name))); \
}), \
std::make_pair(GL_FLOAT_MAT4, LAMBDA_PROTO\
{ \
	return isArray == false ? std::static_pointer_cast<IProgramResources>(std::make_shared<Mat4>(glm::mat4(1.0f), id, std::move(name))) : std::static_pointer_cast<IProgramResources>(std::make_shared<Mat4Array255>(id, std::move(name))); \
}), \
std::make_pair(GL_FLOAT, LAMBDA_PROTO\
{ \
	return (std::make_shared<Vec1>(0.0f, id, std::move(name))); \
}), \
std::make_pair(GL_FLOAT_VEC2, LAMBDA_PROTO\
{ \
	return (std::make_shared<Vec2>(glm::vec2(0.0f), id, std::move(name))); \
}), \
std::make_pair(GL_SAMPLER_2D, LAMBDA_PROTO\
{\
	return (std::make_shared<Sampler2D>(id, std::move(name))); \
}), \
std::make_pair(GL_SAMPLER_2D_SHADOW, LAMBDA_PROTO\
{\
return (std::make_shared<Sampler2D>(id, std::move(name))); \
}),\
std::make_pair(GL_SAMPLER_CUBE, LAMBDA_PROTO\
{\
return (std::make_shared<Sampler3D>(id, std::move(name))); \
}), \
std::make_pair(GL_SAMPLER_BUFFER, LAMBDA_PROTO\
{\
return (std::make_shared<SamplerBuffer>(id, std::move(name))); \
})


namespace AGE
{

	UniformsFactory::UniformsFactory() :
		_blue_prints({ DECLAR_BUILDERS })
	{
		
	}

	std::shared_ptr<IProgramResources> UniformsFactory::build(GLenum mode, GLint id, std::string &&name, bool isArray)
	{
		for (auto &blue_print : _blue_prints) {
			if (mode == blue_print.first) {
				return (blue_print.second(id, std::move(name), isArray));
			}
		}
		return (std::shared_ptr<IProgramResources>(nullptr));
	}

	void UniformsFactory::reset()
	{
	}
}