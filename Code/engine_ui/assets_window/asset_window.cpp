#include "./asset_window.h"
#include <asset_manager/asset_manager.h>

// Copy basically from asset_manager and add the string macro for icon
#define ASSET_ICON_TABLE(ENTRY) \
        ENTRY(AIC_UNKNOWN, ICON_FK_QUESTION_CIRCLE) \
		ENTRY(AIC_FOLDER, ICON_FK_FOLDER_O) \
        ENTRY(AIC_META, ICON_FK_FILE_TEXT_O) \
        ENTRY(AIC_CUSTOM, ICON_FK_FILE_O) \
        ENTRY(AIC_IMAGE, ICON_FK_FILE_IMAGE_O) \
        ENTRY(AIC_OBJECT, ICON_FK_FILE_O) \
        ENTRY(AIC_MATERIAL, ICON_FK_FILE_O) \
        ENTRY(AIC_VERT_SHADER, ICON_FK_FILE_CODE_O) \
        ENTRY(AIC_FRAG_SHADER, ICON_FK_FILE_CODE_O) //\

static sque_dyn_arr<const char*> asset_icons = { ASSET_ICON_TABLE(EXPAND_AS_VALUE) };

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

	if (ImGui::Button(ICON_FK_REFRESH, button_size)) AssetManager_RefreshDirRecursive(current_folder); // Look for changes in disk
	ImGui::SameLine();

	// Item Sizes
	ImVec2 cursor_pos = ImGui::GetCursorPos();
	ImVec2 winsize = ImGui::GetWindowSize();
	ImGui::SetCursorPosX(winsize.x - button_size.x * 1.5 - 3 * button_size.x * 2);
	ImGui::PushItemWidth(button_size.x * 3);
	ImGui::SliderFloat("##SelectableSize", &selectable_size, min_selectable_size, ImGui::GetWindowSize().x / 4.f, "");
	ImGui::PopItemWidth();

	// Search
	ImGui::SameLine();
	ImGui::SetCursorPosX(winsize.x - button_size.x * 1.5 - 3 * button_size.x);
	ImGui::PushItemWidth(button_size.x * 3);
    ImGui::InputTextWithHint("##AssetWindowSearchBar", (search[0]!='\0') ? search : ICON_FK_SEARCH, search, sizeof(search));

	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Selectable("Aa##CaseSensitive", &selectable_case_sensitive);
}

bool SQUE_AssetWindow::SearchConstraints(const char* check)
{
	if(search[0] == '\0')
		return true;
	if(selectable_case_sensitive)
		return strstr(check, search) != NULL;
	else
	{
		static char str1[64];
		static char str2[64];
		static uint16_t s1;
		static uint16_t s2;
		s1 = strlen(check);
		s2 = strlen(search);
		for(uint16_t i = 0; i<s1; ++i)
			str1[i] = tolower(check[i]);
		str1[s1] = '\0';
		for(uint16_t i = 0; i<s2; ++i)
			str2[i] = tolower(search[i]);
		str2[s2] = '\0';
		return strstr(str1, str2) != NULL;
		
	}
}

void SQUE_AssetWindow::UpdateDirectory(const SQUE_Dir* dir)
{
	ImGuiTreeNodeFlags tmp_flags = NULL | ImGuiTreeNodeFlags_OpenOnArrow | (dir->children_ids.size() == 0) * ImGuiTreeNodeFlags_Leaf | (current_folder == dir->id);

	bool open = ImGui::TreeNodeEx(&dir->id, tmp_flags, dir->name);
	if (ImGui::IsItemClicked())
	{
		current_folder = dir->id;
	}


	for (uint32_t i = 0; open && i < dir->children_ids.size(); ++i)
	{
		const SQUE_Dir* child_dir = AssetManager_GetDir(dir->children_ids[i]);
		if(child_dir != NULL)
			UpdateDirectory(child_dir);
	}

	if (open) ImGui::TreePop();
	// openflag clear
}

void SQUE_AssetWindow::DrawSelectableItem(const uint32_t id, const uint32_t type, const char* asset_name, const float last_font_scale)
{
	const ImVec2 initial_cursor = ImGui::GetCursorPos();
	ImVec2 cursor = initial_cursor;

	bool selected = (selected_asset == id);
	char temp_id[32];
	ImVec2 s_vec(selectable_size, selectable_size+margin*2.f);
	
	cursor.x += margin * 3.5f;
	ImGui::SetCursorPos(cursor);
	sprintf(temp_id, "##child%u", id);
	ImGui::BeginChild(temp_id, ImVec2(s_vec.x,  2.f*s_vec.y/3.f), false, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
	{
		double fs_b = ImGui::GetFontSize();
		const float prev_fs = ImGui::GetIO().FontGlobalScale;
		float fs = last_font_scale * font_multiplier * selectable_size / fs_b;
		ImGui::SetWindowFontScale(fs);
		ImGui::LabelText(temp_id, asset_icons[type]);
		ImGui::SetWindowFontScale(prev_fs);
	}
	ImGui::EndChild();

	cursor.x = initial_cursor.x;
	cursor.y += margin/2.f + 2.f * s_vec.y / 3.f;// ImGui::GetCursorPos().y;
	ImGui::SetCursorPos(cursor);

	
	ImGui::PushTextWrapPos(cursor.x + selectable_size);
	// This is atrocious, but it makes it a bit better to look at...
	int32_t str_len = strlen(asset_name);
	if (selected_asset != id && str_len > 9)
	{
		static char cut_str[10] = ".........";
		memcpy(cut_str, asset_name, sizeof(cut_str) - 4);
		ImGui::Text(cut_str);
	}
	else if(str_len < 20)
		ImGui::Text(asset_name);
	else
	{
		static char cut_str[20] = "..................";
		memcpy(cut_str, asset_name, sizeof(cut_str) - 4);
		ImGui::Text(cut_str);
	}

	ImGui::PopTextWrapPos();
	
	ImGui::SetCursorPos(initial_cursor);
	sprintf(temp_id, "##selectable%u", id);
	
	ImGui::Selectable(temp_id, &selected, NULL, s_vec);

	// I don't know if this is the correct way to set this up
	// But looks and is simple for any kind of draggable
	// When dropping, it has to be checked, but feels bad to do so...
	if (SQUE_ItemDraggable::CheckStartDrag())
	{
		selected_asset = id;
		FileDraggable* file = new FileDraggable();
		file->file_id = id;
		EngineUI_StartDraggable(file);
	}
}

void SQUE_AssetWindow::UpdateItems(const ImVec2 item_win_size)
{
	ImGui::BeginChild("##AssetsInFolder", item_win_size, true);
	{
		const float prev_fs = ImGui::GetIO().FontGlobalScale;
		ImGui::SetWindowFontScale(.8f * selectable_size / min_selectable_size);

		sque_vec<SQUE_CtrlAsset*> folder_assets = AssetManger_GetAssetsDir(current_folder);
		ImVec2 cursor(ImGui::GetCursorPos());
		float initial_x = cursor.x;

		margin = (selectable_size / 20.f);

		float fontsize_ratio = (selectable_size * .7f) / ImGui::GetFontSize();

		const ImGuiIO& io = ImGui::GetIO();

		float last_font_scale = io.FontGlobalScale;

		//ImGui::SetWindowFontScale((last_font_scale/1.5f )* (selectable_size / min_selectable_size));

		const SQUE_Dir* curr_dir = AssetManager_GetDir(current_folder);
		for (uint16_t i = 0; curr_dir != NULL && i < curr_dir->children_ids.size(); ++i)
		{
			const SQUE_Dir* child = AssetManager_GetDir(curr_dir->children_ids[i]);
			if(!SearchConstraints(child->name))
				continue;

			DrawSelectableItem(child->id, 1, child->name, last_font_scale);
			cursor.x += selectable_size + margin * 2.f;
			if ((cursor.x + (selectable_size)) > (ImGui::GetWindowSize().x))
			{
				cursor.x = initial_x;
			}
			else ImGui::SameLine();
		}

		for (uint16_t i = 0; i < folder_assets.size(); ++i)
		{
			if(!SearchConstraints(folder_assets[i]->name))
				continue;

			DrawSelectableItem(folder_assets[i]->id, folder_assets[i]->type, folder_assets[i]->name, last_font_scale);
			cursor.x += selectable_size + margin * 2.f;
			if ((cursor.x + (selectable_size)) > (ImGui::GetWindowSize().x))
			{
				cursor.x = initial_x;
			}
			else ImGui::SameLine();
		}

		ImGui::SetWindowFontScale(prev_fs);

		if (ImGui::IsMouseClicked(SQUE_MOUSE_LEFT) && ImGui::IsWindowHovered())
			selected_asset = 0;
	}
	ImGui::EndChild();
}

void SQUE_AssetWindow::Update(float dt)
{
	for (uint16_t i = 0; i < msgr.inbox.size(); ++i)
	{

	}

	if (ImGui::Begin(name, &active))
	{
		MenuBar(dt);
		
		// Folder Structure
		ImGui::Separator();
		ImVec2 child_sizes(ImGui::GetWindowSize());
		child_sizes.x = child_sizes.x / 4;
		child_sizes.y -= ImGui::GetCursorPosY()+10;
		ImGui::BeginChild("##AssetsFoldersHierarchy", child_sizes, true);
		{

			const sque_vec<SQUE_Dir*>& dirs = AssetManager_GetBaseDirs();
			for (uint16_t i = 0; i < dirs.size(); ++i)
				UpdateDirectory(dirs[i]);
		}
		ImGui::EndChild();

		// Asset In Folder Viewer

		ImGui::SameLine();
		child_sizes.x *= 2;
		UpdateItems(child_sizes);

		ImGui::SameLine();
		child_sizes.x = ImGui::GetWindowSize().x - ImGui::GetCursorPosX()-15;
		ImGui::BeginChild("##AssetPreview", child_sizes, true);
		{
			// Preview render???
			const SQUE_CtrlAsset* asset = AssetManager_GetConstAsset(selected_asset);
			if (asset != NULL)
			{
				// Preview Render Image, could be embedded in the metadata on a fixed texture size...
				ImGui::TextWrapped("ID: %u", asset->id);
				ImGui::Text("Name:"); ImGui::SameLine();ImGui::TextWrapped("%s", asset->name);
				ImGui::Text("Location:"); ImGui::SameLine(); ImGui::TextWrapped("%s", asset->location);
				ImGui::TextWrapped("Current Users: %i", asset->current_users);
				ImGui::Text("Last Modification:"); ImGui::SameLine(); ImGui::TextWrapped("%d", asset->last_update);

				ImGui::Separator();
				// Metadata Info
				
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void SQUE_AssetWindow::CleanUp()
{

}
