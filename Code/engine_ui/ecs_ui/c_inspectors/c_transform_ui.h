#ifndef _ECS_TRANSFORM_UI_
#define _ECS_TRANSFORM_UI_

#include "../../../ecs/components/transform.h"
#include "../e_inspector.h"

class InspectorTransform : public ComponentInspector
{
public:
	InspectorTransform();
	InspectorTransform(const SQUE_Entity& e, const uint32_t comp_id);
	~InspectorTransform();

	void Inspect() override final;
	void ApplyChanges(const uint32_t par_id) override final;
	SQUE_Transform c;
};

InspectorTransform::InspectorTransform()
{
	sprintf(inspector_name, "Transform##%u", c.id);
}

InspectorTransform::InspectorTransform(const SQUE_Entity& e, const uint32_t comp_id)
{
	SQUE_Transform& c_ref = SQUE_ECS_GetComponentID<SQUE_Transform>(e, comp_id);
	c = c_ref;
	c.id = c_ref.id;
	sprintf(inspector_name, "Transform##%u", c.id);
}

InspectorTransform::~InspectorTransform()
{

}

void InspectorTransform::Inspect()
{
	if (ImGui::CollapsingHeader(inspector_name))
	{
		ImGui::DragFloat3("Position", &c.position.x);
		ImGui::DragFloat3("Rotation", &c.rotation.x);
		ImGui::DragFloat3("Scale", &c.scale.x);
	}
}

void InspectorTransform::ApplyChanges(const uint32_t par_id)
{
	SQUE_Transform& c_ref = SQUE_ECS_GetComponentID<SQUE_Transform>(SQUE_ECS_GetEntityID(par_id), c.id);
	c_ref = c;
}

#endif