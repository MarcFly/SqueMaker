#ifndef _ECS_COMPONENT_TRANSFORM_
#define _ECS_COMPONENT_TRANSFORM_

#include <squelib.h>
#include <glm/detail/type_quat.hpp>
#include "../ecs.h"

class SQUE_Transform
{
public:
// Required for all components
    static SQUE_Component Create();
    static SQUE_Component Create(const SQUE_Transform& copy);
    static SQUE_Transform& GetByRef(uint32_t ref);
    static SQUE_Transform& GetByID(uint32_t id);
    static const uint32_t type = SQUE_ECS_TRANSFORM;
    uint32_t id = UINT32_MAX;

// Component Specific
    SQUE_Transform();
    ~SQUE_Transform();

    glm::vec3 position;
    glm::fquat rotation;
    glm::vec3 scale;

    SQUE_Transform& operator=(const SQUE_Transform& copy)
    {
        position = copy.position;
        rotation = copy.rotation;
        scale = copy.scale;
        return *this;
    }
};

#endif