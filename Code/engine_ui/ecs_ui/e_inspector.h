#ifndef _ECS_INSPECTOR_UI_
#define _ECS_INSPECTOR_UI_

#include "../engine_ui.h"

#include "../../ecs/ecs.h"
#include "ecs_hierarchy.h"
#include "c_inspectors/c_ui_includeall.h"

extern sque_vec<SQUE_Entity> entities;
extern sque_vec<sque_vec<SQUE_Component>> components_refs;

//#include "../../ecs/components/transform.h"
//extern sque_vec<SQUE_Transform> transforms;

typedef void(*UIComponentUpdate)(uint32_t);

class SQUE_Inspector : public SQUE_UI_Item
{
	//Inspected_Entity inspected;

	uint32_t entity_id;
	char entity_name[32];
	uint32_t tag_refs[5];
	sque_vec<uint32_t> component_ids;
	sque_vec<UIComponentUpdate> components_gui;
	sque_vec<std::string> components_strs;

	bool inspecting;

	SQUE_RMPopupMenu rm_menu;
public:
	void Init() final
	{
		inspecting = false;
		active = true;
		EngineUI_RegisterItem(this);
		
		name = "ECS_Inspector";
	}

	void SetComponentFuncs(uint32_t component_ref)
	{
		components_gui.clear();
		component_ids.clear();
		components_strs.clear();
		sque_vec<SQUE_Component>& ref = components_refs[component_ref];
		for (uint16_t i = 0; i < ref.size(); ++i)
		{
			UIComponentUpdate update_func;
			switch (ref[i].type)
			{
			case SQUE_ECS_TRANSFORM:
				update_func = InspectorTransform;
				components_strs.push_back(std::string("Transform##") + std::to_string(i)); // prob bad, good enough for simplicity
				break;
			case SQUE_ECS_DRAWABLE:
				update_func = InspectorDrawable;
				components_strs.push_back("Drawable##"+ std::to_string(i));
				break;
			// Better a single switch and then iterate all inspectors
			}
			components_gui.push_back(update_func);
			component_ids.push_back(ref[i].ref);
		}
	}

	void SetInspectEntity(SQUE_H_Entry& entry)
	{
		SQUE_Entity& entity = entities[entry.ref];
		memcpy(entity_name, entity.name, 32);
		memcpy(tag_refs, entity.tag_refs, sizeof(uint32_t) * 5);
		entity_id = entity.id;

		SetComponentFuncs(entities[entry.ref].comp_ref);
	}

	void Update(float dt) final
	{
		if (ImGui::Begin(name, &active))
		{
			uint16_t s = components_gui.size();
			for (uint16_t i = 0; i < s; ++i)
			{
				if (ImGui::CollapsingHeader(components_strs[i].c_str()))
				{
					components_gui[i](component_ids[i]);
				}
			}

			ImGui::End();
		}
	}

	void CleanUp() final
	{

	}
};

#endif