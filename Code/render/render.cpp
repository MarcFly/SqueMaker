#include "./render.h"

// I don't think this will work out because there will be much more types of objects...
sque_vec<SQUE_Mesh> meshes;
sque_vec<SQUE_Texture> textures;
sque_vec<SQUE_Shader> shaders;
sque_vec<SQUE_Program> programs; 
sque_vec<SQUE_Framebuffer> framebuffers;

sque_vec<RenderStep*> render_steps; //  TODO: Cleanup RenderSteps... Valgrind 126/235

sque_vec<sque_vec<CompiledSteps>> compiled_steps;

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

void Render_AddStep(RenderStep* step)
{
    step->id = SQUE_RNG();
    render_steps.push_back(step);
    step->id = SQUE_RNG();
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

RenderStep* Render_GetStep(uint32_t render_step_ref)
{
    return render_steps[render_step_ref];
}

RenderValue* Render_GetValue(const uint32_t id)
{
    for (uint32_t i = 0; i < render_steps.size(); ++i)
    {
        RenderStep& step = *render_steps[i];
        if (id == step.shader_in.id) return &step.shader_in;
        if (id == step.shader_out.id) return &step.shader_out;
        for (uint16_t j = 0; j < step.input_data.size(); ++j)
            if (id == step.input_data[j].id) return &step.input_data[j];
        for (uint16_t j = 0; j < step.output_data.size(); ++j)
            if (id == step.output_data[j].id) return &step.output_data[j];
    }
    return NULL;

}