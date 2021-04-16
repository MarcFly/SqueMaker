#include "transform.h"
sque_vec<SQUE_Transform> transforms;
SQUE_Transform invalid;

SQUE_Component SQUE_Transform::Create()
{
    SQUE_Component ret;
    ret.ref = transforms.try_insert(SQUE_Transform());
    ret.type = type;
    ret.id = transforms[ret.ref].id;
    return ret;
}

SQUE_Component SQUE_Transform::Create(const SQUE_Transform& copy)
{
    SQUE_Component ret;
    ret.ref = transforms.try_insert(SQUE_Transform(copy));
    ret.type = type;
    ret.id = transforms[ret.ref].id;
    return ret;
}

SQUE_Transform& SQUE_Transform::GetByRef(uint32_t ref)
{
    return transforms[ref];
}

SQUE_Transform& SQUE_Transform::GetByID(uint32_t id)
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