/*
 * linkedlist.h
 * Author: Patrick Ma
 * 2/11/2014
 *
 * Defines the linkedlist interfce. The linkedlist 
 * dataype produces functionality expected from 
 * linked lists: can traverse, add a node, delete a
 * node, etc.
 */

/*
 * Returns the size of the list
 */
void size();

/*
 * Creates a new linked list. returns a pointer to the
 * head of the list
 */
*listPtr initializeLinkedList();

/*
 * inserts the given node AFTER the current node
 */
void insertNode(void*);

/*
 * Deletes the current node
 */
void deleteNode(void*);

/*
 * Returns true if at the end of the linked list
 */
tBoolean endOfList();

/* Interface for the linked list
 */
typedef struct linkedlist {
  void* initializeLinkedList();
  void addNode (void*);
  void deleteNode(void*);
  int size();
  void* head();
  tBoolean endOfList();
}

