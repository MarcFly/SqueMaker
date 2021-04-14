#include "e_inspector.h"

#include "../../ecs/ecs.h"
#include "c_inspectors/c_ui_includeall.h"

extern sque_vec<SQUE_Entity> entities;
extern sque_vec<sque_vec<SQUE_Component>> components_refs;

void SQUE_Inspector::Init()
{
	inspecting = false;
	active = true;
	EngineUI_RegisterItem(this);

	name = "ECS_Inspector";
}

void SQUE_Inspector::SetComponentFuncs(uint32_t component_ref)
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
			components_strs.push_back("Drawable##" + std::to_string(i));
			break;
			// Better a single switch and then iterate all inspectors
		}
		components_gui.push_back(update_func);
		component_ids.push_back(ref[i].ref);
	}
}

void SQUE_Inspector::SetInspectEntity(SQUE_H_Entry& entry)
{
	SQUE_Entity& entity = entities[entry.ref];
	memcpy(entity_name, entity.name, 32);
	memcpy(tag_refs, entity.tag_refs, sizeof(uint32_t) * 5);
	entity_id = entity.id;

	SetComponentFuncs(entities[entry.ref].comp_ref);
}

void SQUE_Inspector::Update(float dt)
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

void SQUE_Inspector::CleanUp()
{

}
