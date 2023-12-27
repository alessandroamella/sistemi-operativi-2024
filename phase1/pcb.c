#include "./headers/pcb.h"
#include "../klog.c"  // DEBUG

static pcb_t pcbTable[MAXPROC];
LIST_HEAD(pcbFree_h);
static int next_pid = 1;

/*
initialize the pcbFree list to contain all
the elements of the static array of MAXPROC PCBs. This
method will be called only once during data structure
initialization.
*/
void initPcbs() {
    // INIT_LIST_HEAD(&pcbFree_h);

    // inizializzo i pcb
    for (int i = 0; i < MAXPROC; i++) {
        pcb_PTR pcb = &(pcbTable[i]);

        pcb->p_parent = NULL;
        pcb->p_supportStruct = NULL;
        pcb->p_pid = next_pid++;  // incremento il pid per il prossimo pcb
        pcb->p_time = 0;
        mkEmptyProcQ(&(pcb->p_child));
        mkEmptyProcQ(&(pcb->p_sib));
        mkEmptyProcQ(&(pcb->msg_inbox));

        // inserisco il pcb nella lista dei pcb liberi
        freePcb(pcb);
    }
}

/*
Insert the element pointed to by p onto the pcbFree list.
*/
void freePcb(pcb_t* p) {
    // inserisco il pcb nella lista dei pcb liberi
    list_add_tail(&(p->p_list), &pcbFree_h);
}

/*
Return NULL if the pcbFree list is empty. Otherwise, remove an element from the pcbFree
list, provide initial values for ALL of the PCBs fields (i.e. NULL and/or 0) and then return
a pointer to the removed element. PCBs get reused, so it is important that no previous
value persist in a PCB when it gets reallocated.
*/
pcb_t* allocPcb() {
    // controllo se la lista dei pcb liberi e' vuota
    if (list_empty(&pcbFree_h)) {
        return NULL;
    }

    pcb_PTR pcb = headProcQ(&pcbFree_h);

    // rimuovo l'elemento dalla lista dei pcb liberi
    list_del(pcbFree_h.next);

    // inizializzo i campi del pcb
    pcb->p_parent = NULL;
    pcb->p_supportStruct = NULL;
    pcb->p_pid = next_pid++;
    pcb->p_time = 0;
    mkEmptyProcQ(&(pcb->p_child));
    mkEmptyProcQ(&(pcb->p_sib));
    mkEmptyProcQ(&(pcb->msg_inbox));

    return pcb;
}

/*
this method is used to initialize a variable to be head pointer to a
process queue.
*/
void mkEmptyProcQ(struct list_head* head) {
    // inizializzo la lista
    INIT_LIST_HEAD(head);
}

/*
return TRUE if the queue whose head is pointed to by head is empty.
Return FALSE otherwise.
*/
int emptyProcQ(struct list_head* head) {
    // controllo se la lista e' vuota
    return list_empty(head);
}

/*
insert the PCB pointed by p into the process queue
whose head pointer is pointed to by head.
*/
void insertProcQ(struct list_head* head, pcb_t* p) {
    // inserisco il pcb nella lista
    list_add_tail(&(p->p_list), head);
}

/*
return a pointer to the first PCB from the process queue whose head is
pointed to by head. Do not remove this PCB from the
process queue. Return NULL if the process queue is empty.
*/
pcb_t* headProcQ(struct list_head* head) {
    if (list_empty(head)) {
        return NULL;
    }

    // prendo il primo elemento della lista con container_of
    pcb_PTR pcb = container_of(head->next, pcb_t, p_list);
    return pcb;
}

/*
remove the first (i.e. head) element from the process queue whose
head pointer is pointed to by head. Return NULL if the
process queue was initially empty; otherwise return the pointer
to the removed element.
*/
pcb_t* removeProcQ(struct list_head* head) {
    pcb_PTR pcb = headProcQ(head);
    if (pcb == NULL) {
        return NULL;
    }

    list_del(head->next);
    return pcb;
}

/*
remove the PCB pointed to by p from the process queue
whose head pointer is pointed to by head. If the desired entry
is not in the indicated queue (an error condition), return
NULL; otherwise, return p. Note that p can point to any
element of the process queue.
*/
pcb_t* outProcQ(struct list_head* head, pcb_t* p) {
    if (list_empty(head) || p == NULL) {
        return NULL;
    }

    // traverse the list and remove the pcb if found
    struct list_head* pos;
    list_for_each(pos, head) {
        pcb_PTR pcb = container_of(pos, pcb_t, p_list);
        if (pcb == p) {
            list_del(pos);
            return pcb;
        }
    }

    return NULL;
}

/*
return TRUE if the PCB
pointed to by p has no children. Return FALSE otherwise.
*/
int emptyChild(pcb_t* p) {
    return list_empty(&(p->p_child));
}

/*
make the PCB pointed to by p a child of the PCB pointed to by prnt.
*/
void insertChild(pcb_t* prnt, pcb_t* p) {
    // inserisco il pcb nella lista dei figli
    list_add_tail(&(p->p_sib), &(prnt->p_child));
    p->p_parent = prnt;
}

/*
make the first child of the
PCB pointed to by p no longer a child of p. Return NULL if
initially there were no children of p. Otherwise, return a
pointer to this removed first child PCB.
*/
pcb_t* removeChild(pcb_t* p) {
    // controllo se la lista dei figli e' vuota
    if (list_empty(&(p->p_child))) {
        return NULL;
    }

    pcb_PTR pcb = headProcQ(&(p->p_child));
    list_del(p->p_child.next);

    return pcb;
}

/*
make the PCB pointed to by
p no longer the child of its parent. If the PCB pointed to by p
has no parent, return NULL; otherwise, return p. Note that
the element pointed to by p could be in an arbitrary position
(i.e. not be the first child of its parent).
*/
pcb_t* outChild(pcb_t* p) {
    if (p->p_parent == NULL) {
        return NULL;
    }

    list_del(&(p->p_sib));
    p->p_parent = NULL;

    return p;
}
