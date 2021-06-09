#ifndef _ECS_COMPONENT_TRANSFORM_
#define _ECS_COMPONENT_TRANSFORM_

#include <squelib.h>
#include <glm/gtc/quaternion.hpp>
#include "../ecs.h"

class SQUE_Transform : public SQUE_Component
{
public:
    static SQUE_Component Create(const SQUE_Component* copy, const uint32_t _e_id = UINT32_MAX);
// Required for all components
    static SQUE_Component Create(const uint32_t _e_id = UINT32_MAX);
    static SQUE_Component Create(const SQUE_Transform& copy, const uint32_t _e_id = UINT32_MAX);
    static SQUE_Transform& Get(const uint32_t id);
    static SQUE_Component* GetGenericP(const uint32_t id);
    static SQUE_Component* AllocateCopy(const uint32_t id);
    static const uint32_t static_type = SQUE_ECS_TRANSFORM;

// Component Specific
    SQUE_Transform();
    ~SQUE_Transform();

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    SQUE_Transform& operator=(const SQUE_Transform& copy)
    {
        position = copy.position;
        rotation = copy.rotation;
        scale = copy.scale;
        return *this;
    }

    //SQUE_Component* GenerateCopy() { return new SQUE_Transform(*this); }
};

#endif