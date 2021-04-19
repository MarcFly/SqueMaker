#include "drawable.h"
sque_vec<SQUE_Drawable> drawables;
static SQUE_Drawable invalid_drawable;

SQUE_Component SQUE_Drawable::Create()
{
    SQUE_Component ret;
    uint32_t ref = drawables.try_insert(SQUE_Drawable());
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Drawable& copy)
{
    SQUE_Component ret;
    uint32_t ref = drawables.try_insert(SQUE_Drawable());
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    drawables[ref] = copy;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Component* copy)
{
    SQUE_Drawable* t_copy = (SQUE_Drawable*)copy;
    SQUE_Component ret;
    uint32_t ref = drawables.try_insert(SQUE_Drawable());
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    drawables[ref] = *t_copy;
    return ret;
}

SQUE_Drawable& SQUE_Drawable::Get(uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id) return drawables[i];
    return invalid_drawable;
}

SQUE_Component* SQUE_Drawable::AllocateCopy(const uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id)
            return new SQUE_Drawable(drawables[i]);
    return NULL;
}

SQUE_Drawable::SQUE_Drawable()
{
    id = SQUE_RNG();
    type = SQUE_Drawable::static_type;
}

SQUE_Drawable::~SQUE_Drawable()
{

}