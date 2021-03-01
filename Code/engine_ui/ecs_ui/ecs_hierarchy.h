#ifndef _ECS_HIERARCHY_UI_
#define _ECS_HIERARCHY_UI_
#include "../engine_ui.h"


enum ENTRY_STATE
{
    SQES_CHANGED = BITSET1,
    SQES_SELECTED = BITSET2,
    SQES_OPENED = BITSET3,
    //...
};

struct SQUE_H_Entry
{
    uint32_t state = NULL;
    uint32_t ref = -1;
    sque_vec<uint32_t> child_refs;
    bool child_added = false;
};

#include "./e_inspector.h"
#include <ecs/ecs.h>

sque_vec<SQUE_H_Entry> base_entries;
sque_vec<SQUE_H_Entry> entries;
static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

#include <imgui_internal.h>
#include "../../ecs/components/transform.h"

// When opening an entity, it is important to keep 

class SQUE_Hier_Messager : public SQUE_Messager
{
public:
    void RegisterSubscriber() final
    {
        memcpy(name, "ECS_Hierarchy", strlen("ECS_Hierarchy") + 1);
        sub_ref = SQUE_MSG_RegisterSubscriber(this);
    }

    void DeclareSubjects() final
    {
        subjects.push_back(SQUE_MSG_DeclareSubject("NewEngineEntity"));
        // Declare Custom Subjects
    }

    void SubscribeToSubjects() final
    {
        // Subscribe to subjects you want by name
        // SQUE_MSG_SubscribeToSubject(sub_ref, "NewUIEntity");
        SQUE_MSG_SubscribeToSubject(sub_ref, subjects[0]);
    }
};

class SQUE_Hierarchy : public SQUE_UI_Item
{
    sque_vec<SQUE_H_Entry*> selected;
    SQUE_RMPopupMenu rm_menu;
    SQUE_Inspector* inspector;

    SQUE_Hier_Messager msgr;

public:

    // Thi si sincorrect, directly using teh vectors instead of having data properly copied
    void UpdateEntries()
    {
        // brute force reconstruct
        base_entries.clear();
        SQUE_ECS_UpdateBaseEntityList();
        sque_vec<SQUE_Entity>& base_entities = SQUE_ECS_GetNoParents();
        uint32_t entity_s = base_entities.size();
        entries.clear();
        for (uint32_t i = 0; i < entity_s; ++i)
        {
            SQUE_H_Entry e;
            e.state = NULL;
            e.ref = i;
            base_entries.push_back(e);
            entries.push_back(e);
        }
    }

    void Init() final 
    {
        active = true;
        EngineUI_RegisterItem(this);
        UpdateEntries();

        name = "ECS_Hierarchy";
        rm_menu.flags = ImGuiHoveredFlags_ChildWindows || ImGuiHoveredFlags_RootWindow;
        rm_menu.container_name = name;

        EngineUI_RegisterMessager(&msgr);
    }

    void SetInspector(SQUE_Inspector* insp_p) { inspector = insp_p; }

    // If there is an entity being inspected, it will add to that as children

    void UpdateRMMenu()
    {
        rm_menu.CheckToOpen();
        if (ImGui::BeginPopup(rm_menu.container_name))
        {
            if(ImGui::MenuItem("Empty Entity"))
            {
                SQUE_H_Entry entry;
                entry.state = NULL;

                if (selected.size() != 1)
                {
                    entry.ref = SQUE_ECS_NewEntity();
                    base_entries.push_back(entry);
                }
                else
                {
                    entry.ref = SQUE_ECS_NewChildEntity(SQUE_ECS_GetEntityRef(selected[0]->ref).id);
                    selected[0]->child_refs.push_back(entries.size());

                }
                entries.push_back(entry);

                SQUE_Entity& entity = SQUE_ECS_GetEntityRef(entry.ref);
                memcpy(entity.name, "Empty_Entity", sizeof("Empty_Entity"));
            }
            if (ImGui::MenuItem("New Entity"))
            {
                SQUE_H_Entry entry;
                entry.state = NULL;
                
                // Declare Base Components non-empty
                SQUE_Component transform;

                // Parenting?
                if (selected.size() != 1)
                {
                    entry.ref = SQUE_ECS_NewEntity();
                    base_entries.push_back(entry);

                    transform = SQUE_ECS_AddTransform();
                }
                else
                {
                    entry.ref = SQUE_ECS_NewChildEntity(SQUE_ECS_GetEntityRef(selected[0]->ref).id);
                    selected[0]->child_refs.push_back(entries.size());

                }

                // Add Components to ECS
                SQUE_ECS_AddComponent(entry.ref, transform);

                // Push entry
                entries.push_back(entry);
                
                // Change Entity name raw
                SQUE_Entity& entity = SQUE_ECS_GetEntityRef(entry.ref);
                memcpy(entity.name, "New_Entity", sizeof("New_Entity"));
            }
            ImGui::MenuItem("Delete Selected");
            ImGui::MenuItem("Copy Selected");
            ImGui::MenuItem("Duplicate Selected");
            ImGui::EndPopup();
        }
    }

    void UpdateSelection()
    {

    }

    void UpdateEntry(SQUE_H_Entry& entry)
    {
        SQUE_Entity& entity = SQUE_ECS_GetEntityRef(entry.ref);
        sque_vec<uint32_t> children = SQUE_ECS_GetChildren(entity.children_ref);
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
                for (uint16_t i = 0; i < base_entries.size(); ++i)
                    CLR_FLAG(base_entries[i].state, SQES_SELECTED);

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
        if (open)
        {
            if (!entry.child_added)
            {
                for (uint16_t i = 0; i < children.size(); ++i)
                {
                    SQUE_H_Entry child;
                    child.ref = children[i];
                    entries.push_back(child);
                    entry.child_refs.push_back(entries.size() - 1);
                }
            }
            SET_FLAG(entry.state, SQES_OPENED);
            for (uint16_t i = 0; i < children.size(); ++i)
                UpdateEntry(entries[entry.child_refs[i]]);
            
            ImGui::TreePop();
        }
        else CLR_FLAG(entry.state, SQES_OPENED);
    }

    void Update(float dt) final
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
                for (uint16_t i = 0; i < base_entries.size(); ++i)
                    CLR_FLAG(base_entries[i].state, SQES_SELECTED);

                selected.clear();
            }

            UpdateRMMenu();

            for (uint16_t i = 0; i < base_entries.size(); ++i)
                UpdateEntry(base_entries[i]);

            EngineUI_RequireUpdate(active);

        } ImGui::End();        
    };
    //void CleanUp() final;

    // Drag And Drop moving
    // Rename...
    // Delete...
    //.... lots of other functionality as it comes up
};


#endif