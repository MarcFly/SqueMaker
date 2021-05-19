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
	ImGui::SliderInt("##SelectableSize", &selectable_size, 120, ImGui::GetWindowSize().x / 4);
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
		child_sizes.x *= 2;
		ImGui::BeginChild("##AssetsInFolder", child_sizes, true);
		{
			
			sque_vec<Asset*> folder_assets = AssetManger_GetAssetsDir(current_folder);
			char temp_id[32];
			ImVec2 cursor(ImGui::GetCursorPos());
			uint32_t initial_x = cursor.x;
			float margin = selectable_size / 20.f;
			float fontsize_ratio = (selectable_size*.7f) / ImGui::GetFontSize();
			const ImGuiIO& io = ImGui::GetIO();
			float last_font_scale = io.FontGlobalScale;
			ImVec2 s_vec(selectable_size, selectable_size);
			ImGui::SetWindowFontScale((last_font_scale/1.5f )* (selectable_size / 120.f));
			for (uint16_t i = 0; i < folder_assets.size(); ++i)
			{
				bool selected = (selected_asset == folder_assets[i]->id);
				ImVec2 cursor_start = cursor;
				sprintf(temp_id, "##%u", folder_assets[i]->id);
				ImGui::Selectable(temp_id, &selected, NULL, s_vec);
				
				cursor.x += margin;
				cursor.y += margin;

				ImGui::SetCursorPos(cursor);
				ImGui::BeginChild(temp_id, ImVec2(s_vec.x+margin*4, s_vec.y+s_vec.y/10), false, ImGuiWindowFlags_NoInputs);
				{
					ImGui::SetCursorPosX(margin * 4.f);
					ImGui::SetWindowFontScale(last_font_scale * fontsize_ratio);
					ImGui::LabelText("##", ICON_FK_FILE_CODE_O);
				}
				ImGui::EndChild();
				

				ImGui::PushItemWidth(s_vec.x - margin * 2);
				ImGui::SetCursorPos(cursor);

				cursor.y += s_vec.y - ImGui::GetFontSize() - margin * 3;
				ImGui::SetCursorPos(cursor);
				ImGui::LabelText("##", folder_assets[i]->name);

				// File name

				// Set Cursor for next Selectable
				cursor = cursor_start;
				cursor.x += selectable_size;
				if (cursor.x > ImGui::GetWindowSize().x + selectable_size / 2)
				{
					cursor.x = initial_x;
					cursor.y += selectable_size * 1.3;
				}
				ImGui::SetCursorPos(cursor);
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();
		child_sizes.x = ImGui::GetWindowSize().x - ImGui::GetCursorPosX()-15;
		ImGui::BeginChild("##AssetPreview", child_sizes, true);
		{

		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void SQUE_AssetWindow::CleanUp()
{

}