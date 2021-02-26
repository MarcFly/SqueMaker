#include "transform.h"
sque_vec<SQUE_Transform> transforms;

uint32_t SQUE_ECS_AddTransform()
{
    SQUE_Transform new_transform;
    return transforms.try_insert(new_transform);
}

uint32_t SQUE_ECS_AddTransform(uint32_t par_ref)
{
    SQUE_Transform new_transform;
    new_transform = transforms[par_ref];
    return transforms.try_insert(new_transform);
}