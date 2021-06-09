#include "./render.h"

sque_vec<RenderStep*> render_steps; //  TODO: Cleanup RenderSteps... Valgrind 126/235

void Render_Init()
{

}

void Render_Update(float dt)
{

}

void Render_CleanUp()
{
    for (uint16_t i = 0; i < render_steps.size(); ++i)
    {
        delete render_steps[i];
    }
    render_steps.clear();
}

RenderValue_Link Render_GetValue(const uint32_t id)
{
    RenderValue_Link ret;

    for(uint16_t i = 0; i < render_steps.size(); ++i)
    {
        const sque_vec<uint32_t>& unifs = render_steps[i]->internal_uniform_ids; 
        for (uint16_t j = 0; j < unifs.size(); ++i)
        {
            if(unifs[i] == id )
            {
                ret.id = id;
                ret.type = render_steps[i]->program.uniforms[j].type;
                return ret;
            }
        }

        const sque_vec<uint32_t>& texs = render_steps[i]->internal_texture_ids;
        for (uint16_t j = 0; j < texs.size(); ++j)
        {
            if (texs[i] == id)
            {
                ret.id = id;
                ret.type = render_steps[i]->framebuffer.textures[j].use_format;
                return ret;
            }
        }
    }

    ret.type = ret.id = UINT32_MAX;
    return ret;
}

void Render_AddStep(RenderStep* step)
{
    step->id = SQUE_RNG();
    render_steps.push_back(step);
}

void Render_CompileSteps()
{
    // Go throug the steps and generate linkage from vertex stage to fragment stage
    // Then Order them in tiers of execution
}

sque_vec<RenderStep*>& Render_GetSteps()
{
    return render_steps;
}

RenderStep* Render_GetStep(uint32_t render_step_id)
{
    for (uint32_t i = 0; i < render_steps.size(); ++i)
        if (render_step_id == render_steps[i]->id)
            return render_steps[i];
    return NULL;
}