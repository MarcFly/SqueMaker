#ifndef _SQUE_RENDER_WINDOW_
#define _SQUE_RENDER_WINDOW_

#include "../engine_ui.h"
#include "../../ecs/components/camera.h"
#include "../../render/render.h"
#include <glm/detail/type_quat.hpp>

class SQUE_RenderWindow : public SQUE_UI_Item
{
private:
    glm::vec3 position;
    glm::fquat rotation;
    SQUE_Camera camera;
    uint32_t render_step_ref;
    uint32_t output_value_ref;
    // For usage

    // RenderStep Combo
    sque_vec<const char*> rs_names;

    // Texture Combo
    sque_vec<const char*> t_names;
    sque_vec<uint32_t> t_refs;
public:
    void Init() final;
    void Update(float dt) final;
    void CleanUp() final;
};

#endif