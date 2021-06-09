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
	
	ImColor title_color;

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

// bad but i can't find any other easy way
static const char* RenderType_String(const uint32_t type)
{
	switch (type)
	{
		case SQUE_FLOAT:	return "float";
		case SQUE_INT:		return "int";
		case SQUE_DOUBLE:	return "double";
		case SQUE_FVEC2:	return "fvec2";
		case SQUE_FVEC3:	return "fvec3";
		case SQUE_FVEC4:	return "fvce4";
		case SQUE_FMAT4:	return "fmat4";
		case SQUE_SAMPLER2D:return "texture2D";
	}
	return "unsupported_type";
}

static const char* TextureType_String(const uint32_t type)
{
	switch (type)
	{
		case SQUE_RGBA:		return "RGBA";
		case SQUE_RGBA8:	return "RGBA8";
		case SQUE_R8:		return "R8";
		case SQUE_R16:		return "R16";
		case SQUE_RGB:		return "RGB";
		case SQUE_RGB8:		return "RGB8";
	}
	return "unsupported_type";
}

class SQUE_RenderGraph : public SQUE_UI_Item
{
	sque_vec<RenderStep_UI_Item> steps;
	SQUE_RenderGraph_Messager msgr;
	SQUE_RMPopupMenu rm_menu;
	sque_list<AttributeLink> links;

	bool editing_name = false;

	sque_vec<uint32_t> selected_outputs;
	
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