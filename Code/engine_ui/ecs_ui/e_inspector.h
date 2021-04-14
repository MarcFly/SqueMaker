#ifndef _ECS_INSPECTOR_UI_
#define _ECS_INSPECTOR_UI_

#include "../engine_ui.h"
#include "ecs_hierarchy.h"

typedef void(*UIComponentUpdate)(uint32_t);

class SQUE_Inspector : public SQUE_UI_Item
{
	uint32_t entity_id;
	char entity_name[32];
	uint32_t tag_refs[5];
	sque_vec<uint32_t> component_ids;
	sque_vec<UIComponentUpdate> components_gui;
	sque_vec<std::string> components_strs;

	bool inspecting;

	SQUE_RMPopupMenu rm_menu;
public:
	void Init() final;

	void SetComponentFuncs(uint32_t component_ref);

	void SetInspectEntity(SQUE_H_Entry& entry);

	void Update(float dt) final;

	void CleanUp() final;
};

#endif