#ifndef _SQUE_MENU_BAR_
#define _SQUE_MENU_BAR_

#include <engine_ui/engine_ui.h>

#include <squelib.h>

struct SQUE_SubMenu
{
    const char* name;
    sque_vec<uint32_t> items;
    sque_vec<uint32_t> sub_menus;
};

struct SQUE_MenuItem
{
    bool* active_state;
    const char* name;
    sque_vec<uint32_t> sub_items;
};



class SQUE_MenuBar : public SQUE_UI_Item
{
public:
    SQUE_UI_Id id;
    sque_vec<SQUE_MenuItem> items;
    sque_vec<uint32_t> base_items;
    sque_vec<SQUE_MenuItem> variable_ui;

    void Init() final
    {
    }

    void UpdateMenuItem(SQUE_MenuItem& item)
    {
        if (item.sub_items.size() == 0)
        {
            bool prev = *item.active_state;
            ImGui::MenuItem(item.name, "", item.active_state);
            EngineUI_RequireUpdate(!(*item.active_state != prev));
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

        // Should a main menu register and be a class/struct by itself or should it be standalone
        if (ImGui::BeginMenu("Menu1"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Menu2"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("UI WIndows"))
        {
            for (uint16_t i = 0; i < base_items.size(); ++i)
            {
                UpdateMenuItem(items[base_items[i]]);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    void CleanUp() final {}

    uint32_t RegisterMenuItem(bool* item_active, const char* name, uint32_t sub_item = -1)
    {
        SQUE_MenuItem mi;
        mi.active_state = item_active;
        mi.name = name;
        items.push_back(mi);
        uint32_t ret = items.size() - 1;
        if (sub_item == -1) base_items.push_back(ret);
        else items[sub_item].sub_items.push_back(ret);
        return ret;
    };
};

#endif