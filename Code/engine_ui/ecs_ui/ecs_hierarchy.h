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
    uint32_t state;
    uint32_t ref;
};

#include "./e_inspector.h"
#include <ecs/ecs.h>
extern sque_vec<SQUE_Entity> entities;
extern sque_vec<SQUE_Entity> base_entities;
extern sque_vec<sque_vec<uint32_t>> children_refs;
extern sque_vec<sque_vec<SQUE_Component>> components_refs;
extern sque_vec<char[32]> tags;

sque_vec<SQUE_H_Entry> base_entries;
sque_vec<SQUE_H_Entry> entries;
static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

#include <imgui_internal.h>
#include "../../ecs/components/transform.h"

// When opening an entity, it is important to keep 

class SQUE_Hierarchy : public SQUE_UI_Item
{
    sque_vec<SQUE_H_Entry*> selected;
    SQUE_RMPopupMenu rm_menu;
    SQUE_Inspector* inspector;

public:

    // Thi si sincorrect, directly using teh vectors instead of having data properly copied
    void UpdateEntries()
    {
        // brute force reconstruct
        base_entries.clear();
        uint32_t entity_s = base_entities.size();
        for (int i = 0; i < entity_s; ++i)
        {
            SQUE_H_Entry e;
            e.state = NULL;
            e.ref = i;
            base_entries.push_back(e);
        }

        entries.clear();
        entity_s = entities.size();
        for (int i = 0; i < entity_s; ++i)
        {
            SQUE_H_Entry e; 
            e.state = NULL;
            e.ref = i;
            entries.push_back(e);
        }
    }

    void Init() final 
    {
        active = true;
        EngineUI_RegisterItem(this);
        UpdateEntries();

        name = "ECS_Hierarchy";
        rm_menu.flags = ImGuiHoveredFlags_ChildWindows || ImGuiHoveredFlags_RootWindow;;
        rm_menu.container_name = name;
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
                SQUE_Entity entity;
                memcpy(entity.name, "Empty_Entity", sizeof("Empty_Entity"));
                entity.id = SQUE_RNG();
                entity.children_ref = children_refs.try_insert(sque_vec<uint32_t>());
                entity.comp_ref = components_refs.try_insert(sque_vec<SQUE_Component>());
                
                entry.ref = entities.try_insert(entity);
                entry.state = NULL;
                entries.push_back(entry);
                if (selected.size() != 1)
                {
                    base_entities.try_insert(entity);
                    base_entries.push_back(entry);
                }
            }
            if (ImGui::MenuItem("New Entity"))
            {
                SQUE_H_Entry entry;
                SQUE_Entity entity;
                memcpy(entity.name, "New_Entity", sizeof("New_Entity"));
                entity.id = SQUE_RNG();
                entity.children_ref = children_refs.try_insert(sque_vec<uint32_t>());
                entity.comp_ref = components_refs.try_insert(sque_vec<SQUE_Component>());

                entry.ref = entities.try_insert(entity);
                entry.state = NULL;
                entries.push_back(entry);
                if (selected.size() != 1)
                {
                    base_entities.try_insert(entity);
                    base_entries.push_back(entry);
                }

                sque_vec<SQUE_Component>& c_ref = components_refs[entity.comp_ref];
                SQUE_Component transform;
                transform.type = SQUE_ECS_TRANSFORM;
                transform.ref = SQUE_ECS_AddTransform(); // Take care of selected object as parent
                c_ref.try_insert(transform);

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
        SQUE_Entity& entity = base_entities[entry.ref];
        sque_vec<uint32_t>& children = children_refs[entity.children_ref];
        bool selected = CHK_FLAG(entry.state, SQES_SELECTED);
        ImGuiTreeNodeFlags tmp_flags = node_flags 
            | ((children.size() == 0) * ImGuiTreeNodeFlags_Leaf)
            | (selected * ImGuiTreeNodeFlags_Selected);
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
            }
            SET_FLAG(entry.state, SQES_SELECTED);
            if (!selected) inspector->SetInspectEntity(entry);
        }
        if (open)
        {
            SET_FLAG(entry.state, SQES_OPENED);
            for (uint16_t i = 0; i < children.size(); ++i)
                UpdateEntry(entries[children[i]]);
            ImGui::TreePop();
        }
        else CLR_FLAG(entry.state, SQES_OPENED);
    }

    void Update(float dt) final
    {
        if (base_entries.populated() != base_entities.populated()) 
            UpdateEntries(); // naive update, should be more of a message system
        
        if (ImGui::Begin(name, &active))
        {
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