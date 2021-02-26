#ifndef _ECS_HIERARCHY_UI_
#define _ECS_HIERARCHY_UI_
#include "../engine_ui.h"

enum ENTRY_STATE
{
    SQES_CHANGED = BITSET1,
    SQES_SELECTED = BITSET2,
    //...
};

struct SQUE_H_Entry
{
    uint32_t state;
    uint32_t entity_id;
    char name[32];
    uint32_t ref;
};
#include <ecs/ecs.h>
extern sque_vec<SQUE_Entity> entities;
extern sque_vec<SQUE_Entity> base_entities;
extern sque_vec<sque_vec<uint32_t>> children_refs;
extern sque_vec<sque_vec<SQUE_Component>> components_refs;
extern sque_vec<char[32]> tags;

sque_vec<SQUE_H_Entry> entity_entries;
static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

#include <imgui_internal.h>

class SQUE_Hierarchy : public SQUE_UI_Item
{
    sque_vec<SQUE_H_Entry*> selected;
    SQUE_RMPopupMenu rm_menu;

public:

    // Thi si sincorrect, directly using teh vectors instead of having data properly copied
    void UpdateEntries()
    {
        // brute force reconstruct
        entity_entries.clear();
        uint32_t entity_s = base_entities.size();
        for (int i = 0; i < entity_s; ++i)
        {
            SQUE_H_Entry e;
            e.entity_id = entities[i].id;
            e.state = NULL;
            e.ref = i;
            entity_entries.push_back(e);
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
    
    void UpdateRMMenu()
    {
        rm_menu.CheckToOpen();
        if (ImGui::BeginPopup(rm_menu.container_name))
        {
            if (ImGui::MenuItem("New Entity"))
            {
                SQUE_Entity test;
                memcpy(test.name, "Fake Entity", sizeof("Fake Entity"));
                test.id = SQUE_RNG();
                
                children_refs.push_back(sque_vec<uint32_t>());
                test.children_ref = children_refs.size() - 1;

                base_entities.push_back(test);
            }
            ImGui::MenuItem("Delete Selected");
            ImGui::MenuItem("Copy Selected");
            ImGui::MenuItem("Duplicate Selected");
            ImGui::EndPopup();
        }
    }

    void Update(float dt) final
    {
        if (entity_entries.size() != entities.size()) UpdateEntries(); // naive update, should be more of a message system
        
        if (ImGui::Begin(name, &active))
        {
            UpdateRMMenu();
            std::string name_id;
            for (int i = 0; i < base_entities.size(); ++i)
            {
                name_id = std::to_string(base_entities[i].id); // this is super bad
                ImGuiTreeNodeFlags tmp_flags = node_flags | ((children_refs[base_entities[i].children_ref].size() == 0) * ImGuiTreeNodeFlags_Leaf);
                if (ImGui::TreeNodeEx(name_id.c_str(), tmp_flags, "%s", base_entities[i].name))
                {
                    ImGui::TreePop();
                }
            }
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