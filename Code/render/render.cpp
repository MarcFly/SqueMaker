#include "./render.h"

sque_vec<RenderStep*> render_steps; //  TODO: Cleanup RenderSteps... Valgrind 126/235

static float vertices_test[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

static SQUE_Mesh test_mesh;

void Render_Init()
{
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
    
    SQUE_DISPLAY_MakeContextMain();
    SQUE_RenderState prev;
    prev.BackUp();
    //SQUE_RENDER_Clear(ColorRGBA(0, 0, 0, 0));
    static sque_vec<uint32_t> draw_buffers;
    for (uint16_t i = 0; i < render_steps.size(); ++i)
    {
        if (render_steps[i]->framebuffer.textures.size() <= 0)
            continue;
        SQUE_FRAMEBUFFER_Bind(render_steps[i]->framebuffer.type, render_steps[i]->framebuffer.id);
        //draw_buffers.clear();
        //for (uint16_t j = 0; j < render_steps[i]->framebuffer.textures.size(); ++j)
        //    draw_buffers.push_back(SQUE_COLOR_ATTACHMENT0+j);
        //SQUE_FRAMEBUFFER_SetDrawBuffers(draw_buffers.begin(), draw_buffers.size());
        
        SQUE_RENDER_Clear(ColorRGBA(1, 0, 0, .3), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_MESH_BindBuffer(test_mesh);
        SQUE_PROGRAM_Use(render_steps[i]->program.id);
        SQUE_RENDER_DrawVertices(test_mesh);
    }
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
        SQUE_SHADERS_CheckCompileLog(vert_s->id);
        SQUE_SHADERS_Compile(frag_s->id);
        SQUE_SHADERS_CheckCompileLog(frag_s->id);

        SQUE_PROGRAM_GenerateID(&step->program.id);
        SQUE_PROGRAM_AttachShader(&step->program, *vert_s);
        SQUE_PROGRAM_AttachShader(&step->program, *frag_s);
        SQUE_PROGRAM_Link(step->program.id);
        SQUE_PROGRAM_CheckLinkLog(step->program.id);

        SQUE_PROGRAM_CacheUniforms(&step->program);

        step->framebuffer.width = w/10;
        step->framebuffer.height = h/10;
        SQUE_FRAMEBUFFER_GenerateID(&step->framebuffer.id);
        SQUE_FRAMEBUFFER_GenRenderTypeID(&step->framebuffer.depth_buffer_id);
        SQUE_FRAMEBUFFER_Bind(step->framebuffer.type, step->framebuffer.id);
        uint32_t fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        // Properly generate depth buffer...
        SQUE_FRAMEBUFFER_BindRenderType(step->framebuffer.depth_buffer_id);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        SQUE_FRAMEBUFFER_SetRenderTypeInfo(step->framebuffer.depth_type, w/10, h/10);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        SQUE_FRAMEBUFFER_AttachRenderType(SQUE_TYPE_DEPTH, step->framebuffer.depth_buffer_id);
        fb_status = SQUE_FRAMEBUFFER_CheckStatus();

        sque_vec<SQUE_Texture>& texs = step->framebuffer.textures;
        for (uint16_t j = 0; j < texs.size(); ++j)
        {
            SQUE_TEXTURE_GenBufferIDs(1, &texs[i].id);
            SQUE_TEXTURE_Bind(texs[i].id, texs[i].dim_format);
            SQUE_TEXTURE_SendAs2DToGPU(texs[i], NULL);
            SQUE_TEXTURE_ApplyAttributes(texs[i]);

            SQUE_FRAMEBUFFER_AttachTexture(i, texs[i].id);
            fb_status = SQUE_FRAMEBUFFER_CheckStatus();
        }

        static sque_vec<uint32_t> draw_buffers;
        draw_buffers.clear();
        for (uint16_t j = 0; j < render_steps[i]->framebuffer.textures.size(); ++j)
            draw_buffers.push_back(SQUE_COLOR_ATTACHMENT0 + j);
        if(draw_buffers.size() > 0)
            SQUE_FRAMEBUFFER_SetDrawBuffers(draw_buffers.begin(), draw_buffers.size());

        fb_status = SQUE_FRAMEBUFFER_CheckStatus();
    }

    prev.SetUp();
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