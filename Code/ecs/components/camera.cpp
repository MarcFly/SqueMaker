#include "camera.h"
sque_vec<SQUE_Camera> cameras;
SQUE_Camera invalid;

SQUE_Component SQUE_Camera::Create()
{
    SQUE_Component ret;
    ret.ref = cameras.try_insert(SQUE_Camera());
    ret.type = type;
    ret.id = cameras[ret.ref].id;
    return ret;
}

SQUE_Component SQUE_Camera::Create(const SQUE_Camera& copy)
{
    SQUE_Component ret;
    ret.ref = cameras.try_insert(SQUE_Camera(copy));
    ret.type = type;
    ret.id = cameras[ret.ref].id;
    return ret;
}

SQUE_Camera& SQUE_Camera::GetByRef(uint32_t ref)
{
    return cameras[ref];
}

SQUE_Camera& SQUE_Camera::GetByID(uint32_t id)
{
    for (uint32_t i = 0; i < cameras.size(); ++i)
        if (cameras[i].id == id) return cameras[i];
    return invalid;
}

SQUE_Camera::SQUE_Camera()
{
    id = SQUE_RNG();
}

SQUE_Camera::~SQUE_Camera()
{

}