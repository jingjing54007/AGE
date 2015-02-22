#include "SceneSelectorScene.hpp"
#include <imgui\imgui.h>
#include "AssetsEditorScene.hpp"
#include "WorldEditorScene.hpp"
#include <Core/Input.hh>
#include <SDL/SDL.h>

namespace AGE
{
	const std::string SceneSelectorScene::Name = "SceneSelector";

	SceneSelectorScene::SceneSelectorScene(AGE::Engine *engine)
		: AScene(engine)
	{
	}

	SceneSelectorScene::~SceneSelectorScene(void)
	{
	}

	bool SceneSelectorScene::userStart()
	{
		return true;
	}

	bool SceneSelectorScene::userUpdateBegin(double time)
	{
		ImGui::Begin("Assets Convertor", (bool*)1, ImGui::GetIO().DisplaySize, -1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);


		ImGui::BeginChild("Global Options", ImVec2(ImGui::GetIO().DisplaySize.x, 30), false);
		if (ImGui::Button("Asset Editor"))
		{
			getEngine()->disableScene(WorldEditorScene::Name);
			getEngine()->enableScene(AssetsEditorScene::Name, 1000);
		}
		ImGui::SameLine();
		if (ImGui::Button("World Editor"))
		{
			getEngine()->enableScene(WorldEditorScene::Name, 1000);
			getEngine()->disableScene(AssetsEditorScene::Name);
		}
		ImGui::EndChild();
		return true;
	}

	bool SceneSelectorScene::userUpdateEnd(double time)
	{
		if (getInstance<Input>()->getInput(SDLK_ESCAPE))
			return (false);
		return true;
	}
}