#ifndef _ECS_COMPONENT_TRANSFORM_
#define _ECS_COMPONENT_TRANSFORM_

#include <squelib.h>
#include <glm/detail/type_quat.hpp>
typedef struct SQUE_Transform
{
    glm::vec3 position;
    glm::fquat rotation;
    glm::vec3 scale;
} SQUE_Transform;

#endif