#ifndef _ECS_COMPONENT_TRANSFORM_
#define _ECS_COMPONENT_TRANSFORM_

#include <squelib.h>
#include <glm/detail/type_quat.hpp>
#include "../ecs.h"

typedef struct SQUE_Transform
{
    glm::vec3 position;
    glm::fquat rotation;
    glm::vec3 scale;
} SQUE_Transform;

SQUE_Component SQUE_ECS_AddTransform();
SQUE_Component SQUE_ECS_AddTransform(uint32_t par_ref);

#endif