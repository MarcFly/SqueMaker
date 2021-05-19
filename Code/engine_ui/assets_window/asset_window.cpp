#include "./asset_window.h"
#include <asset_manager/asset_manager.h>

void SQUE_AssetWindow_Messager::DeclareSubjects()
{

}

void SQUE_AssetWindow_Messager::RegisterSubscriber()
{

}

void SQUE_AssetWindow_Messager::SubscribeToSubjects()
{

}

void SQUE_AssetWindow::Init()
{
	active = true;
	EngineUI_RegisterItem(this);
	name = "Asset_Window";

	EngineUI_RegisterMessager(&msgr);
}

void SQUE_AssetWindow::MenuBar(float dt)
{
	static ImVec2 button_size(40, 40);

	if (ImGui::Button(ICON_FK_REFRESH, button_size)); // Look for changes in disk
	ImGui::SameLine();

	// Search...
	ImVec2 cursor_pos = ImGui::GetCursorPos();
	ImVec2 winsize = ImGui::GetWindowSize();
	ImGui::SetCursorPosX(winsize.x - button_size.x * 1.5 - 3 * button_size.x * 2);
	ImGui::PushItemWidth(button_size.x * 3);
	ImGui::SliderInt("##SelectableSize", &selectable_size, 60, ImGui::GetWindowSize().x / 4);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SetCursorPosX(winsize.x - button_size.x * 1.5 - 3 * button_size.x);
	static char search[64];
	ImGui::PushItemWidth(button_size.x * 3);
	ImGui::InputText("##AssetWindowSearchBar", search, sizeof(search));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button(ICON_FK_SEARCH));
}

void SQUE_AssetWindow::Update(float dt)
{
	for (uint16_t i = 0; i < msgr.inbox.size(); ++i)
	{

	}

	
	if (ImGui::Begin(name, &active))
	{
		MenuBar(dt);
		
		ImGui::Separator();
		ImVec2 child_sizes(ImGui::GetWindowSize());
		child_sizes.x = child_sizes.x / 4;
		child_sizes.y -= ImGui::GetCursorPosY()+10;
		ImGui::BeginChild("##AssetsFoldersHierarchy", child_sizes, true);
		{

			const sque_vec<SQUE_Dir*>& dirs = AssetManager_GetBaseDirs();
			for (uint16_t i = 0; i < dirs.size(); ++i)
			{
				ImGuiTreeNodeFlags tmp_flags = NULL | (dirs[i]->children.size() == 0) * ImGuiTreeNodeFlags_Leaf | (current_folder == dirs[i]->id);

				bool open = ImGui::TreeNodeEx(&dirs[i]->id, tmp_flags, dirs[i]->name);
				if (ImGui::IsItemClicked())
				{
					current_folder = dirs[i]->id;
				}

				if (open && dirs[i]->children.size() > 0)
				{
					// Recursive...
				}
				if (open) ImGui::TreePop();
				// openflag clear
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();
		child_sizes.x *= 3;
		ImGui::BeginChild("##AssetsInFolder", child_sizes, true);
		{
			
			sque_vec<Asset*> folder_assets = AssetManger_GetAssetsDir(current_folder);
			char temp_id[32];
			for (uint16_t i = 0; i < folder_assets.size(); ++i)
			{
				bool selected = (selected_asset == folder_assets[i]->id);
				sprintf(temp_id, "##%u", folder_assets[i]->id);
				ImGui::Selectable(temp_id, &selected, NULL, ImVec2(selectable_size,selectable_size*1.3));
				
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void SQUE_AssetWindow::CleanUp()
{

}