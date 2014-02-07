/* CircularBuffer.h
 * Jonathan Ellington
 * 2/7/14
 */

/* A circular buffer implementation.
 * Meant to work without any dynamically allocated memory by wrap()ing 
 * user defined arrays.
 *
 * NOTE: This implementation keeps NO type information.  This means the 
 *       USER IS RESPONSIBLE FOR KEEPING TRACK OF THE TYPE OF ARRAY THAT
 *       HAS BEEN WRAPPED!
 *
 * NOTE: The implementation here should also be hidden, but this is
 *       troublesome without dynamic memory.  Do not rely on any of these
 *       elements!
 */
typedef struct _circBuf {
  void *array;
  int sizeElm;
  int nElm;
  int currElm;
} CircularBuffer;

/* Wrap an array in a circular buffer.  
 *       
 * @param arr       the array to wrap
 * @param sizeElem  the size of each element, in bytes
 * @param nElem     the number of elements in the array
 *
 * This function expects the array is freshly created.  It will overwrite
 * array contents on adds.
 */
CircularBuffer cbWrap(void *arr, int sizeElem, int nElem);

/* Returns a pointer to the current element in the circular buffer 
 * Be sure not to clobber this value! */
void *cbGet(CircularBuffer *cb);

/* Returns a pointer to the wrapped array, with the oldest element
 * at the end and the newest element at the beginning */
void *cbGetArray(CircularBuffer *cb);

/* Adds elem to cb */
void cbAdd(CircularBuffer *cb, void *elem);

