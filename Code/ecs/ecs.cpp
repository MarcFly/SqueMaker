#include <ecs/ecs.h>

sque_vec<SQUE_Entity> entities;
static SQUE_Entity invalid_entity;
sque_vec<SQUE_Entity> base_entities;
sque_free_vec<sque_vec<uint32_t>> children_refs;
sque_free_vec<sque_vec<SQUE_Component>> components_refs;
sque_vec<char*> tags;

sque_vec<SQUE_Entity> cached_entity_search;

void SQUE_ECS_EarlyDestruct()
{
	entities.~sque_vec();
	base_entities.~sque_vec();
	children_refs.~sque_free_vec();

	for (int i = 0; i < components_refs.size(); ++i)
		components_refs[i].~sque_vec();

	components_refs.~sque_free_vec();
}

SQUE_Entity& SQUE_ECS_GetEntity(const uint32_t id)
{
	//for (uint16_t i = 0; i < cached_entity_search.size(); ++i)
		//if (id == cached_entity_search[i].id) return cached_entity_search[i];

	for (uint16_t i = 0; i < entities.size(); ++i)
		if (id == entities[i].id)
			{
				cached_entity_search.push_back(entities[i]);
				return entities[i];
			}
	return invalid_entity;
}

uint32_t SQUE_ECS_GetNumChildren(const uint32_t child_ref)
{
	return children_refs[child_ref].size();
}

sque_vec<uint32_t>& SQUE_ECS_GetChildren(const uint32_t child_ref)
{
	return children_refs[child_ref];
}

uint32_t SQUE_ECS_NewEntity(const uint32_t par_id)
{
	entities.push_back(SQUE_Entity());
	SQUE_Entity& entity = *entities.last();

	entity.comp_ref = components_refs.push(sque_vec<SQUE_Component>());
	entity.children_ref = children_refs.push(sque_vec<uint32_t>());
	entity.id = SQUE_RNG();
	entity.par_id = par_id;

	if (par_id != UINT32_MAX)
		children_refs[SQUE_ECS_GetEntity(par_id).children_ref].push_back(entity.id);
	else
		base_entities.push_back(entity);

	return entity.id;
}

void SQUE_ECS_RebaseChild(const uint32_t id, const uint32_t par_id)
{
	SQUE_Entity& e = SQUE_ECS_GetEntity(id);
	if (e.par_id != UINT32_MAX)
	{
		SQUE_Entity& par_e = SQUE_ECS_GetEntity(e.par_id);
		sque_vec<uint32_t>& par_children = SQUE_ECS_GetChildren(par_e.children_ref);
		for (uint32_t i = 0; i < par_children.size(); ++i)
		{
			if (par_children[i] == e.id)
			{
				SQUE_Swap(par_children.last(), &par_children[i]);
				par_children.pop_back();
				break;
			}
		}
	}

	if (par_id != UINT32_MAX)
	{
		SQUE_Entity& par_e = SQUE_ECS_GetEntity(par_id);
		sque_vec<uint32_t>& par_children = SQUE_ECS_GetChildren(par_e.children_ref);
		par_children.push_back(id);
	}
	e.par_id = par_id;
}

void SQUE_ECS_DeleteEntity(const uint32_t id)
{
	SQUE_Entity& entity = SQUE_ECS_GetEntity(id);
	if (entity.id == UINT32_MAX) return;

	// Take care of children...
	// For now children vecs are not popped, so waste space in children ref vector
	const sque_vec<uint32_t>& children = children_refs[entity.children_ref];
	for (uint32_t i = 0; i < children.size(); ++i)
		SQUE_ECS_DeleteEntity(children[i]);

	// Be deleted from the parent
	if (entity.par_id != UINT32_MAX)
	{
		sque_vec<uint32_t>& par_children = children_refs[SQUE_ECS_GetEntity(entity.par_id).children_ref];
		for (uint32_t i = 0; i < par_children.size(); ++i)
			if (par_children[i] == id)
			{
				SQUE_Swap(&par_children[i], par_children.last());
				par_children.pop_back();
				break;
			}
	}

	// Pop Entity from entity vector
	entity.id = UINT32_MAX;
	entity.par_id = UINT32_MAX;
	// If the pop needs to be later, at least they ar marked with invalid ids
	SQUE_Swap((entities.end()-1), &entity);
	entities.pop_back();

	SQUE_ECS_UpdateBaseEntityList();
}

const sque_vec<SQUE_Component>& SQUE_ECS_GetComponentVec(const uint32_t comp_ref)
{
	return components_refs[comp_ref];
}

void SQUE_ECS_DeclareComponent(const uint32_t component_ref, SQUE_Component& component)
{
	components_refs[component_ref].push_back(component);
}


#include "components/components_includeall.h"
void SQUE_ECS_Component_AddType(const SQUE_Entity& entity, const uint32_t type)
{
	SQUE_Component temp = CreateFunTable[type](entity.id);
	SQUE_ECS_DeclareComponent(entity.comp_ref, temp);
	//SQUE_ECS_DeclareComponent(entity.comp_ref, const CreateFunTable[type]());
}

void SQUE_ECS_Component_CopyFromGeneric(const SQUE_Entity& entity, const uint32_t type, const SQUE_Component* copy)
{
	SQUE_Component temp = TemplateCreateFunTable[type](copy, entity.id);
	SQUE_ECS_DeclareComponent(entity.comp_ref, temp);
}

SQUE_Component* SQUE_ECS_Component_GetGenericP(const SQUE_Entity& entity, const uint32_t type)
{
	const sque_vec<SQUE_Component>& comps = SQUE_ECS_GetComponentVec(entity.comp_ref);
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == type)
			return ComponentGetFunTable[type](comps[i].id);

	return NULL;
}

SQUE_Component* SQUE_ECS_Component_AllocateCopy(const SQUE_Entity& entity, const uint32_t type, const uint32_t id)
{
	return ComponentAllocateCopyFunTable[type](id);
}

void SQUE_ECS_UpdateBaseEntityList()
{
	base_entities.clear();
	for (uint16_t i = 0; i < entities.size(); ++i)
		if (entities[i].par_id == -1) base_entities.push_back(entities[i]);
}

sque_vec<SQUE_Entity>& SQUE_ECS_GetNoParents()
{
	return base_entities;
}

sque_vec<SQUE_Entity>& SQUE_ECS_GetEntities()
{
	return entities;
}