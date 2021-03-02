#ifndef _ECS_COMPONENT_DRAWABLE_
#define _ECS_COMPONENT_DRAWABLE_

#include <squelib.h>
#include "../ecs.h"

typedef struct SQUE_Drawable
{
    // How to trasnfrom position with size with the size?
    glm::mat4x4 worldMatrix;
    SQUE_Mesh draw_data;
    uint32_t vertex_data_id;
    uint32_t index_data_id;

    sque_vec<uint32_t> material_refs;
} SQUE_Drawable;

SQUE_Component SQUE_ECS_AddDrawable();
SQUE_Component SQUE_ECS_AddDrawable(uint32_t par_ref);

#endif