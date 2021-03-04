#ifndef _SQUE_MAKER_RENDER_
#define _SQUE_MAKER_RENDER_

#include <squelib.h>


// Rethinking rendering
/*
    A render pipeline should be modifiable
    A render takes a bunch of steps -> A series of program executions
    Each with different uniforms, data to be passed in,...

    So a render step should:
    - Declare Input Types (meshes, sprites,...)
    - Have a Program to execute -> A program has the uniforms and attributes
    - A way to pass in the values to uniforms
    - Framebuffer -> Where to draw and how

    [] Input/Output Tag system
    The idea is that whatever entity, object, texture,... will have some render tags
    Each tag is used by different render steps, for example, the drawables will be setup
    in structs and a render step will have access to those structs using them after being
    setup. A RenderStep does not modify them but may produce more tags (aka render objects)

    Examples:
    - Geometry Processing Tag: entity processing that genereates transform and drawable object pair
    - Skeleton Forward Rendering Tag: entity processing that takes care of skeleton based animated ojbects
    - Sprite Forward Processing Tag: sets up the tilemap and what do draw where,... 
    - Stream Draw/ Static Draw/... tag: some data requires constant change, some mostly don't
    Then, a render Step just looks at the generated data for drawing (the uniforms) and uses the
    already setup data for rendering (meshes, textures,...)

    A rendering Step might take a look at other tags that are generated by other RenderSteps,
    but these steps are ordered, so one can only take a look at previously generated tags!
*/

typedef struct RenderTag
{
    // Has vectors for references to each render object type generated and data generated...
} RenderTag;

enum OutTypes
{
    OUT_Texture2D,
    OUT_Texture3D,
    OUT_Mesh,
    //...
};

typedef struct OutValue
{
    char name[32];
    uint32_t type;
    uint64_t data_size;
    void* data;
    uint32_t specific_data_type;
} OutValue;

typedef struct RenderStep
{
    char name[64];
    // Tagged input and outputs
    
    sque_vec<uint32_t> input_tags;
    sque_vec<OutValue> output_data;

    SQUE_RenderState state;

    uint32_t framebuffer_ref;
    uint32_t program_ref;
} RenderStep;

void Render_Init();
void Render_Update(float dt);
void Render_CleanUp();

sque_vec<RenderStep*>& Render_GetSteps();

RenderStep* Render_GetStep(uint32_t render_step_ref);

#endif