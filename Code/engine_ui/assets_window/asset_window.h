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
	int selectable_size = 60;
	uint32_t selected_asset = UINT32_MAX;
	void MenuBar(float dt);

public:
	void Init() final;
	void Update(float dt) final;
	void CleanUp() final;
};

#endif