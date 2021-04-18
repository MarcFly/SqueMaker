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
    uint32_t id = UINT32_MAX;
    uint32_t par_id = UINT32_MAX;
    sque_vec<uint32_t> child_ids;
};

class SQUE_Hier_Messager : public SQUE_Messager
{
public:
    void RegisterSubscriber() final;
    void DeclareSubjects() final;
    void SubscribeToSubjects() final;
};

class SQUE_Inspector;

class SQUE_Hierarchy : public SQUE_UI_Item
{
    sque_vec<SQUE_H_Entry*> selected;
    SQUE_RMPopupMenu rm_menu;
    SQUE_Inspector* inspector;

    SQUE_Hier_Messager msgr;

    
public:

    // Thi si sincorrect, directly using teh vectors instead of having data properly copied
    void UpdateEntries();

    void Init() final;

    void SetInspector(SQUE_Inspector* insp_p);

    // If there is an entity being inspected, it will add to that as children

    void UpdateRMMenu();

    void UpdateSelection();

    void UpdateEntry(SQUE_H_Entry& entry);

    void Update(float dt) final;

    //void CleanUp() final;

    // Drag And Drop moving
    // Rename...
    // Delete...
    //.... lots of other functionality as it comes up
};

SQUE_H_Entry& SQUE_H_GetEntry(const uint32_t id);


#endif