#ifndef _ECS_COMPONENT_DRAWABLE_
#define _ECS_COMPONENT_DRAWABLE_

#include <squelib.h>
#include "../ecs.h"
#include <glm.hpp>

class SQUE_Drawable : public SQUE_Component
{
public:

// Required for all components
    static SQUE_Component Create(const uint32_t _e_id = UINT32_MAX );
    static SQUE_Component Create(const SQUE_Drawable& copy, const uint32_t _e_id = UINT32_MAX);
    static SQUE_Component Create(const SQUE_Component* copy, const uint32_t _e_id = UINT32_MAX);
    static SQUE_Drawable& Get(uint32_t id);
    static SQUE_Component* GetGenericP(const uint32_t id);
    static SQUE_Component* AllocateCopy(const uint32_t id);
    static const uint32_t static_type = SQUE_ECS_DRAWABLE;

// Drawable Specific
    

    SQUE_Drawable();
    ~SQUE_Drawable();

    glm::mat4x4 worldMatrix;
    uint32_t mesh_id;


    // Copy For the data, ID is copied only if when required like in UI
    SQUE_Drawable& operator=(const SQUE_Drawable& copy)
    {
        mesh_id = copy.mesh_id;
        worldMatrix = copy.worldMatrix;
        return *this;
    }
};

// DoDrawables();

#endif