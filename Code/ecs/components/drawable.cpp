#include "drawable.h"
sque_free_vec<SQUE_Drawable> drawables;
static SQUE_Drawable invalid_drawable;

SQUE_Component SQUE_Drawable::Create(const uint32_t _e_id)
{
    SQUE_Component ret;
    uint32_t ref = drawables.push(SQUE_Drawable());
    drawables[ref].par_id = _e_id;
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    ret.par_id = _e_id;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Drawable& copy, const uint32_t _e_id)
{
    SQUE_Component ret;
    uint32_t ref = drawables.push(SQUE_Drawable());
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    drawables[ref] = copy;
    drawables[ref].par_id = _e_id;
    ret.par_id = _e_id;
    return ret;
}

SQUE_Component SQUE_Drawable::Create(const SQUE_Component* copy, const uint32_t _e_id)
{
    SQUE_Drawable* t_copy = (SQUE_Drawable*)copy;
    SQUE_Component ret;
    uint32_t ref = drawables.push(SQUE_Drawable());
    ret.type = SQUE_Drawable::static_type;
    ret.id = drawables[ref].id;
    drawables[ref] = *t_copy;
    drawables[ref].par_id = _e_id;
    ret.par_id = _e_id;
    return ret;
}

SQUE_Drawable& SQUE_Drawable::Get(uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id) return drawables[i];
    return invalid_drawable;
}

SQUE_Component* SQUE_Drawable::GetGenericP(uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id)
            return &drawables[i];
    return NULL;
}

SQUE_Component* SQUE_Drawable::AllocateCopy(const uint32_t id)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
        if (drawables[i].id == id)
            return new SQUE_Drawable(drawables[i]);
    return NULL;
}

#include <render/render.h>
#include <glm/common.hpp>

void SQUE_Drawable::Update(float dt)
{
    for (uint32_t i = 0; i < drawables.size(); ++i)
    {
        if (drawables[i].mesh_id == UINT32_MAX)
            continue;
        // SHould create here the world matrix properly from transform
        Render_AddToDrawableList((float*)&drawables[i].worldMatrix[0], drawables[i].mesh_id);
    }
}

SQUE_Drawable::SQUE_Drawable()
{
    id = SQUE_RNG();
    type = SQUE_Drawable::static_type;
}

SQUE_Drawable::~SQUE_Drawable()
{

}