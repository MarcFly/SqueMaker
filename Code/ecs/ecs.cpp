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
	for (uint16_t i = 0; i < cached_entity_search.size(); ++i)
		if (id == cached_entity_search[i].id) return cached_entity_search[i];

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

void SQUE_ECS_DeleteEntity(const uint32_t par_id)
{

}

uint32_t SQUE_ECS_GetComponentRef(const uint32_t entity_ref, const uint32_t component_type)
{
	uint32_t ret = SQUE_ECS_UNKNOWN;
	sque_vec<SQUE_Component>& comps = components_refs[entities[entity_ref].comp_ref];
	for (uint16_t i = 0; i < comps.size(); ++i)
		if (comps[i].type == component_type)
			return comps[i].ref;
	return ret;
}

void SQUE_ECS_AddComponent(const uint32_t entity_ref, SQUE_Component component)
{
	components_refs[entities[entity_ref].comp_ref].push_back(component);
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