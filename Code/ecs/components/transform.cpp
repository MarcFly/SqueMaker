#include "transform.h"
sque_vec<SQUE_Transform> transforms;
SQUE_Transform invalid;

SQUE_Component SQUE_Transform::Create()
{
    SQUE_Component ret;
    uint32_t ref = transforms.try_insert(SQUE_Transform());
    ret.type = type;
    ret.id = transforms[ref].id;
    return ret;
}

SQUE_Component SQUE_Transform::Create(const SQUE_Transform& copy)
{
    SQUE_Component ret;
    uint32_t ref = transforms.try_insert(SQUE_Transform());
    ret.type = type;
    ret.id = transforms[ref].id;
    transforms[ref] = copy;
    return ret;
}

SQUE_Component SQUE_Transform::Create(const SQUE_Component_Template* copy)
{
    SQUE_Transform* t_copy = (SQUE_Transform*)copy;
    SQUE_Component ret;
    uint32_t ref = transforms.try_insert(SQUE_Transform());
    ret.type = type;
    ret.id = transforms[ref].id;
    transforms[ref] = *t_copy;
    return ret;
}

SQUE_Transform& SQUE_Transform::Get(uint32_t id)
{
    for (uint32_t i = 0; i < transforms.size(); ++i)
        if (transforms[i].id == id) return transforms[i];
    return invalid;
}

SQUE_Transform::SQUE_Transform()
{
    id = SQUE_RNG();
}

SQUE_Transform::~SQUE_Transform()
{

}