#ifndef _ECS_DRAWABLE_UI_
#define _ECS_DRAWABLE_UI_

#include "../../../ecs/components/drawable.h"
#include "../../IconsForkAwesome.h"

extern sque_vec<SQUE_Drawable> drawables;

// only draw_mode should be modifiable
// Then which object is selected and then the materials

void InspectorDrawable(uint32_t drawable_ref)
{
    SQUE_Drawable& t = drawables[drawable_ref];

    // Selector for the Mesh Object
    ImGui::Button(ICON_FK_LINK);
    // Materials
}
#endif