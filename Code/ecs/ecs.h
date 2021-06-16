#ifndef _SQUE_ECS_
#define _SQUE_ECS_

#include <squelib.h>

struct SQUE_Entity
{
	uint32_t id = UINT32_MAX;
	uint32_t par_id = UINT32_MAX;
	
	char name[32] = "";

	uint32_t comp_ref = UINT32_MAX;
	uint32_t children_ref = UINT32_MAX;

	uint32_t tag_refs[5];
};

struct SQUE_Socket
{
	uint32_t id = UINT32_MAX;
	uint32_t par_id = UINT32_MAX;

	uint32_t component_ref = UINT32_MAX;
};

enum ECS_COMPONENTS
{
	SQUE_ECS_UNKNOWN = -1,
	SQUE_ECS_TRANSFORM = 0,
	SQUE_ECS_DRAWABLE,
	SQUE_ECS_CAMERA,

};

class SQUE_Component
{
public:
	SQUE_Component() {};
	virtual ~SQUE_Component() {};

	uint32_t id = UINT32_MAX;
	uint32_t par_id = UINT32_MAX;
	uint32_t type = SQUE_ECS_UNKNOWN;

	// This is so bad...
	SQUE_Component* PointerCopy() 
	{ 
		SQUE_Component* ret = (SQUE_Component*) (void*)(new char[sizeof(this)]);
		*ret = *this;
		return ret;
	}
	//virtual SQUE_Component* GenerateCopy() { return new SQUE_Component(*this); };
};

void SQUE_ECS_DeclareSubjects();
void SQUE_ECS_RegisterSubjects();


SQUE_Entity& SQUE_ECS_GetEntity(const uint32_t id);
// Fuzzy Search for Entity Names?
uint32_t SQUE_ECS_GetNumChildren(const uint32_t children_ref);
sque_vec<uint32_t>& SQUE_ECS_GetChildren(const uint32_t children_ref);
uint32_t SQUE_ECS_NewEntity(const uint32_t par_id = UINT32_MAX);
void SQUE_ECS_RebaseChild(const uint32_t id, const uint32_t par_id);
void SQUE_ECS_DeleteEntity(const uint32_t id);

void SQUE_ECS_UpdateBaseEntityList();
sque_vec<SQUE_Entity>& SQUE_ECS_GetNoParents();
sque_vec<SQUE_Entity>& SQUE_ECS_GetEntities();

void SQUE_ECS_EarlyDestruct();

// Helper to not get memory out
const sque_vec<SQUE_Component>& SQUE_ECS_GetComponentVec(const uint32_t comp_ref);
void SQUE_ECS_DeclareComponent(const uint32_t component_ref, SQUE_Component& component);

void SQUE_ECS_Component_AddType(const SQUE_Entity& entity, const uint32_t type);
void SQUE_ECS_Component_CopyFromGeneric(const SQUE_Entity& entity, const uint32_t type, const SQUE_Component* copy);
SQUE_Component* SQUE_ECS_Component_GetGenericP(const SQUE_Entity& entity, const uint32_t type);
SQUE_Component* SQUE_ECS_Component_AllocateCopy(const SQUE_Entity& entity, const uint32_t type, const uint32_t id);

// Template Definitions
template<class T>
void SQUE_ECS_AddComponent(const SQUE_Entity& entity)
{
	SQUE_Component& cpy = T::Create(entity.id);
	SQUE_ECS_DeclareComponent(entity.comp_ref, cpy);// T::Create());
}

template<class T>
void SQUE_ECS_CopyComponentTo(const SQUE_Entity& entity, const T& component)
{
	SQUE_Component cpy = T::Create(component);
	SQUE_ECS_DeclareComponent(entity.comp_ref, cpy);//T::Create(component));
}

static SQUE_Component invalid_comp;

template<class T>
T& SQUE_ECS_GetComponent(const SQUE_Entity& entity)
{
	const sque_vec<SQUE_Component>& comps = SQUE_ECS_GetComponentVec(entity.comp_ref);
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == T::static_type)
			return T::Get(comps[i].id);

	return *(T*)(&invalid_comp); // This is not good, returns invalid data
}

template<class T>
T& SQUE_ECS_GetComponentID(const SQUE_Entity& entity, const uint32_t comp_id)
{
	const sque_vec<SQUE_Component>& comps = SQUE_ECS_GetComponentVec(entity.comp_ref);
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == T::static_type && comps[i].id == comp_id)
			return T::Get(comps[i].id);

	return *(T*)(&invalid_comp); // This is not good, returns invalid data
}


void ECS_Update(float dt);

#endif