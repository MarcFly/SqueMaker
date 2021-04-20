#include "engine_ui.h"
#include "menu_bar.h"

sque_vec<SQUE_UI_Item*> items;
sque_vec<SQUE_UI_Id> active_items;

#include <imgui.h>
#include <imgui_impl_squelib.h>
#include <engine_ui/IconsForkAwesome.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ITEMS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_UI_Item::Init()
{
    EngineUI_RegisterItem(this);
}

void SQUE_RMPopupMenu::CheckToOpen()
{
    rm_was_clicked = (ImGui::IsWindowHovered(flags) && ImGui::IsMouseClicked(ImGuiMouseButton_Right) || rm_was_clicked);

    if (rm_was_clicked && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::SetNextWindowPos(ImGui::GetMousePos());
        ImGui::OpenPopup(container_name);
        rm_was_clicked = false;
    }    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENGINE_UI DEFAULTS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void EngineUI_DefaultTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Text Color
    style.Colors[0] = ImVec4(.9f, .9f, .9f, 1); // Default
    style.Colors[1] = ImVec4(1, 0, 186 / 255.f, .4f); // Disabled
    // Popup Background
    style.Colors[4] = ImVec4(30 / 255.f, 40 / 255.f, 60 / 255.f, 1);
    // Border Colors
    style.Colors[5] = ImVec4(1, 1, 1, .5f); // Default
    style.Colors[6] = ImVec4(1, .6f, .9f, .5f); // Shadow
    // Frame Background
    style.Colors[7] = ImVec4(.2f, .4f, .9f, .2f); // Default
    style.Colors[8] = ImVec4(.2f, .6f, 1, .6f); // Hovered
    style.Colors[9] = ImVec4(.2f, .6f, 1, .9f); // Click
    // Title Colors
    style.Colors[10] = ImVec4(0, .4f, 1.f, .1f); // Inactive
    style.Colors[11] = ImVec4(.2f, .3f, .5f, .7f); // Active
    style.Colors[12] = style.Colors[10]; // Collapsed
    // MenuBar Background
    style.Colors[13] = ImVec4(.1f, .1f, .1f, 1);
    // ScrollBar Colors
    style.Colors[14] = ImVec4(1, 1, 1, .1f); // Background
    style.Colors[15] = ImVec4(1, 1, 1, .4f); // Default
    style.Colors[16] = ImVec4(1, 1, 1, .6f); // Hover
    style.Colors[17] = ImVec4(1, 1, 1, .9f); // Active
    // Checkbox Color
    style.Colors[18] = ImVec4(0, 1, 1, 1);
    // Slider
    style.Colors[19] = ImVec4(.8f, .6f, .9f, .8f); // Default
    style.Colors[20] = ImVec4(.8f, .6f, .9f, 1); // Grabbed
    // Button
    style.Colors[21] = style.Colors[7];
    style.Colors[22] = style.Colors[8];
    style.Colors[23] = style.Colors[9];
    // Headers
    style.Colors[24] = ImVec4(.2f, .4f, .9f, .5f); // Default
    style.Colors[25] = ImVec4(.2f, .6f, 1, .8f); // Hovered
    style.Colors[26] = ImVec4(.2f, .6f, 1, .9f); // Click
    // Separator
    style.Colors[27] = ImVec4(.55f, .55f, .75f, .7f); // Default
    style.Colors[28] = ImVec4(.55f, .55f, .75f, .9f); // Hovered
    style.Colors[29] = ImVec4(.55f, .55f, .75f, 1); // Active
    // Resize Mark
    style.Colors[30] = style.Colors[5]; // Default
    style.Colors[31] = ImVec4(1, 1, 1, .8f); // Hovered
    style.Colors[32] = ImVec4(1, 1, 1, 1); // Active
    // Tabs
    style.Colors[33] = style.Colors[7]; // Default
    style.Colors[34] = style.Colors[8]; // Hovered
    style.Colors[35] = style.Colors[9]; // Active
    style.Colors[36] = style.Colors[7];
    style.Colors[37] = ImVec4(0, 0, 0, 0);
    // TODO: Docking... 
    // Plots will go with ImPlot when implemented
    // continue...
}
void EngineUI_DefaultStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Main

    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(10, 5);
    style.ItemSpacing = ImVec2(10, 5);
    style.ItemInnerSpacing = ImVec2(5, 5);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 20;
    style.ScrollbarSize = 20;
    style.GrabMinSize = 20;

    // Border
    style.FrameBorderSize = 1;
    style.WindowBorderSize = 2;
    style.PopupBorderSize = 1.5f;
    style.TabBorderSize = 1;
    style.ChildBorderSize = 1;

    // Rounding
    style.GrabRounding = (style.FrameRounding = 6) - 2;
    style.WindowRounding = style.FrameRounding * 2;
    style.ChildRounding = 0;
    style.PopupRounding = style.FrameRounding;
    style.ScrollbarRounding = 4;
    style.TabRounding = style.FrameRounding;

    // Alignment
    style.WindowTitleAlign = ImVec2(0.02f, 0.5f);
    style.WindowMenuButtonPosition = 0;
    style.ColorButtonPosition = 0;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0, 0.5f);
    style.DisplaySafeAreaPadding = ImVec2(0, 0);
}

void EngineUI_SetFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    static const ImWchar icons_ranges[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    std::string ExecPath = SQUE_FS_GetExecPath();
    io.FontDefault = io.Fonts->AddFontFromFileTTF((ExecPath + "/Assets/fonts/Cousine-Regular.ttf").c_str(), 20);
    io.Fonts->AddFontFromFileTTF((ExecPath + "/Assets/fonts/forkawesome-webfont.ttf").c_str(), 20, &icons_config, icons_ranges);
    
    io.Fonts->Build();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENGINE_UI INIT ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <engine_ui/ecs_ui/ecs_hierarchy.h>
#include <engine_ui/menu_bar.h>
#include <engine_ui/ecs_ui/e_inspector.h>
#include <engine_ui/render_window/render_window.h>
#include <engine_ui/render_window/render_graph.h>

static ImGuiDockNodeFlags base_dockflags = ImGuiDockNodeFlags_NoWindowMenuButton;
// TODO: Convert to XMacro (enum of the types...)
static SQUE_Inspector inspector;
static SQUE_Hierarchy hier;
static SQUE_MenuBar menu_bar;
static SQUE_RenderWindow render_window;
static SQUE_RenderGraph render_graph;
static sque_vec<SQUE_Messager*> messagers;

void EngineUI_Init()
{
    //IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    EngineUI_SetFonts();

    ImGui_ImplSqueLib_Init();
    ImNodes::CreateContext();

    EngineUI_DefaultStyle();
    EngineUI_DefaultTheme();
    
    SET_FLAG(io.ConfigFlags, ImGuiConfigFlags_DockingEnable);
    io.ConfigInputTextCursorBlink = false;
    io.ConfigDragClickToInputText = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    
    hier.Init();
    inspector.Init();
    hier.SetInspector(&inspector);

    render_window.Init();
    render_graph.Init();

    // MenuBar Default starts
    menu_bar.Init();
    menu_bar.RegisterBarItem("View");
    menu_bar.RegisterMenuItem(hier.name, "View", &hier.active);
    menu_bar.RegisterMenuItem(inspector.name, "View", &inspector.active);
    menu_bar.RegisterMenuItem(render_window.name, "View", &render_window.active);
    menu_bar.RegisterMenuItem(render_graph.name, "View", &render_graph.active);
    //io.ConfigWindowsResizeFromEdges = true; // ImguiBackend Has Mouse Cursor

    for (uint16_t i = 0; i < messagers.size(); ++i)
    {
        messagers[i]->RegisterSubscriber();
        messagers[i]->DeclareSubjects();
    }
    for (uint16_t i = 0; i < messagers.size(); ++i)
        messagers[i]->SubscribeToSubjects();

}

void EngineUI_RegisterItem(SQUE_UI_Item* item)
{
    items.push_back(item);
    item->id.pos = items.size() - 1;
    if (item->active) active_items.push_back(item->id);
    
}

void EngineUI_RegisterMessager(SQUE_Messager* msgr)
{
    messagers.push_back(msgr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENGINE_UI UPDATE ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool require_update = false;

void EngineUI_UpdateActives()
{
    active_items.clear();
    uint32_t s = items.size();
    for (uint32_t i = 0; i < s; ++i)
        if (items[i]->active) active_items.push_back(items[i]->id);

    require_update = false;
}


void EngineUI_RequireUpdate(bool window_state)
{
    require_update = require_update || !window_state;
}

void EngineUI_Update(float dt)
{
    //if(SQUE_DISPLAY_)

    ImGui_ImplSqueLib_NewFrame();
    ImGui::NewFrame();
    
    menu_bar.Update(dt);

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), base_dockflags);

    for (uint16_t i = 0; i < active_items.size(); ++i)
        items[active_items[i].pos]->Update(dt);

    bool check = false;
    ImGui::ShowDemoWindow(&check);

    ImGui::Render();
    ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
    if(require_update) EngineUI_UpdateActives();
    return;
}

void EngineUI_CleanUp()
{
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
    EngineUI_CleanActions();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENGINE_UI UNDO/REDO ACTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Obviously none of this is thread safe...
sque_vec<SQUE_Executer*> redo_list;
sque_vec<SQUE_Executer*> undo_list;

void EngineUI_ExecuteAction(SQUE_Executer* action)
{
    action->Execute();
    undo_list.push_back(action);
}

void EngineUI_RedoLastAction()
{
    if (redo_list.size() == 0) return;
    SQUE_Executer* a = redo_list[redo_list.size() - 1];
    a->Redo();
    undo_list.push_back(a);
    redo_list.pop_back();
}
void EngineUI_UndoLastAction()
{
    if (undo_list.size() == 0) return;
    SQUE_Executer* a = undo_list[undo_list.size() - 1];
    a->Undo();
    redo_list.push_back(a);
    undo_list.pop_back();
}

void EngineUI_CleanActions()
{
    for (uint32_t i = 0; i < redo_list.size(); ++i)
        delete redo_list[i];
    for (uint32_t i = 0; i < undo_list.size(); ++i)
        delete undo_list[i];

    redo_list.clear();
    undo_list.clear();
}