#include "./render.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

sque_vec<RenderStep*> render_steps; //  TODO: Cleanup RenderSteps... Valgrind 126/235
struct RenderDrawable
{
    float* matrix;
    void* data;
    SQUE_Mesh* descriptor;
};

sque_vec<RenderDrawable> r_drawables;
static float vertices_test[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

static SQUE_Mesh test_mesh;
static glm::mat4x4 view;
static glm::mat4x4 proj;
static glm::mat4x4 model;

void Render_Init()
{
    view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
    
    model = glm::rotate(glm::mat4(1.f), glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));

    SQUE_MESH_SetDrawConfig(&test_mesh, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
    SQUE_MESH_SetDataConfig(&test_mesh, 3);
    SQUE_MESH_GenBuffer(&test_mesh);
    SQUE_MESH_BindBuffer(test_mesh);
    SQUE_MESH_AddAttribute(&test_mesh, "aPos", SQUE_FLOAT, false, 3);
    SQUE_MESH_BlockOffsets(&test_mesh);
    SQUE_MESH_SetLocations(&test_mesh);
    SQUE_MESH_SendToGPU(test_mesh, vertices_test);
}

void Render_Update(float dt)
{
    // Render order is incorrect right now
    // When compiling, the order should be set in stages depending on linkage of attributes...
    
    int32_t vpx, vpy;
    SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
    proj = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

    SQUE_DISPLAY_MakeContextMain();
    SQUE_RenderState prev;
    prev.BackUp();
    //SQUE_RENDER_Clear(ColorRGBA(0, 0, 0, 0));
    static sque_vec<uint32_t> draw_buffers;
    //for (uint16_t i = 0; i < render_steps.size(); ++i)
    //{
    //    if (render_steps[i]->framebuffer.textures.size() <= 0 && render_steps[i]->framebuffer.id != prev.draw_framebuffer)
    //        continue;
    //    SQUE_FRAMEBUFFER_Bind(render_steps[i]->framebuffer.type, render_steps[i]->framebuffer.id);
    //    draw_buffers.clear();
    //    for (uint16_t j = 0; j < render_steps[i]->framebuffer.textures.size(); ++j)
    //        draw_buffers.push_back(SQUE_COLOR_ATTACHMENT0+j);
    //    SQUE_FRAMEBUFFER_SetDrawBuffers(draw_buffers.begin(), draw_buffers.size());
    //    SQUE_RENDER_SetViewport(0, 0, render_steps[i]->framebuffer.width, render_steps[i]->framebuffer.height);
    //
    //    SQUE_RENDER_Clear(ColorRGBA(1, 0, 0, .3), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
    //    SQUE_MESH_BindBuffer(test_mesh);
    //    SQUE_PROGRAM_Use(render_steps[i]->program.id);
    //    SQUE_RENDER_DrawVertices(test_mesh);
    //}

    for (uint16_t i = 0; i < render_steps.size(); ++i)
    {
        if (render_steps[i]->framebuffer.textures.size() <= 0 && render_steps[i]->framebuffer.id != prev.draw_framebuffer)
            continue;
        SQUE_FRAMEBUFFER_Bind(render_steps[i]->framebuffer.type, render_steps[i]->framebuffer.id);
        draw_buffers.clear();
        for (uint16_t j = 0; j < render_steps[i]->framebuffer.textures.size(); ++j)
            draw_buffers.push_back(SQUE_COLOR_ATTACHMENT0 + j);
        SQUE_FRAMEBUFFER_SetDrawBuffers(draw_buffers.begin(), draw_buffers.size());
        SQUE_RENDER_SetViewport(0, 0, render_steps[i]->framebuffer.width, render_steps[i]->framebuffer.height);

        SQUE_RENDER_Clear(ColorRGBA(1, 0, 0, .3), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        //SQUE_MESH_BindBuffer(test_mesh);
        SQUE_PROGRAM_Use(render_steps[i]->program.id);

        for (uint32_t j = 0; j < r_drawables.size(); ++j)
        {
            SetMatrix4(SQUE_PROGRAM_GetUniformID(render_steps[i]->program, "view"), glm::value_ptr(view));
            SetMatrix4(SQUE_PROGRAM_GetUniformID(render_steps[i]->program, "model"), glm::value_ptr(model));
            SetMatrix4(SQUE_PROGRAM_GetUniformID(render_steps[i]->program, "proj"), glm::value_ptr(proj));
            SQUE_RENDER_DrawIndices(*r_drawables[j].descriptor);
        }
    }

    r_drawables.clear();

    prev.SetUp();
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
        for (uint16_t j = 0; j < unifs.size(); ++j)
        {
            if(unifs[j] == id)
            {
                ret.id = id;
                ret.type = render_steps[i]->program.uniforms[j].type;
                return ret;
            }
        }

        const sque_vec<uint32_t>& texs = render_steps[i]->internal_texture_ids;
        for (uint16_t j = 0; j < texs.size(); ++j)
        {
            if (texs[j] == id)
            {
                ret.id = id;
                ret.type = SQUE_SAMPLER2D;
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
    SQUE_PROGRAM_GenerateID(&step->program.id);
    SQUE_FRAMEBUFFER_GenerateID(&step->framebuffer.id);
    SQUE_FRAMEBUFFER_GenRenderTypeID(&step->framebuffer.depth_buffer_id);

    render_steps.push_back(step);
}

#include "asset_manager/asset_manager.h"
void Render_CompileSteps()
{
    // Go throug the steps and generate linkage from vertex stage to fragment stage
    // Then Order them in tiers of execution
    SQUE_RenderState prev;
    prev.BackUp();

    uint16_t w, h;
    SQUE_DISPLAY_GetMainDisplaySize(&w, &h);
    // All framebuffers to be of display size... its bad but for now...

    for (uint32_t i = 0; i < render_steps.size(); ++i)
    {
        RenderStep* step = render_steps[i];
        SQUE_CtrlAsset* vert_asset = AssetManager_Get(step->vert_source_id);
        SQUE_CtrlAsset* frag_asset = AssetManager_Get(step->frag_source_id);
        if (vert_asset == NULL || frag_asset == NULL)
        {
            SQUE_PRINT(LT_WARNING, "Incomplete Render Step...");
            continue;
        }
        SQUE_Shader* vert_s = (SQUE_Shader*)vert_asset->datapack.metadata.raw_data;
        SQUE_Shader* frag_s = (SQUE_Shader*)frag_asset->datapack.metadata.raw_data;
        
        SQUE_SHADERS_SetSource(vert_s->id, vert_asset->datapack.data.raw_data);
        SQUE_SHADERS_SetSource(frag_s->id, frag_asset->datapack.data.raw_data);
        
        SQUE_SHADERS_Compile(vert_s->id);
        SQUE_SHADERS_Compile(frag_s->id);
        
        // Revise MemLeak from Here
        // Remember to only generate memory once, including GenerateIDs and similar...
        SQUE_PROGRAM_AttachShader(&step->program, *vert_s);
        SQUE_PROGRAM_AttachShader(&step->program, *frag_s);
        SQUE_PROGRAM_Link(step->program.id);
        SQUE_PROGRAM_CheckLinkLog(step->program.id);
        
        // Cache Uniforms and Generate IDs for linking
        SQUE_PROGRAM_CacheUniforms(&step->program);
        step->internal_uniform_ids.clear();
        for (uint16_t j = 0; j < step->program.uniforms.size(); ++j)
            step->internal_uniform_ids.push_back(SQUE_RNG());
        
        step->framebuffer.width = w;
        step->framebuffer.height = h;
        
        SQUE_FRAMEBUFFER_Bind(step->framebuffer.type, step->framebuffer.id);
        uint32_t fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        // Properly generate depth buffer...
        SQUE_FRAMEBUFFER_BindRenderType(step->framebuffer.depth_buffer_id);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        SQUE_FRAMEBUFFER_SetRenderTypeInfo(step->framebuffer.depth_type, w, h);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        SQUE_FRAMEBUFFER_AttachRenderType(SQUE_TYPE_DEPTH, step->framebuffer.depth_buffer_id);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        
        sque_vec<SQUE_Texture>& texs = step->framebuffer.textures;
        for (uint16_t j = 0; j < texs.size(); ++j)
        {            
            SQUE_TEXTURE_Bind(texs[j].id, texs[j].dim_format);
            SQUE_TEXTURE_ApplyAttributes(texs[j]);
            SQUE_TEXTURE_SendAs2DToGPU(texs[j], NULL);            
            
            SQUE_FRAMEBUFFER_AttachTexture(j, texs[j].id);
            fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        }
        
        sque_vec<uint32_t> draw_buffers;
        draw_buffers.clear();
        for (uint16_t j = 0; j < render_steps[i]->framebuffer.textures.size(); ++j)
            draw_buffers.push_back(SQUE_COLOR_ATTACHMENT0 + j);
        if(draw_buffers.size() > 0)
            SQUE_FRAMEBUFFER_SetDrawBuffers(draw_buffers.begin(), draw_buffers.size());
        
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
    }

    prev.SetUp();
}

void Render_AddToDrawableList(float* matrix, uint32_t data_id)
{
    const SQUE_CtrlAsset* a = AssetManager_GetConstAsset(data_id);
    
    EngineMesh* m = (EngineMesh*)a->datapack.data.raw_data;
    for (uint32_t i = 0; i < m->primitives.size(); ++i)
    {
        r_drawables.push_back(RenderDrawable());
        RenderDrawable* r = r_drawables.last();
        EngineMeshPrimitive* p = &m->primitives[i];
        r->data = p->vertices; //? indices ?
        r->descriptor = &p->config;
        r->matrix = matrix;
    }
    
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