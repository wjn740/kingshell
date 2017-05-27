#ifndef __LINK_H
#define __LINK_H
#define LINKADD(head,node) \
    do {\
    (node)->next = (head)->next;\
    (head)->next = (node);\
    } while(0)


#endif
