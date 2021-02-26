#ifndef _ECS_COMPONENT_DRAWABLE_
#define _ECS_COMPONENT_DRAWABLE_

#include <squelib.h>

typedef struct SQUE_Drawable
{
    // How to trasnfrom position with size with the size?
    glm::matrix4x4 worldMatrix;
    SQUE_Mesh draw_data;

    sque_vec<uint32_t> material_refs;
} SQUE_Drawable;

#endif