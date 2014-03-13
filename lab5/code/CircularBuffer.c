/* CircularBuffer.c
 * Jonathan Ellington
 * 2/7/14
 */

#include "CircularBuffer.h"
#include <stdio.h>

CircularBuffer cbWrap(void *arr, int se, int ne) {
  CircularBuffer cb;
  cb.array = arr;
  cb.sizeElm = se;
  cb.nElm = ne;
  cb.currElm = 0;

  return cb;
}

/* Returns the current elment in the circular buffer */
void *cbGet(CircularBuffer *cb) {
  int sizeElm = cb->sizeElm;
  int index = cb->currElm;

  unsigned char *bytePtr = (unsigned char *)cb->array;
  bytePtr += sizeElm * index;

  return (void *) bytePtr;
}

/* Returns a pointer to the wrapped array, with the oldest elment
 * at the end and the newest elment at the beginning */
void *cbGetArray(CircularBuffer *cb) {
  return NULL;
}

/* Adds elm to cb */
void cbAdd(CircularBuffer *cb, void *elm) {
  cb->currElm = (cb->currElm + 1) % cb->nElm;

  int sizeElm = cb->sizeElm;
  int index = cb->currElm;

  // copy sizeElm bytes from elm into the right spot
  unsigned char *elmPtr = (unsigned char *) elm;
  unsigned char *arrElmPtr = (unsigned char *) cb->array;
  arrElmPtr += sizeElm * index;

  for (int i = 0; i < sizeElm; i++) {
    arrElmPtr[i] = elmPtr[i];
  }
}
