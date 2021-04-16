#ifndef _ECS_DRAWABLE_UI_
#define _ECS_DRAWABLE_UI_

#include "../../../ecs/components/drawable.h"
#include "../../IconsForkAwesome.h"


class InspectorDrawable : public ComponentInspector
{
public:
	InspectorDrawable();
	InspectorDrawable(const SQUE_Entity& e, const uint32_t comp_id);
	~InspectorDrawable();

	void Inspect() final;
	void ApplyChanges(const uint32_t par_id) final;
	SQUE_Drawable c;
};

InspectorDrawable::InspectorDrawable()
{
	sprintf(inspector_name, "Drawable##%u", c.id);
}

InspectorDrawable::InspectorDrawable(const SQUE_Entity& e, const uint32_t comp_id)
{
	SQUE_Drawable& c_ref = SQUE_ECS_GetComponentID<SQUE_Drawable>(e, comp_id);
	c = c_ref;
	c.id = c_ref.id;
	sprintf(inspector_name, "Drawable##%u", c.id);
}

InspectorDrawable::~InspectorDrawable()
{

}

void InspectorDrawable::Inspect()
{
	if (ImGui::CollapsingHeader(inspector_name))
	{
		// Draw mode,...
		// Selector for the Mesh Object
			ImGui::Button(ICON_FK_LINK);
		// Materials
	}
}

void InspectorDrawable::ApplyChanges(const uint32_t par_id)
{
	SQUE_Drawable& c_ref = SQUE_ECS_GetComponentID<SQUE_Drawable>(SQUE_ECS_GetEntityID(par_id), c.id);
	c_ref = c;
}
#endif