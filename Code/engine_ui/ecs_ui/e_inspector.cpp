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
	for (uint32_t i = 0; i < components.size(); ++i)
		delete components[i];
	components.clear();

	sque_vec<SQUE_Component>& ref = components_refs[component_ref];
	SQUE_Entity& e = SQUE_ECS_GetEntityID(entity_id);

	for (uint16_t i = 0; i < ref.size(); ++i)
	{
		switch (ref[i].type)
		{
		case SQUE_ECS_TRANSFORM:
			components.push_back(new InspectorTransform(e, ref[i].id));
			break;
		case SQUE_ECS_DRAWABLE:
			components.push_back(new InspectorDrawable(e, ref[i].id));
			break;
		}
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
		uint16_t s = components.size();
		for (uint16_t i = 0; i < s; ++i)
			components[i]->Inspect();

		ImGui::End();
	}
}

void SQUE_Inspector::CleanUp()
{

}
