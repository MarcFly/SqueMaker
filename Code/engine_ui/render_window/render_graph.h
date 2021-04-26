#ifndef _SQUE_RENDER_GRAPH_
#define _SQUE_RENDER_GRAPH_

#include <imnodes.h>
#include "../engine_ui.h"
#include "../../render/render.h"

// As the IDS are constructed sequentially, to later Compile the Graph the search for links will be:
// link id = value
// Step 1: used_ids = 0 + InputValues.size() + OutputValues.size();
// id < used_ids -> Search in Input or Output Values = InputValues.size < id -> Value = InputValues[id+(used_ids-id)]
// id > used_ids -> seach in next Step
// ...

// This does not work
// Basically you might add things before sequence and not adhere to previously made nodes....
// Have to redo with specific ids per Value and Step
// Then when creating the link you have to check that you don't link from fragment to VERTEX
// Should also check link types and such
struct AttributeLink
{
	uint32_t type;
	int out_id;
	int in_id;
};

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
	sq_list<AttributeLink> links;

	bool editing_name = false;
	// Render Graph Options
	ImColor title_color[RENDER_VALUE_TABLE_NUM_STATES];
	ImColor title_hovered_color[RENDER_VALUE_TABLE_NUM_STATES];
	ImColor title_selected_color[RENDER_VALUE_TABLE_NUM_STATES];
	//ImColor value_color[RENDER_VALUE_TABLE_NUM_STATES];
	//ImColor value_hovered_color[RENDER_VALUE_TABLE_NUM_STATES];
	
public:
	void Init() final;
	void Update(float dt) final;
	void CleanUp() final;

	void UpdateRMMenu();

private:
	void UpdateNodeTitleBar(RenderStep* step);
	void UpdateNodeOptions(RenderStep* step);
	void UpdateNodeInputs(RenderStep* step);
	void UpdateNodeOutputs(RenderStep* step);

};

#endif