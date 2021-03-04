#ifndef _ECS_COMPONENT_CAMERA_
#define _ECS_COMPONENT_CAMERA_

#include <squelib.h>
#include "../ecs.h"

typedef struct SQUE_Camera
{
    float near_plane;
    float far_plane;

    uint32_t resolution_x;
    uint32_t resolution_y;
} SQUE_Camera;

SQUE_Component SQUE_ECS_AddCamera();



#endif