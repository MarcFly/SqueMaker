#ifndef _SQUE_ECS_
#define _SQUE_ECS_

#include <squelib.h>

struct SQUE_Entity
{
	uint32_t id;
	uint32_t par_id;
	
	char name[32];

	uint32_t comp_ref;
	uint32_t children_ref;

	uint32_t tag_refs[5];
};

struct SQUE_Socket
{
	uint32_t id;
	uint32_t par_id;

	uint32_t component_ref;
};

enum ECS_COMPONENTS
{
	SQUE_ECS_TRANSFORM = 0,

};

struct SQUE_Component
{
	uint32_t type;	
	uint32_t ref;
};

#endif