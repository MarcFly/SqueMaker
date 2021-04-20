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
   // RenderStep* ds = new RenderStep(); // default step
   // memcpy(ds->name, "Default Render Step", sizeof("Default Render Step"));
   // ds->state.BackUp();
   // 
   //render_steps.push_back(ds);
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