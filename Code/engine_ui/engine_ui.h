#ifndef _SQUE_MAKER_GUI_
#define _SQUE_MAKER_GUI_

#include <imgui.h>
#include <imgui_internal.h>
#include "IconsForkAwesome.h"
#include <squelib.h>
#include "../messages/messages.h"

class SQUE_Executer
{
public:
    const char* name = "Default Action Name";
    virtual void Execute() {};
    virtual void Redo() {};
    virtual void Undo() {};
};

struct SQUE_UI_Id
{
    uint32_t id;
    uint16_t pos;
};

class SQUE_ItemDraggable
{
    uint32_t id;
    uint32_t type;
public:
    static bool CheckStartDrag();
    virtual void SetUp() {};
    virtual void Receive(SQUE_ItemDraggable* item) {};
    virtual void CleanUp() {};
};

class SQUE_RMPopupMenu
{
    bool rm_was_clicked;
public:    
    ImGuiHoveredFlags flags = ImGuiHoveredFlags_None;
    const char* container_name;

    void CheckToOpen();
};

class SQUE_UI_Item
{
public:
    SQUE_UI_Id id;
    const char* name;
    uint16_t style_ind;
    bool active;

    virtual void Init();
    virtual void Update(float dt) {};
    virtual void CleanUp() {};
};

// OOP vs DoD on windows
// Each window is a separate entity entirely
// Each window will do a specific job and have completely different data that it will use to accomplish its purpose
// EX: Scene Tree will have a copy of scene and structure it in a way that is presentable
// EX2: Inspector will have an object and will display it's data
// EX3: Inspector parts will have access to the readable info and allow modification...
// After all, window function make no sense DoD as they have nothing in common besides requiring data to Read and Write Functions but different to each case
//class SQUE_MenuBar : public SQUE_UI_Item
//{
//    SQUE_UI_Id id;
//    sque_vec<SQUE_UI_Item*> menu_items;
//};


void EngineUI_Init();
void EngineUI_DefaultTheme();
void EngineUI_RegisterItem(SQUE_UI_Item* item);
void EngineUI_RegisterMessager(SQUE_Messager* msgr);

void EngineUI_RequireUpdate(bool window_state);
void EngineUI_StartDraggable(SQUE_ItemDraggable* item);
SQUE_ItemDraggable* EngineUI_CheckDroppedDraggable();
void EngineUI_Update(float dt);

void EngineUI_ExecuteAction(SQUE_Executer* action);
void EngineUI_RedoLastAction();
void EngineUI_UndoLastAction();

void EngineUI_CleanActions();

void EngineUI_CleanUp();

#endif