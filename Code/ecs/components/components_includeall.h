#ifndef _ECS_COMPONENTS_INCLUDEALL_
#define _ECS_COMPONENTS_INCLUDEALL_

#include "transform.h"
#include "drawable.h"
#include "camera.h"

// The BIG Create Switch Function
// - IF my theory is correct, it will happen that there is a need for a kilometric
//	switch function that dependingon type and call teh templated functions per type...
// Hope Jesus can get me out of this, but it is needed for holding onto multiple components,
// initializing from serialized memory (you can't pass from enum data to TYPE...

// X Macros...?


// This is so illegal...
// Still this will fail should someone add components from elsewhere, like runtime...
// How would someone expand without having to thouch this file?

typedef SQUE_Component CreateFun();
typedef SQUE_Component TemplateCreateFun(const SQUE_Component* copy);
typedef SQUE_Component* GetFun(const uint32_t id);
typedef SQUE_Component* AllocateCopyFun(const uint32_t id);


#define COMPONENT_CREATE_TABLE(ENTRY) \
		ENTRY(CREATE_ECS_TRANSFORM, SQUE_Transform::Create) \
		ENTRY(CREATE_ECS_DRAWABLE, SQUE_Drawable::Create) \
		ENTRY(CREATE_ECS_CAMERA, SQUE_Camera::Create) // \

#define COMPONENT_GET_TABLE(ENTRY) \
		ENTRY(GET_ECS_TRANSFORM, SQUE_Transform::GetGenericP) \
		ENTRY(GET_ECS_DRAWABLE, SQUE_Drawable::GetGenericP) \
		ENTRY(GET_ECS_CAMERA, SQUE_Camera::GetGenericP) // \

#define COMPONENT_ALLOCATE_COPY_TABLE(ENTRY) \
		ENTRY(ALLOCATE_COPY_ECS_TRANSFORM, SQUE_Transform::AllocateCopy) \
		ENTRY(ALLOCATE_COPY_ECS_DRAWABLE, SQUE_Drawable::AllocateCopy) \
		ENTRY(ALLOCATE_COPY_ECS_CAMERA, SQUE_Camera::AllocateCopy) // \


enum {
	COMPONENT_CREATE_TABLE(EXPAND_AS_ENUM)
	COMPONENT_CREATE_NUM_STATES
};

enum {
	COMPONENT_GET_TABLE(EXPAND_AS_ENUM)
	COMPONENT_GET_NUM_STATES
};

enum {
	COMPONENT_ALLOCATE_COPY_TABLE(EXPAND_AS_ENUM)
	COMPONENT_ALLOCATE_COPY_NUM_STATES
};

static CreateFun* CreateFunTable[COMPONENT_CREATE_NUM_STATES] = {
	COMPONENT_CREATE_TABLE(EXPAND_AS_VALUE)
};

static TemplateCreateFun* TemplateCreateFunTable[COMPONENT_CREATE_NUM_STATES] = {
	COMPONENT_CREATE_TABLE(EXPAND_AS_VALUE)
};

static GetFun* ComponentGetFunTable[COMPONENT_GET_NUM_STATES] = {
	COMPONENT_GET_TABLE(EXPAND_AS_VALUE)
};

static AllocateCopyFun* ComponentAllocateCopyFunTable[COMPONENT_ALLOCATE_COPY_NUM_STATES] = {
	COMPONENT_ALLOCATE_COPY_TABLE(EXPAND_AS_VALUE)
};

// TODO: Change all xmacros to use the sque_vec initializer, so they become expandable macros!
static sque_dyn_arr<GetFun*> TestGetFunVec = { COMPONENT_GET_TABLE(EXPAND_AS_VALUE) };

#endif