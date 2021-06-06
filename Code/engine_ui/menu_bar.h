#ifndef _SQUE_MENU_BAR_
#define _SQUE_MENU_BAR_

#include <engine_ui/engine_ui.h>

#include <squelib.h>

struct SQUE_MenuItem
{
    bool* active_state;
    uint32_t id;
    const char* name;
    sque_vec<uint32_t> sub_items;
    const char* shortcut;
    VoidFun* cb = [](){};
    // Callback? Executer? Executer that is added by itself if needed?
    // Should be a void fun callback probably.
};


//  

class SQUE_MenuBar : public SQUE_UI_Item
{
public:
    SQUE_UI_Id id;
    sque_vec<SQUE_MenuItem> bar_items;
    sque_vec<SQUE_MenuItem> items;

    sque_vec<SQUE_MenuItem> variable_ui; // If an external wants to register items?

private:
    uint32_t GetBarItem(const char* bar_item)
    {
        for (uint16_t i = 0; i < bar_items.size(); ++i)
            if (strcmp(bar_items[i].name, bar_item) == 0)
                return i;

        return UINT32_MAX;
    }

public:

    void RegisterBarItem(const char* name)
    {
        SQUE_MenuItem p;
        p.name = name;
        bar_items.push_back(p);
    }

    

    uint32_t RegisterMenuItem(const char* name, const char* bar_item = "Unsorted Commands", bool* item_active = NULL, const char* shortcut = "", VoidFun callback = []() {}, const char* sub_item = "")
    {
        uint32_t bar = GetBarItem(bar_item);
        SQUE_MenuItem mi;
        mi.active_state = item_active;
        mi.name = name;
        mi.shortcut = shortcut;
        mi.cb = callback;
        mi.id = SQUE_RNG();
        items.push_back(mi);
        bar_items[bar].sub_items.push_back(items.size() - 1);

        return mi.id; // Return ID for later usage (maybe in future change command preferences or shortcut and such)
    };

    void Init() final
    {
        static bool b = false;
        RegisterBarItem("File");
        RegisterBarItem("Edit");
        RegisterMenuItem("Unsorted Commands", "File");
        RegisterMenuItem("Undo", "Edit", NULL, "", 
            []() {
                EngineUI_UndoLastAction();
            });
        RegisterMenuItem("Redo", "Edit", NULL, "", 
            []() {
                EngineUI_RedoLastAction();
            });
    }

    void UpdateMenuItem(SQUE_MenuItem& item)
    {
        if (item.sub_items.size() == 0)
        {
            bool has_bool = !(item.active_state == NULL);
            bool prev = (has_bool) ? *item.active_state : false;
            if (ImGui::MenuItem(item.name, "", item.active_state))
            {
                item.cb();
                if (has_bool)EngineUI_RequireUpdate(!(*item.active_state != prev));
            }
            
        }
        else if (ImGui::BeginMenu(item.name))
        {
            for (uint16_t i = 0; i < item.sub_items.size(); ++i)
                UpdateMenuItem(items[item.sub_items[i]]);

            ImGui::EndMenu();
        }
        
    }

    void Update(float dt) final
    {
        ImGui::BeginMainMenuBar();

        for (uint16_t i = 0; i < bar_items.size(); ++i)
        {
            if (ImGui::BeginMenu(bar_items[i].name))
            {
                for (uint32_t j = 0; j < bar_items[i].sub_items.size(); ++j)
                    UpdateMenuItem(items[bar_items[i].sub_items[j]]);
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

    void CleanUp() final {}

    
};

#endif