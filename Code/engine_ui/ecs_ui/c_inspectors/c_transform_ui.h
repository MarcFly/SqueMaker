#ifndef _ECS_TRANSFORM_UI_
#define _ECS_TRANSFORM_UI_

#include "../../../ecs/components/transform.h"
extern sque_vec<SQUE_Transform> transforms;

void InspectorTransform(uint32_t transform_ref)
{
	SQUE_Transform& t = transforms[transform_ref];
	ImGui::DragFloat3("Position", &t.position.x); //  , & t.position.y, & t.position.z);
	ImGui::DragFloat3("Rotation", &t.rotation.x);
	ImGui::DragFloat3("Scale", &t.scale.x);
}

#endif