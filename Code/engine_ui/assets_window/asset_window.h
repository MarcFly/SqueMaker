#ifndef _SQUE_ASSET_WINDOW_
#define _SQUE_ASSET_WINDOW_

#include "../engine_ui.h"

class SQUE_AssetWindow_Messager : public SQUE_Messager
{
public:
	void RegisterSubscriber() final;
	void DeclareSubjects() final;
	void SubscribeToSubjects() final;
};

class SQUE_AssetWindow : public SQUE_UI_Item
{
	SQUE_AssetWindow_Messager msgr;
	uint32_t current_folder = UINT32_MAX;
	float selectable_size = 120;
	float min_selectable_size = 120;
	uint32_t selected_asset = UINT32_MAX;

	float margin = 0;
	float font_multiplier = .75f;

	void MenuBar(float dt);
	void UpdateDirectory(const SQUE_Dir* dir);
	void DrawSelectableItem(const uint32_t id, const uint32_t type, const char* asset_name, ImVec2 cursor, const float last_font_scale);
public:
	void Init() final;
	void Update(float dt) final;
	void CleanUp() final;
};

#endif