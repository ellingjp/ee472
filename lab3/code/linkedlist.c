/*
 * linkedlist.c
 * Author: Patrick Ma
 * 2/11/2014
 *
 * implements the linked list functionality.
 *
 * Some pseudo code ideas takenfrom appendix B of lab
 */

int length;	// Size of the list
void *head;	// the front of the list
void *current;	// current place in the list
void *tail

//
int size() {
  return length;
}

//
void* head() {
  return head;
}

tBoolean endOfList() {
  return NULL == *current;
}

void addNode(void* newNode) {
  if(NULL == head) {	// empty list
    head = newNode;
    tail = newNode;
    size++;
  }
  else {	// insert the newNode between two nodes
    newNode -> next = current -> next;
    current -> next = newNode;
    newNode -> prev = current;
    if (NULL == (newNode -> next)) {	// just added to end of list
      tail = newNode;
    } else {	// in the middle somewhere
      newNode -> next -> prev = newNode;
    }
  }
  size++;
}
