#include "drawable.h"
sque_vec<SQUE_Drawable> drawables;
SQUE_Drawable invalid;

SQUE_Component SQUE_Drawable::Create()
{
    SQUE_Component ret;
    uint32_t ref = drawables.try_insert(SQUE_Drawable());
    ret.type = type;
    ret.id = drawables[ref].id;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Drawable& copy)
{
    SQUE_Component ret;
    uint32_t ref = drawables.try_insert(SQUE_Drawable());
    ret.type = type;
    ret.id = drawables[ref].id;
    drawables[ref] = copy;
    return ret;
}

SQUE_Drawable& SQUE_Drawable::Get(uint32_t id)
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