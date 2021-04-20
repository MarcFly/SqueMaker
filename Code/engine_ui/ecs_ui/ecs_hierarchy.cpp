#include "ecs_hierarchy.h"

#include "e_inspector.h"
#include <ecs/ecs.h>
#include "../../ecs/components/components_includeall.h"

sque_vec<SQUE_H_Entry> entries;
static SQUE_H_Entry invalid_entry;

static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;


void SQUE_Hier_Messager::RegisterSubscriber()
{
    memcpy(name, "ECS_Hierarchy", strlen("ECS_Hierarchy") + 1);
    sub_ref = SQUE_MSG_RegisterSubscriber(this);
}

void SQUE_Hier_Messager::DeclareSubjects()
{
    subjects.push_back(SQUE_MSG_DeclareSubject("NewEngineEntity"));
    // Declare Custom Subjects
}

void SQUE_Hier_Messager::SubscribeToSubjects()
{
    // Subscribe to subjects you want by name
    // SQUE_MSG_SubscribeToSubject(sub_ref, "NewUIEntity");
    SQUE_MSG_SubscribeToSubject(sub_ref, subjects[0]);
}

void SQUE_Hierarchy::UpdateEntries()
{
   //// brute force reconstruct
   //SQUE_ECS_UpdateBaseEntityList();
   //sque_vec<SQUE_Entity>& base_entities = SQUE_ECS_GetNoParents();
   //uint32_t entity_s = base_entities.size();
   //entries.clear();
   //for (uint32_t i = 0; i < entity_s; ++i)
   //{
   //    SQUE_H_Entry e;
   //    e.state = NULL;
   //    base_entries.push_back(e);
   //    entries.push_back(e);
   //}
}

void SQUE_Hierarchy::Init()
{
    active = true;
    EngineUI_RegisterItem(this);
    UpdateEntries();

    name = "ECS_Hierarchy";
    rm_menu.flags = ImGuiHoveredFlags_ChildWindows || ImGuiHoveredFlags_RootWindow;
    rm_menu.container_name = name;

    EngineUI_RegisterMessager(&msgr);
}

void SQUE_Hierarchy::SetInspector(SQUE_Inspector* insp_p) 
{ 
    inspector = insp_p; 
}

void DeleteEntity(SQUE_H_Entry* selected_entry)
{
    
    //entries[selected_entry.p]
}

SQUE_H_Entry& SQUE_H_GetEntry(const uint32_t id)
{
    for (uint32_t i = 0; i < entries.size(); ++i)
        if (entries[i].id == id)
            return entries[i];

    return invalid_entry;// This is not good, returns invalid data
}

void SQUE_H_CreateEntry(const uint32_t id, const uint32_t par_id = UINT32_MAX)
{
    SQUE_H_Entry& par_entry = SQUE_H_GetEntry(par_id);
    SQUE_H_Entry entry;
    entry.state = NULL;
    entry.id = id;
    entry.par_id = par_id;
    if (par_id != UINT32_MAX)
        par_entry.child_ids.push_back(entry.id);
    entries.push_back(entry);
}

void SQUE_H_RebaseEntry(const uint32_t id, const uint32_t par_id = UINT32_MAX)
{
    SQUE_H_Entry& e = SQUE_H_GetEntry(id);
    if(e.par_id != UINT32_MAX)
    { 
        SQUE_H_Entry& par_e = SQUE_H_GetEntry(e.par_id);
        for (uint32_t i = 0; i < par_e.child_ids.size(); ++i)
        {
            if (par_e.child_ids[i] == id)
            {
                SQUE_Swap(par_e.child_ids.last(), &par_e.child_ids[i]);
                par_e.child_ids.pop_back();
                break;
            }
        }
    }

    e.par_id = par_id;
    if (par_id != UINT32_MAX)
    {
        SQUE_H_Entry& par_e = SQUE_H_GetEntry(e.par_id);
        par_e.child_ids.push_back(e.id);
    }
}

void SQUE_H_DeleteEntry(const uint32_t id)
{
    SQUE_H_Entry& e = SQUE_H_GetEntry(id);
    if (e.id == UINT32_MAX) return;

    SQUE_H_Entry& p_e = SQUE_H_GetEntry(e.par_id);
    if (p_e.id != UINT32_MAX)
    {
        uint32_t c_id = 0;
        for (; c_id < p_e.child_ids.size(); ++c_id)
            if (e.id == p_e.child_ids[c_id]) 
                break;
        SQUE_Swap(&p_e.child_ids[c_id], p_e.child_ids.last());
        p_e.child_ids.pop_back();
    }

    while (e.child_ids.size() > 0)
        SQUE_H_DeleteEntry(e.child_ids[0]);

    uint32_t c_id = 0;
    for (; c_id < entries.size(); ++c_id)
        if (e.id == entries[c_id].id)
            break;

    SQUE_Swap(entries.last(), &entries[c_id]);
    entries.pop_back();
}

class Exec_CreateEntities : public SQUE_Executer
{
    sque_vec<uint32_t> entities;

public:
    const char* name = "Create New Entity/ies";
    uint32_t par_id = UINT32_MAX;
    uint32_t num_entities = 1;
    void Execute() {

        for (uint16_t i = 0; i < num_entities; ++i)
        {
            uint32_t id = SQUE_ECS_NewEntity(par_id);
            SQUE_Entity& entity = SQUE_ECS_GetEntity(id);
            memcpy(entity.name, "New_Entity", sizeof("New_Entity"));
            entities.push_back(entity.id);

            SQUE_H_CreateEntry(id, par_id);
        }

        SQUE_ECS_UpdateBaseEntityList();
    }

    void Undo()
    {
        for (uint32_t i = 0; i < entities.size(); ++i)
        {
            SQUE_ECS_DeleteEntity(entities[i]);
            SQUE_H_DeleteEntry(entities[i]);
        }
        // Delete From Entries?

        SQUE_ECS_UpdateBaseEntityList();
        // Function to reset hierarchy to 
    }

    void Redo()
    {
        for (uint16_t i = 0; i < num_entities; ++i)
        {
            uint32_t id = SQUE_ECS_NewEntity(par_id);

            SQUE_Entity& entity = SQUE_ECS_GetEntity(id);
            memcpy(entity.name, "New_Entity", sizeof("New_Entity"));
            entity.id = entities[i];

            SQUE_H_CreateEntry(entity.id, par_id);
        }

        SQUE_ECS_UpdateBaseEntityList();
    }

};



// Full copy?
class Exec_DeleteEntities : public SQUE_Executer
{
    // Copy of the entities
    sque_vec<uint32_t> selected_entities;
    sque_vec<sque_vec<uint32_t>> child_entities;
    sque_vec<SQUE_Entity> entities;
    sque_vec<sque_vec<SQUE_Component*>> components;

    bool FindEntity(const uint32_t id)
    {
        for (uint32_t i = 0; i < entities.size(); ++i)
            if (entities[i].id == id)
                return true;
        return false;
    }

    void CopyComponents(const SQUE_Entity& e, sque_vec<SQUE_Component*>& comp_vec)
    {
        const sque_vec<SQUE_Component>& entity_comps = SQUE_ECS_GetComponentVec(e.comp_ref);
        for (uint16_t i = 0; i < entity_comps.size(); ++i)
            comp_vec.push_back(SQUE_ECS_Component_AllocateCopy(e, entity_comps[i].type, entity_comps[i].id));
    }

    void CopyChildren(const SQUE_Entity& e, sque_vec<uint32_t>& children_vec)
    {
        sque_vec<uint32_t> children_ids = SQUE_ECS_GetChildren(e.children_ref);
        for (uint32_t i = 0; i < children_ids.size(); ++i)
        {
            const SQUE_Entity& child = SQUE_ECS_GetEntity(children_ids[i]);
            children_vec.push_back(child.id);
            if (FindEntity(child.id)) continue;

            entities.push_back(child);
            // Generate Copy of Components
            components.push_back(sque_vec<SQUE_Component*>());
            CopyComponents(child, *components.last());
            // Generate Copy of Children
            child_entities.push_back(sque_vec<uint32_t>());
            CopyChildren(child, *child_entities.last());            
        }
    }

public:
    sque_vec<uint32_t> selected_ids;
    void Execute()
    {
        for (uint32_t i = 0; i < selected_ids.size(); ++i)
        {
            const SQUE_Entity& e = SQUE_ECS_GetEntity(selected_ids[i]);
            entities.push_back(e);
            SQUE_Entity& last = *entities.last();

            // Generate Copies for the components
            components.push_back(sque_vec<SQUE_Component*>());
            CopyComponents(e, *components.last());

            // Generate Copies of their children
            child_entities.push_back(sque_vec<uint32_t>());
            CopyChildren(last, *child_entities.last());
        }

        for (uint32_t i = 0; i < selected_ids.size(); ++i)
        {
            SQUE_ECS_DeleteEntity(selected_ids[i]);
            SQUE_H_DeleteEntry(selected_ids[i]);
        }
    }

    void Undo()
    {
        for (uint32_t i = 0; i < entities.size(); ++i)
        {
            uint32_t id = SQUE_ECS_NewEntity();
            SQUE_Entity& e = SQUE_ECS_GetEntity(id);
            e.id = entities[i].id;
            e.par_id = entities[i].par_id;
            memcpy(e.name, entities[i].name, sizeof(entities[i].name));
            memcpy(e.tag_refs, entities[i].tag_refs, sizeof(entities[i].tag_refs));
            
            for (uint16_t j = 0; j < components[i].size(); ++j)
                SQUE_ECS_Component_CopyFromGeneric(e, components[i][j]->type, components[i][j]);

            SQUE_H_CreateEntry(e.id);
        }

        for (uint32_t i = 0; i < entities.size(); ++i)
        {
            SQUE_ECS_RebaseChild(entities[i].id, entities[i].par_id);
            SQUE_H_RebaseEntry(entities[i].id, entities[i].par_id);
        }
    }

    void Redo()
    {
        for (uint32_t i = 0; i < selected_ids.size(); ++i)
        {
            SQUE_ECS_DeleteEntity(selected_ids[i]);
            SQUE_H_DeleteEntry(selected_ids[i]);
        }
    }
};

void SQUE_Hierarchy::UpdateRMMenu()
{
    rm_menu.CheckToOpen();
    if (ImGui::BeginPopup(rm_menu.container_name))
    {
        if(ImGui::MenuItem("Empty Entity"))
        {
            Exec_CreateEntities* new_entity = new Exec_CreateEntities();
            new_entity->par_id = (selected.size() != 1) ? UINT32_MAX : selected[0]->id;
            new_entity->num_entities = 1;

            EngineUI_ExecuteAction(new_entity);
        }
        if (ImGui::MenuItem("New Entity"))
        {

            Exec_CreateEntities* new_entity = new Exec_CreateEntities();
            new_entity->par_id = (selected.size() != 1) ? UINT32_MAX : selected[0]->id;
            new_entity->num_entities = 1;

            EngineUI_ExecuteAction(new_entity);
            // Add Components

            
            // These can be converted into executors in each inspector most probably
            SQUE_Entity& e = SQUE_ECS_GetEntity(entries.last()->id);
            SQUE_ECS_AddComponent<SQUE_Transform>(e);
            SQUE_ECS_AddComponent<SQUE_Drawable>(e);
        }
        if(ImGui::MenuItem("Delete Selected"))
        {
            if (selected.size() > 0)
            {
                Exec_DeleteEntities* del_entity = new Exec_DeleteEntities();
                for (uint32_t i = 0; i < selected.size(); ++i)
                    del_entity->selected_ids.push_back(selected[i]->id);

                EngineUI_ExecuteAction(del_entity);

                SQUE_H_Entry cpy = SQUE_H_Entry();
                inspector->SetInspectEntity(cpy);
            }
        }
        ImGui::MenuItem("Copy Selected");
        ImGui::MenuItem("Duplicate Selected");
        ImGui::EndPopup();
    }
}

void SQUE_Hierarchy::UpdateSelection()
{

}

void SQUE_Hierarchy::UpdateEntry(SQUE_H_Entry& entry)
{
    SQUE_Entity& entity = SQUE_ECS_GetEntity(entry.id);
    sque_vec<uint32_t>& children = SQUE_ECS_GetChildren(entity.children_ref);
    bool is_selected = CHK_FLAG(entry.state, SQES_SELECTED);
        
    ImGuiTreeNodeFlags tmp_flags = node_flags 
        | ((children.size() == 0) * ImGuiTreeNodeFlags_Leaf)
        | (is_selected * ImGuiTreeNodeFlags_Selected);
        
    bool open = ImGui::TreeNodeEx(&entity.id, tmp_flags, entity.name);
        
    if (ImGui::IsItemClicked())
    {
            
        if(!(ImGui::IsKeyDown(SQUE_KEY_LEFT_CTRL) 
            || ImGui::IsKeyDown(SQUE_KEY_LEFT_SHIFT) 
            || ImGui::IsKeyDown(SQUE_KEY_RIGHT_CTRL) 
            || ImGui::IsKeyDown(SQUE_KEY_RIGHT_CTRL) ))
        {
            for (uint16_t i = 0; i < entries.size(); ++i)
                CLR_FLAG(entries[i].state, SQES_SELECTED);

            selected.clear();
            selected.push_back(&entry);
        }
        else
        {
            selected.push_back(&entry);
        }
        SET_FLAG(entry.state, SQES_SELECTED);
        if (!is_selected) 
            inspector->SetInspectEntity(entry);
    }
    if (open && children.size() > 0)
    {
        SET_FLAG(entry.state, SQES_OPENED);
        for (uint16_t i = 0; i < children.size(); ++i)
            UpdateEntry(SQUE_H_GetEntry(entry.child_ids[i]));
    }
    
    if(open) ImGui::TreePop();
    else CLR_FLAG(entry.state, SQES_OPENED);
}

void SQUE_Hierarchy::Update(float dt)
{
    for (uint16_t i = 0; i < msgr.inbox.size(); ++i)
    {
        // take care of messages
    }
    msgr.inbox.clear();
        
    if (ImGui::Begin(name, &active))
    {
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
        {
            for (uint16_t i = 0; i < entries.size(); ++i)
                CLR_FLAG(entries[i].state, SQES_SELECTED);

            selected.clear();
        }

        UpdateRMMenu();

        for (uint32_t i = 0; i < entries.size(); ++i)
        {
            if (entries[i].par_id != UINT32_MAX)
                continue;
            UpdateEntry(entries[i]);
        }

        EngineUI_RequireUpdate(active);


    }
    ImGui::End();
}