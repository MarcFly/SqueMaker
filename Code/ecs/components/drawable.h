#ifndef _ECS_COMPONENT_DRAWABLE_
#define _ECS_COMPONENT_DRAWABLE_

#include <squelib.h>
#include "../ecs.h"

class SQUE_Drawable
{
public:

// Required for all components
    static SQUE_Component Create();
    static SQUE_Component Create(const SQUE_Drawable& copy);
    static SQUE_Component Create(const SQUE_Component_Template* copy) { return SQUE_Component(); }; // TODO
    static SQUE_Drawable& Get(uint32_t id);
    static const uint32_t type = SQUE_ECS_DRAWABLE;
    uint32_t id = UINT32_MAX;

// Drawable Specific
    

    SQUE_Drawable();
    ~SQUE_Drawable();

    glm::mat4x4 worldMatrix;
    SQUE_Mesh draw_data;
    uint32_t vertex_data_id;
    uint32_t index_data_id;
    uint32_t material_refs[5];

    // Copy For the data, ID is copied only if when required like in UI
    SQUE_Drawable& operator=(const SQUE_Drawable& copy)
    {
        draw_data = copy.draw_data;
        vertex_data_id = copy.vertex_data_id;
        index_data_id = index_data_id;
        memcpy(material_refs, copy.material_refs, sizeof(material_refs));
        worldMatrix = copy.worldMatrix;
        return *this;
    }
};

#endif