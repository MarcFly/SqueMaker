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

void SQUE_ECS_DeclareSubjects();
void SQUE_ECS_RegisterSubjects();


SQUE_Entity& SQUE_ECS_GetEntityID(const uint32_t id);
SQUE_Entity& SQUE_ECS_GetEntityRef(const uint32_t ref);
uint32_t SQUE_ECS_GetNumChildren(const uint32_t children_ref);
sque_vec<uint32_t>& SQUE_ECS_GetChildren(const uint32_t children_ref);
uint32_t SQUE_ECS_NewEntity();
uint32_t SQUE_ECS_NewChildEntity(const uint32_t par_id = -1);

void SQUE_ECS_AddComponent(const uint32_t entity_ref, SQUE_Component comp);

void SQUE_ECS_UpdateBaseEntityList();
sque_vec<SQUE_Entity>& SQUE_ECS_GetNoParents();
sque_vec<SQUE_Entity>& SQUE_ECS_GetEntities();

#endif