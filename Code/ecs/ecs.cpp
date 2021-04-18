#include <ecs/ecs.h>

sque_vec<SQUE_Entity> entities;
SQUE_Entity invalid;
sque_vec<SQUE_Entity> base_entities;
sque_vec<sque_vec<uint32_t>> children_refs;
sque_vec<sque_vec<SQUE_Component>> components_refs;
sque_vec<char[32]> tags;

sque_vec<SQUE_Entity> cached_entity_search;

void SQUE_ECS_EarlyDestruct()
{
	entities.~sque_vec();
	base_entities.~sque_vec();
	children_refs.~sque_vec();

	for (int i = 0; i < components_refs.size(); ++i)
		components_refs[i].~sque_vec();

	components_refs.~sque_vec();
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
	return invalid;
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
	uint32_t ref = entities.try_insert(SQUE_Entity());
	SQUE_Entity& entity = entities[ref];

	entity.comp_ref = components_refs.try_insert(sque_vec<SQUE_Component>());
	entity.children_ref = children_refs.try_insert(sque_vec<uint32_t>());
	entity.id = SQUE_RNG();
	entity.par_id = par_id;

	if (par_id != UINT32_MAX)
		children_refs[SQUE_ECS_GetEntity(par_id).children_ref].push_back(entity.id);
	else
		base_entities.push_back(entity);

	return entity.id;
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
void SQUE_ECS_AddComponentIllegal(const SQUE_Entity& entity, const uint32_t type)
{
	SQUE_ECS_DeclareComponent(entity.comp_ref, CreateFunTable[type]());
}

void SQUE_ECS_CopyComponentIllegal(const SQUE_Entity& entity, const uint32_t type, const SQUE_Component_Template* copy)
{
	SQUE_ECS_DeclareComponent(entity.comp_ref, TemplateCreateFunTable[type](copy));
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