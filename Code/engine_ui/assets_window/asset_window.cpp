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

	if (ImGui::Button(ICON_FK_REFRESH, button_size)); // Look for changes in disk
	ImGui::SameLine();

	// Search...
	ImVec2 cursor_pos = ImGui::GetCursorPos();
	ImVec2 winsize = ImGui::GetWindowSize();
	ImGui::SetCursorPosX(winsize.x - button_size.x * 1.5 - 3 * button_size.x * 2);
	ImGui::PushItemWidth(button_size.x * 3);
	ImGui::SliderFloat("##SelectableSize", &selectable_size, 120, ImGui::GetWindowSize().x / 4.f, "");
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

void SQUE_AssetWindow::UpdateDirectory(const SQUE_Dir* dir)
{
	ImGuiTreeNodeFlags tmp_flags = NULL | (dir->children_ids.size() == 0) * ImGuiTreeNodeFlags_Leaf | (current_folder == dir->id);

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

void SQUE_AssetWindow::DrawSelectableItem(const uint32_t id, const uint32_t type, const char* asset_name, ImVec2 cursor, const float last_font_scale)
{
	bool selected = (selected_asset == id);
	char temp_id[32];
	ImVec2 s_vec(selectable_size, selectable_size);
	sprintf(temp_id, "##selectable%u", id);
	ImGui::Selectable(temp_id, &selected, NULL, s_vec);

	float fs_a = ImGui::GetFontSize();
	cursor.x += margin *3.f;
	cursor.y += margin;
	ImGui::SetCursorPos(cursor);
	sprintf(temp_id, "##child%u", id);
	ImGui::BeginChild(temp_id, ImVec2(s_vec.x,  s_vec.y - margin*2.f), false, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
	{
		double fs_b = ImGui::GetFontSize();
		const float prev_fs = ImGui::GetIO().FontGlobalScale;
		float fs = last_font_scale * font_multiplier * selectable_size / fs_b;
		ImGui::SetWindowFontScale(fs);
		ImGui::LabelText(temp_id, asset_icons[type]);
		ImGui::SetWindowFontScale(prev_fs);
	}
	ImGui::EndChild();

	ImGui::PushItemWidth(s_vec.x - margin * 2.f);
	cursor.x -= margin;
	cursor.y += s_vec.y - ImGui::GetFontSize() - margin * 3.f;
	ImGui::SetCursorPos(cursor);
	ImGui::LabelText("##", asset_name);
	ImGui::PopItemWidth();
}

static inline void SelectableNextCursor(ImVec2& cursor, float selectable_size, float win_width, float initial_x)
{
	cursor.x += selectable_size;
	if (cursor.x > (win_width - (selectable_size / 2.f)))
	{
		cursor.x = initial_x;
		cursor.y += selectable_size;
	}
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
		ImGui::BeginChild("##AssetsInFolder", child_sizes, true);
		{
			sque_vec<SQUE_CtrlAsset*> folder_assets = AssetManger_GetAssetsDir(current_folder);
			ImVec2 cursor(ImGui::GetCursorPos());
			float initial_x = cursor.x;

			margin = (selectable_size / 20.f);
			
			float fontsize_ratio = (selectable_size*.7f) / ImGui::GetFontSize();
			
			const ImGuiIO& io = ImGui::GetIO();
			
			float last_font_scale = io.FontGlobalScale;
			
			ImGui::SetWindowFontScale((last_font_scale/1.5f )* (selectable_size / min_selectable_size));
			
			const SQUE_Dir* curr_dir = AssetManager_GetDir(current_folder);
			for (uint16_t i = 0; curr_dir != NULL && i < curr_dir->children_ids.size(); ++i)
			{
				const SQUE_Dir* child = AssetManager_GetDir(curr_dir->children_ids[i]);
				DrawSelectableItem(child->id, 1, child->name, cursor, last_font_scale);
				SelectableNextCursor(cursor, selectable_size, ImGui::GetWindowSize().x, initial_x);
				ImGui::SetCursorPos(cursor);
			}

			for (uint16_t i = 0; i < folder_assets.size(); ++i)
			{
				DrawSelectableItem(folder_assets[i]->id, folder_assets[i]->type, folder_assets[i]->name, cursor, last_font_scale);
				SelectableNextCursor(cursor, selectable_size, ImGui::GetWindowSize().x, initial_x);
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