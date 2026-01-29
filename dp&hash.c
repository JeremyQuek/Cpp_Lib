

#include <stdbool.h>
#include <stdint.h>


typedef struct AddressPair {
    uintptr_t addr1;
    uintptr_t addr2;
} AddressPair;

typedef struct _dbllistnode
{
    int  item;
	struct _dbllistnode *next;
	struct _dbllistnode *pre;
} CDblListNode;

typedef struct _dbllinkedlist{
   int size;
   CDblListNode *head;
} CDblLinkedList;


int min(int a, int b);
int addAddressPair(AddressPair array[100], int* sizePtr, void* addr1, void* addr2);

int numMountainPairs(CDblLinkedList CDLL)
{
    CDblListNode *cur=CDLL.head, *curpair, *temp;
    AddressPair pseudo_hashset[100];
    int pairs=0;
    int size = 0;

    if (CDLL.size <=1) {
        return 0;
    }
    else if (CDLL.size == 2) {
        return 1;
    }
    else {
        pairs = CDLL.size;
    }

    
    for (int i=0; i< CDLL.size; i++) {
        curpair=cur->next->next;
        while (curpair!=cur->pre) {
            bool visible = true;
            int h_min=min(curpair->item, cur->item);
            temp=cur->next;
            while (temp!=curpair) {
                if (temp->item > h_min) {
                    visible = false;
                    break;
                }
                temp = temp->next;
            }
            
            if (visible==false) {
                break;
            }
            else if (visible ==true) {
                size = addAddressPair(pseudo_hashset, &size, cur, curpair);
            }
            curpair=curpair->next;
        }

        curpair=cur->pre->pre;
        while (curpair!=cur->next) {
            bool visible = true;
            int h_min=min(curpair->item, cur->item);
            temp=cur->pre;
            while (temp!=curpair) {
                if (temp->item > h_min) {
                    visible = false;
                    break;
                }
                temp = temp->pre;
            }
            
            if (visible==false) {
                break;
            }
            else if (visible ==true) {
                size = addAddressPair(pseudo_hashset, &size, cur, curpair);
            }
            curpair=curpair->pre;
        }

        cur=cur->next;
    }
    return pairs + size;
}



int addAddressPair(AddressPair array[100], int* sizePtr, void* addr1, void* addr2) {
    uintptr_t temp1 = (uintptr_t)addr1;
    uintptr_t temp2 = (uintptr_t)addr2;
    if (temp1 > temp2) {
        uintptr_t temp = temp1;
        temp1 = temp2;
        temp2 = temp;
    }

    for (int i = 0; i < *sizePtr; i++) {
        if (array[i].addr1 == temp1 && array[i].addr2 == temp2) {
            return *sizePtr;
        }
    }

    if (*sizePtr < 100) {
        array[*sizePtr].addr1 = temp1;
        array[*sizePtr].addr2 = temp2;
        (*sizePtr)++;
    }

    return *sizePtr;
}



int min(int a, int b) {
    if (a<b) {
        return a;
    }
    return b;
}
