#include "drawable.h"
sque_vec<SQUE_Drawable> drawables;
SQUE_Drawable invalid;

SQUE_Component SQUE_Drawable::Create()
{
    SQUE_Component ret;
    ret.ref = drawables.try_insert(SQUE_Drawable());
    ret.type = type;
    ret.id = drawables[ret.ref].id;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Drawable& copy)
{
    SQUE_Component ret;
    ret.ref = drawables.try_insert(SQUE_Drawable(copy));
    ret.type = type;
    ret.id = drawables[ret.ref].id;
    return ret;
}

SQUE_Drawable& SQUE_Drawable::GetByRef(uint32_t ref)
{
    return drawables[ref];
}

SQUE_Drawable& SQUE_Drawable::GetByID(uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id) return drawables[i];
    return invalid;
}

SQUE_Drawable::SQUE_Drawable()
{
    id = SQUE_RNG();
}

SQUE_Drawable::~SQUE_Drawable()
{

}