#ifndef _ECS_DRAWABLE_UI_
#define _ECS_DRAWABLE_UI_

#include "../../../ecs/components/drawable.h"
#include "../../IconsForkAwesome.h"
#include "../../assets_window/asset_window.h"

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
	c.par_id = c_ref.par_id;
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
		const SQUE_CtrlAsset* mesh = AssetManager_GetConstAsset(c.mesh_id);
		if (mesh != NULL)
			ImGui::Text(mesh->name);
		else
			ImGui::Text("No mesh set...");

		ImVec2 min = ImGui::GetItemRectMin();

		ImGui::SameLine();
		
		
		if (ImGui::Button(ICON_FK_LINK))
		{
			// Popup to search between all time .sq_mesh files
		}

		ImVec2 max = ImGui::GetItemRectMax();

		FileDraggable* mesh_drag = (FileDraggable*)EngineUI_CheckDroppedDraggable(min, max);
		if(mesh_drag != NULL && (AssetManager_GetConstAsset(mesh_drag->file_id)->type == FT_CUSTOM))
		{
			c.mesh_id = mesh_drag->file_id;
			// Send message for the drawable to have the new mesh...
			SQUE_Drawable& c_ref = SQUE_ECS_GetComponentID<SQUE_Drawable>(SQUE_ECS_GetEntity(c.par_id), c.id);
			c_ref.mesh_id = c.mesh_id;
		}

		// Materials
	}

	if (ImGui::CollapsingHeader(inspector_name))
	{
	};
}

void InspectorDrawable::ApplyChanges(const uint32_t par_id)
{
	SQUE_Drawable& c_ref = SQUE_ECS_GetComponentID<SQUE_Drawable>(SQUE_ECS_GetEntity(par_id), c.id);
	c_ref = c;
}
#endif