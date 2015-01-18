#include <Components/AudioListener.hpp>

namespace AGE
{
	using namespace Component;

	AudioListener::AudioListener() :
		Component::ComponentBase<Component::AudioListener>()
	{}

	AudioListener::~AudioListener(void)
	{}

	void AudioListener::init(AScene *)
	{
	}

	void AudioListener::reset(AScene *)
	{}
}