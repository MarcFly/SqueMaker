#include <ecs/ecs.h>

sque_vec<SQUE_Entity> entities;
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

SQUE_Entity& SQUE_ECS_GetEntityID(const uint32_t id)
{
	//for (uint16_t i = 0; i < cached_entity_search.size(); ++i)
		//if (id == cached_entity_search[i].id) return cached_entity_search[i];

	for (uint16_t i = 0; i < entities.size(); ++i)
		if (id == entities[i].id)
			{
				cached_entity_search.push_back(entities[i]);
				return entities[i];
			}
	return entities[-1];
}

SQUE_Entity& SQUE_ECS_GetEntityRef(const uint32_t ref)
{
	return entities[ref];
}

uint32_t SQUE_ECS_GetNumChildren(const uint32_t child_ref)
{
	return children_refs[child_ref].size();
}

sque_vec<uint32_t>& SQUE_ECS_GetChildren(const uint32_t child_ref)
{
	return children_refs[child_ref];
}

uint32_t SQUE_ECS_NewEntity()
{
	uint32_t ind = entities.try_insert(SQUE_Entity());
	SQUE_Entity& entity = entities[ind];
	base_entities.push_back(entity);

	entity.comp_ref = components_refs.try_insert(sque_vec<SQUE_Component>());
	entity.children_ref = children_refs.try_insert(sque_vec<uint32_t>());
	entity.id = SQUE_RNG();
	entity.par_id = -1;

	return ind;
}

uint32_t SQUE_ECS_NewChildEntity(const uint32_t par_id)
{
	uint32_t ind = entities.try_insert(SQUE_Entity());
	SQUE_Entity& entity = entities[ind];
	children_refs[SQUE_ECS_GetEntityID(par_id).children_ref].push_back(ind);

	entity.comp_ref = components_refs.try_insert(sque_vec<SQUE_Component>());
	entity.children_ref = children_refs.try_insert(sque_vec<uint32_t>());
	entity.id = SQUE_RNG();
	entity.par_id = par_id;

	return ind;
}

void SQUE_ECS_DeleteEntity(SQUE_Entity& entity)
{
	entity.id = UINT32_MAX;
	entity.par_id = UINT32_MAX;
	SQUE_ECS_UpdateBaseEntityList();





	// Pop Entity from entity vector
	SQUE_Swap((entities.end()-1), &entity);
	entities.pop_back();
}

void SQUE_ECS_DeleteEntityRef(const uint32_t ref)
{
	SQUE_ECS_DeleteEntity(SQUE_ECS_GetEntityRef(ref));
	entities[ref].id = UINT32_MAX;
	SQUE_Swap(&entities[entities.size() - 1], &entities[ref]);

	entities.pop_back();
}

void SQUE_ECS_DeleteEntityID(const uint32_t id)
{
	SQUE_Entity& e = SQUE_ECS_GetEntityID(id);
	e.id = UINT32_MAX;
	SQUE_Swap(&entities[entities.size() - 1], &e);
	entities.pop_back();
}


sque_vec<SQUE_Component>& GetComponentVec(const uint32_t comp_ref)
{
	return components_refs[comp_ref];
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