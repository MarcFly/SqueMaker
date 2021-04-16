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
	SQUE_ECS_UNKNOWN = -1,
	SQUE_ECS_TRANSFORM = 0,
	SQUE_ECS_DRAWABLE,
	SQUE_ECS_CAMERA,

};

struct SQUE_Component
{
	uint32_t type;	
	uint32_t id;
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

void SQUE_ECS_DeleteEntityRef(const uint32_t ref);
void SQUE_ECS_DeleteEntityID(const uint32_t id);

void SQUE_ECS_UpdateBaseEntityList();
sque_vec<SQUE_Entity>& SQUE_ECS_GetNoParents();
sque_vec<SQUE_Entity>& SQUE_ECS_GetEntities();

void SQUE_ECS_EarlyDestruct();

// Helper to not get memory out
sque_vec<SQUE_Component>& GetComponentVec(const uint32_t comp_ref);

// Template Definitions
template<class T>
void SQUE_ECS_AddComponent(const SQUE_Entity& entity)
{
	GetComponentVec(entity.comp_ref).push_back(T::Create());
}

template<class T>
void SQUE_ECS_CopyComponentTo(const SQUE_Entity& entity, const T& component)
{
	GetComponentVec(entity.comp_ref).push_back(T::Create(component));
}

template<class T>
T& SQUE_ECS_GetComponent(const SQUE_Entity& entity)
{
	sque_vec<SQUE_Component>& comps = GetComponentVec(entity.comp_ref);
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == T::type)
			return T::GetByID(comps[i].id);
}

template<class T>
T& SQUE_ECS_GetComponentID(const SQUE_Entity& entity, const uint32_t comp_id)
{
	sque_vec<SQUE_Component>& comps = GetComponentVec(entity.comp_ref);
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == T::type && comps[i].id == comp_id)
			return T::GetByID(comps[i].id);
}

#endif