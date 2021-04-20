#ifndef _ECS_COMPONENT_DRAWABLE_
#define _ECS_COMPONENT_DRAWABLE_

#include <squelib.h>
#include "../ecs.h"

class SQUE_Drawable : public SQUE_Component
{
public:

// Required for all components
    static SQUE_Component Create();
    static SQUE_Component Create(const SQUE_Drawable& copy);
    static SQUE_Component Create(const SQUE_Component* copy);
    static SQUE_Drawable& Get(uint32_t id);
    static SQUE_Component* GetGenericP(const uint32_t id);
    static SQUE_Component* AllocateCopy(const uint32_t id);
    static const uint32_t static_type = SQUE_ECS_DRAWABLE;

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