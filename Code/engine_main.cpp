#include <engine_ui/engine_ui.h>
#include <squelib.h>
#include <render/render.h>
#include <ecs/ecs.h>
#include <asset_manager/asset_manager.h>

int main(int argc, char** argv)
{
    // Libraries Init
    SQUE_LIB_Init("Sque Maker", SQ_INIT_DEFAULTS | SQ_INIT_DEBUG_RENDER);

    SQUE_DISPLAY_SetDropFileCallback(AssetManager_HandleDropFile);
    
    // Init Modules
    Render_Init();
    EngineUI_Init();
    AssetManager_Init();


    // Update Loop
    float dt = 1 / 60.;
    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        AssetManager_Update();
        // SqueLib Loop Needed
        SQUE_INPUT_Process(0);
        SQUE_RENDER_Clear(ColorRGBA(.5, .2, .2, 1.));
        // Engine Specific Loop
        ECS_Update(dt);
        EngineUI_Update(dt);
        Render_Update(dt);
        // Game Loop

        // SqueLib Loop Needed
        SQUE_DISPLAY_SwapAllBuffers();

    }

    SQUE_ECS_EarlyDestruct();
    
    // CleanUp Modules
    Render_CleanUp();
    EngineUI_CleanUp();

    // Libraries CleanUp

    SQUE_LIB_Close();
    return 0;
}