#include "./messages.h"

#include <set>

sque_vec<SQUE_Subscriber> subscribers;
sque_vec<SQUE_Subject> subjects;
sque_vec<sque_vec<SQUE_Message>> messages;
std::set<SQUE_Message> unique_messages;

uint32_t SQUE_MSG_RegisterSubscriber(const char* name, sque_vec<SQUE_Message>* inbox)
{
    SQUE_Subscriber sub;
    sub.inbox = inbox;
    memcpy(sub.name, name, strlen(name)+1);
    subscribers.push_back(sub);
    return subscribers.size() - 1;
}

uint32_t SQUE_MSG_DeclareSubject(const char* subject)
{
    SQUE_Subject subj;
    subj.subject = subject;
    messages.push_back(sque_vec<SQUE_Message>());
    subj.ref = messages.size()-1;
    return subj.ref;
}

void SQUE_MSG_SubscribeToSubject(uint32_t subscriber_ref, uint32_t subject_ref)
{
    subscribers[subscriber_ref].subjects.push_back(subject_ref);
    subjects[subject_ref].subscribers.push_back(subscriber_ref);
}

void SQUE_MSG_SendMessage(SQUE_Message message)
{
    messages[message.subject_id].push_back(message);
}

void SQUE_MSG_SendUniqueMessage(SQUE_Message message)
{
    unique_messages.insert(message);
}