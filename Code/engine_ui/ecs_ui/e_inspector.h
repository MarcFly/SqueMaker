#ifndef _ECS_INSPECTOR_UI_
#define _ECS_INSPECTOR_UI_

#include "../engine_ui.h"
#include "ecs_hierarchy.h"
#include "ecs/ecs.h"

class ComponentInspector
{
public:
	ComponentInspector() {};
	ComponentInspector(const SQUE_Entity& e, const uint32_t comp_id) {};

	virtual ~ComponentInspector() {};

	virtual void Inspect() {};
	virtual void ApplyChanges(const uint32_t par_id) {};
	char inspector_name[64] = "\0";
};

class SQUE_Inspector : public SQUE_UI_Item
{
	uint32_t entity_id;
	char entity_name[32];
	uint32_t tag_refs[5];
	sque_vec<ComponentInspector*> components;

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