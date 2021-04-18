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

#define EXPAND_AS_ENUM(a,b) a,
#define EXPAND_AS_FUN(a,b) b,

typedef SQUE_Component CreateFun();
typedef SQUE_Component TemplateCreateFun(const SQUE_Component_Template* copy);

#define COMPONENT_CREATE_TABLE(ENTRY) \
		ENTRY(ILLEGAL_ECS_TRANSFORM, SQUE_Transform::Create) \
		ENTRY(ILLEGAL_ECS_DRAWABLE, SQUE_Drawable::Create) \
		ENTRY(ILLEGAL_ECS_CAMERA, SQUE_Camera::Create) \


enum {
	COMPONENT_CREATE_TABLE(EXPAND_AS_ENUM)
	COMPONENT_CREATE_NUM_STATES
};



static CreateFun* CreateFunTable[COMPONENT_CREATE_NUM_STATES] = {
	COMPONENT_CREATE_TABLE(EXPAND_AS_FUN)
};

static TemplateCreateFun* TemplateCreateFunTable[COMPONENT_CREATE_NUM_STATES] = {
	COMPONENT_CREATE_TABLE(EXPAND_AS_FUN)
};

#endif