#include "./headers/msg.h"

static msg_t msgTable[MAXMESSAGES];
LIST_HEAD(msgFree_h);

/*
initialize the msgFree list to contain all
the elements of the static array of MAXMESSAGES messages.
This method will be called only once during data structure
initialization.
*/
void initMsgs() {
    // inizializzo i messaggi
    for (int i = 0; i < MAXMESSAGES; i++) {
        msg_t* m = &(msgTable[i]);

        m->m_sender = NULL;
        m->m_payload = 0;

        // inserisco il messaggio nella lista dei messaggi liberi
        freeMsg(m);
    }
}

/*
insert the element pointed to by m onto the msgFree list.
*/
void freeMsg(msg_t* m) {
    // inserisco il messaggio nella lista dei messaggi liberi
    insertMessage(&msgFree_h, m);
}

/*
return NULL if the msgFree list is
empty. Otherwise, remove an element from the msgFree list,
provide initial values for ALL of the messages fields and then
return a pointer to the removed element. Messages get
reused, so it is important that no previous value persist in a
message when it gets reallocated.
*/
msg_t* allocMsg() {
    // controllo se la lista dei messaggi liberi e' vuota
    if (list_empty(&msgFree_h)) {
        return NULL;
    }

    msg_t* m = headMessage(&msgFree_h);

    // rimuovo l'elemento dalla lista dei messaggi liberi
    list_del(msgFree_h.next);

    return m;
}

/*
used to initialize a variable to be head pointer to a message queue;
returns a pointer to the head of an empty message queue.
*/
void mkEmptyMessageQ(struct list_head* head) {
    INIT_LIST_HEAD(head);
}

/*
returns TRUE if the queue whose tail is pointed to by head is empty,
FALSE otherwise.
*/
int emptyMessageQ(struct list_head* head) {
    return list_empty(head);
}

/*
insert the message pointed to by m at the end (tail) of
the queue whose head pointer is pointed to by head.
*/
void insertMessage(struct list_head* head, msg_t* m) {
    list_add_tail(&(m->m_list), head);
}

/*
insert the message pointed to by m at the head of the
queue whose head pointer is pointed to by head.
*/
void pushMessage(struct list_head* head, msg_t* m) {
    list_add(&(m->m_list), head);
}

/*
remove the first element (starting by the head) from
the message queue accessed via head whose sender is p ptr.
If p ptr is NULL, return the first message in the queue.
Return NULL if the message queue was empty or if no
message from p ptr was found; otherwise return the pointer
to the removed message.
*/
msg_t* popMessage(struct list_head* head, pcb_t* p_ptr) {
    // controllo se la lista e' vuota
    if (list_empty(head)) {
        return NULL;
    }

    // traverso la lista e controllo se il mittente del messaggio e' p_ptr
    msg_t* m;
    list_for_each_entry(m, head, m_list) {
        if (m->m_sender == p_ptr) {
            // rimuovo il messaggio dalla lista
            list_del(&(m->m_list));

            return m;
        }
    }

    return NULL;
}

/*
return a pointer to the first message from the queue whose head is
pointed to by head. Do not remove the message from the
queue. Return NULL if the queue is empty.
*/
msg_t* headMessage(struct list_head* head) {
    if (list_empty(head)) {
        return NULL;
    }

    // prendo il primo elemento della lista con container_of
    return container_of(head->next, msg_t, m_list);
}
