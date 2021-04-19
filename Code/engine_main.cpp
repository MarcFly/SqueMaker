#include <engine_ui/engine_ui.h>
#include <squelib.h>
#include <render/render.h>
#include <ecs/ecs.h>

int main(int argc, char** argv)
{
    // Libraries Init
    SQUE_LIB_Init("Sque Maker");

    // Init Modules
    Render_Init();
    EngineUI_Init();
    SQUE_CHECK_RENDER_ERRORS();

    // Update Loop
    float dt = 1 / 60.;
    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        // SqueLib Loop Needed
        SQUE_INPUT_Process(0);
        SQUE_RENDER_Clear(ColorRGBA(.2, .2, .2, 1.));
        // Engine Specific Loop
        Render_Update(dt);
        EngineUI_Update(dt);
        
        // Game Loop

        // SqueLib Loop Needed
        
        SQUE_DISPLAY_SwapAllBuffers();
        SQUE_CHECK_RENDER_ERRORS();
    }

    SQUE_ECS_EarlyDestruct();
    
    // CleanUp Modules
    Render_CleanUp();
    EngineUI_CleanUp();

    // Libraries CleanUp

    SQUE_LIB_Close();
    return 0;
}