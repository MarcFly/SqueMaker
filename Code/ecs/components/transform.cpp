#include "transform.h"
sq_free_vec<SQUE_Transform> transforms;
static SQUE_Transform invalid_transform;

SQUE_Component SQUE_Transform::Create()
{
    SQUE_Component ret;
    uint32_t ref = transforms.push(SQUE_Transform());
    ret.type = SQUE_Transform::static_type;
    ret.id = transforms[ref].id;
    return ret;
}

SQUE_Component SQUE_Transform::Create(const SQUE_Transform& copy)
{
    SQUE_Component ret;
    uint32_t ref = transforms.push(SQUE_Transform());
    ret.type = SQUE_Transform::static_type;
    ret.id = transforms[ref].id;
    transforms[ref] = copy;
    return ret;
}

SQUE_Component SQUE_Transform::Create(const SQUE_Component* copy)
{
    SQUE_Transform* t_copy = (SQUE_Transform*)copy;
    SQUE_Component ret;
    uint32_t ref = transforms.push(SQUE_Transform());
    ret.type = SQUE_Transform::static_type;
    ret.id = transforms[ref].id;
    transforms[ref] = *t_copy;
    return ret;
}

SQUE_Transform& SQUE_Transform::Get(uint32_t id)
{
    for (uint32_t i = 0; i < transforms.size(); ++i)
        if (transforms[i].id == id) return transforms[i];
    return invalid_transform;
}

SQUE_Component* SQUE_Transform::GetGenericP(const uint32_t id)
{
    for (uint32_t i = 0; i < transforms.size(); ++i)
        if (transforms[i].id == id) 
            return &transforms[i];
    return NULL;
}

SQUE_Component* SQUE_Transform::AllocateCopy(const uint32_t id)
{
    for (uint32_t i = 0; i < transforms.size(); ++i)
        if (transforms[i].id == id)
            return new SQUE_Transform(transforms[i]);
    return NULL;
}

SQUE_Transform::SQUE_Transform()
{
    id = SQUE_RNG();
    type = SQUE_Transform::static_type;
}

SQUE_Transform::~SQUE_Transform()
{

}