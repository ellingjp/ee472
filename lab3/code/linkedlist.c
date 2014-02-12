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

/*
 * inserts the given node as the next node in the list
 */
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

/*
 * removes the current node  from the list. The current pointer is unchanged
 */
void deleteNode() {
cases:
  if(NULL == current || NULL == head)	// no elements in list or last in list
    return;
  
 if (NULL == head -> next) // one-element list
   head = NULL;
 	return;
 else {	//any other list
   (current -> prev) -> next = current -> next;
   (current -> next) -> prev = current -> prev;
   return;
 }
}

/*
 * Initializes an instance of a linked list, returns a pointer to the current
 * position in the list.
 */
void* initializeLinkedList(){
	linkedlist myList;

	mylist.length = 0;
	mylist.head = NULL;
	mylist.tail = NULL;
	mylist.current = NULL;

	return mylist.current;
}
