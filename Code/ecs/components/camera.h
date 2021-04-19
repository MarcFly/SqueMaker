#ifndef _ECS_COMPONENT_CAMERA_
#define _ECS_COMPONENT_CAMERA_

#include <squelib.h>
#include "../ecs.h"

class SQUE_Camera : public SQUE_Component
{
public:
// Required for all components
    static SQUE_Component Create();
    static SQUE_Component Create(const SQUE_Camera& copy);
    static SQUE_Component Create(const SQUE_Component* copy) { return SQUE_Component(); }; // TODO
    static SQUE_Camera& Get(uint32_t id);
    static SQUE_Component* GetT(const uint32_t id) { return NULL; }; // TODO
    static SQUE_Component* AllocateCopy(const uint32_t id);

    static const uint32_t static_type = SQUE_ECS_CAMERA;

// Component Specific
    SQUE_Camera();
    ~SQUE_Camera();

    float near_plane;
    float far_plane;
    uint32_t resolution_x;
    uint32_t resolution_y;

    SQUE_Camera& operator=(const SQUE_Camera& copy)
    {
        near_plane = copy.near_plane;
        far_plane = copy.far_plane;
        resolution_x = copy.resolution_x;
        resolution_y = copy.resolution_y;

        return *this;
    }
};

#endif