#ifndef _SQUE_RENDER_WINDOW_
#define _SQUE_RENDER_WINDOW_

#include "../engine_ui.h"
#include "../../ecs/components/camera.h"
#include "../../render/render.h"
#include <glm/gtc/quaternion.hpp>

class SQUE_RenderWindow : public SQUE_UI_Item
{
private:
    glm::vec3 position;
    glm::quat rotation;
    SQUE_Camera camera;
    uint32_t render_step_ref;
    uint32_t output_value_ref;
public:
    void Init() final;
    void Update(float dt) final;
    void CleanUp() final;
};

#endif