#include "drawable.h"
sque_vec<SQUE_Drawable> drawables;

SQUE_Component SQUE_ECS_AddDrawable()
{
    SQUE_Component ret;
    SQUE_Drawable new_drawable;
    ret.ref = drawables.try_insert(new_drawable);
    ret.type = SQUE_ECS_DRAWABLE;

    return ret;
}

SQUE_Component SQUE_ECS_AddDrawable(uint32_t par_ref)
{
    SQUE_Component ret;
    SQUE_Drawable new_drawable;
    new_drawable = drawables[par_ref];
    ret.ref = drawables.try_insert(new_drawable);
    ret.type = SQUE_ECS_DRAWABLE;

    return ret;
}