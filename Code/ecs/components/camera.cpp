#include "camera.h"
sque_vec<SQUE_Camera> cameras;
static SQUE_Camera invalid_cam;

SQUE_Component SQUE_Camera::Create()
{
    SQUE_Component ret;
    uint32_t ref = cameras.try_insert(SQUE_Camera());
    ret.type = SQUE_Camera::static_type;
    ret.id = cameras[ref].id;
    return ret;
}

SQUE_Component SQUE_Camera::Create(const SQUE_Camera& copy)
{
    SQUE_Component ret;
    uint32_t ref = cameras.try_insert(SQUE_Camera());
    ret.type = SQUE_Camera::static_type;
    ret.id = cameras[ref].id;
    cameras[ref] = copy;
    return ret;
}

SQUE_Component SQUE_Camera::Create(const SQUE_Component* copy)
{
    SQUE_Camera* t_copy = (SQUE_Camera*)copy;
    SQUE_Component ret;
    uint32_t ref = cameras.try_insert(SQUE_Camera());
    ret.type = SQUE_Camera::static_type;
    ret.id = cameras[ref].id;
    cameras[ref] = *t_copy;
    return ret;
}

SQUE_Camera& SQUE_Camera::Get(uint32_t id)
{
    for (uint32_t i = 0; i < cameras.size(); ++i)
        if (cameras[i].id == id) return cameras[i];
    return invalid_cam;
}

SQUE_Component* SQUE_Camera::GetGenericP(const uint32_t id)
{
    for (uint32_t i = 0; i < cameras.size(); ++i)
        if (cameras[i].id == id)
            return &cameras[i];
    return NULL;
}

SQUE_Component* SQUE_Camera::AllocateCopy(const uint32_t id)
{
    for (uint32_t i = 0; i < cameras.size(); ++i)
        if (cameras[i].id == id)
            return new SQUE_Camera(cameras[i]);
    return NULL;
}

SQUE_Camera::SQUE_Camera()
{
    id = SQUE_RNG();
    //this->type = SQUE_Camera::type;
}

SQUE_Camera::~SQUE_Camera()
{

}