#include <ecs/ecs.h>

sque_vec<SQUE_Entity> entities;
sque_vec<SQUE_Entity> base_entities;
sque_vec<sque_vec<uint32_t>> children_refs;
sque_vec<sque_vec<SQUE_Component>> components_refs;
sque_vec<char[32]> tags;