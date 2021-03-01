#include "transform.h"
sque_vec<SQUE_Transform> transforms;

SQUE_Component SQUE_ECS_AddTransform()
{
    SQUE_Component ret;
    SQUE_Transform new_transform;
    ret.ref = transforms.try_insert(new_transform);
    ret.type = SQUE_ECS_TRANSFORM;

    return ret;
}

SQUE_Component SQUE_ECS_AddTransform(uint32_t par_ref)
{
    SQUE_Component ret;
    SQUE_Transform new_transform;
    new_transform = transforms[par_ref];
    ret.ref = transforms.try_insert(new_transform);
    ret.type = SQUE_ECS_TRANSFORM;

    return ret;
}