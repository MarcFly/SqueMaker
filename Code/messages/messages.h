#ifndef _SQUE_MESSAGES_
#define _SQUE_MESSAGES_

#include <squelib.h>

typedef struct SQUE_Subject
{
    uint32_t ref;
    const char* subject;
    sque_vec<uint32_t> subscribers;
} SQUE_Subject;

typedef struct SQUE_Message
{
    uint32_t subject_id = -1;
    void* data = NULL;
    uint32_t data_size = 0;

    constexpr bool operator()(const SQUE_Message left, const SQUE_Message msg) const
    {
        return left.subject_id < msg.subject_id;
    }

    constexpr bool operator<(const SQUE_Message right) const
    {
        return subject_id < right.subject_id;
    }

} SQUE_Message;



class SQUE_Messager
{
    
    
public:
    char name[32] = "InvalidName";
    uint32_t sub_ref;
    sque_vec<uint32_t> subjects;
    SQUE_Messager(){};
    sque_vec<SQUE_Message> inbox;

    virtual void RegisterSubscriber() {};
    virtual void DeclareSubjects() {};
    virtual void SubscribeToSubjects() {};

};

// First Initialization
uint32_t SQUE_MSG_RegisterSubscriber(SQUE_Messager* messager);
uint32_t SQUE_MSG_DeclareSubject(const char* subject);
// 2nd Set Initialization
void SQUE_MSG_SubscribeToSubject(uint32_t subscriber_ref, uint32_t subject_ref);
void SQUE_MSG_SubscribeToSubjectString(uint32_t subscriber_ref, const char* subject);
// At Start of update
// DeliverMessages...
// Whenever
void SQUE_MSG_SendMessage(SQUE_Message message);


// Distribute Messages

#endif