#ifndef _SQUE_RENDER_GRAPH_
#define _SQUE_RENDER_GRAPH_

#include <imnodes.h>
#include "../engine_ui.h"
#include "../../render/render.h"

class RenderStep_UI_Item
{
public:
	ImVec2 pos;
	
	void Update();
};

class SQUE_RenderGraph_Messager : public SQUE_Messager
{
public:
	void RegisterSubscriber() final;
	void DeclareSubjects() final;
	void SubscribeToSubjects() final;
};

#define RENDER_VALUE_ICON_TABLE(ENTRY)

class SQUE_RenderGraph : public SQUE_UI_Item
{
	sque_vec<RenderStep_UI_Item> steps;
	SQUE_RenderGraph_Messager msgr;
	SQUE_RMPopupMenu rm_menu;


	bool editing_name = false;
	// Render Graph Options
	ImColor title_color[RENDER_STEP_TYPE_TABLE_NUM_STATES];
	ImColor title_hovered_color[RENDER_STEP_TYPE_TABLE_NUM_STATES];
	ImColor title_selected_color[RENDER_STEP_TYPE_TABLE_NUM_STATES];
	//ImColor value_color[RENDER_VALUE_TABLE_NUM_STATES];
	//ImColor value_hovered_color[RENDER_VALUE_TABLE_NUM_STATES];
	
public:
	void Init() final;
	void Update(float dt) final;
	void CleanUp() final;

	void UpdateRMMenu();

private:
	void UpdateNodeTitleBar(RenderStep* step, int& node_item_id, const int node_id);
	void UpdateNodeOptions(RenderStep* step, int& node_item_id, const int node_id);
	void UpdateNodeInputs(RenderStep* step, int& node_item_id, const int node_id);
	void UpdateNodeOutputs(RenderStep* step, int& node_item_id, const int node_id);

};

#endif