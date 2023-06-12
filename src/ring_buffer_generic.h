#ifdef __cplusplus

#ifndef _RING_BUFFER_GENERIC_
#define _RING_BUFFER_GENERIC_

#include <stdint.h>
#include <string.h>

namespace arduino {

template <int N, typename T>
class RingBufferNGeneric
{
  public:
    T _aucBuffer[N] ;
    volatile int _iHead ;
    volatile int _iTail ;
    volatile int _numElems;

  public:
    RingBufferNGeneric( void ) ;
    void store_elem( T c ) ;
    void clear();
    T read_elem();
    int available();
    int availableForStore();
    T peek();
    bool isFull();

  private:
    int nextIndex(int index);
    inline bool isEmpty() const { return (_numElems == 0); }
};

template <int N, typename T>
RingBufferNGeneric<N, T>::RingBufferNGeneric( void )
{
    memset( _aucBuffer, 0, N ) ;
    clear();
}

template <int N, typename T>
void RingBufferNGeneric<N, T>::store_elem( T c )
{
  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (!isFull())
  {
    memcpy(&_aucBuffer[_iHead], &c, sizeof(c));
    _iHead = nextIndex(_iHead);
    _numElems++;
  }
}

template <int N, typename T>
void RingBufferNGeneric<N, T>::clear()
{
  _iHead = 0;
  _iTail = 0;
  _numElems = 0;
}

template <int N, typename T>
T RingBufferNGeneric<N, T>::read_elem()
{
  if (isEmpty()) {
    T _void_ret;
    return _void_ret;
  }

  auto value = _aucBuffer[_iTail];
  _iTail = nextIndex(_iTail);
  _numElems--;

  return value;
}

template <int N, typename T>
int RingBufferNGeneric<N, T>::available()
{
  return _numElems;
}

template <int N, typename T>
int RingBufferNGeneric<N, T>::availableForStore()
{
  return (N - _numElems);
}

template <int N, typename T>
T RingBufferNGeneric<N, T>::peek()
{
  if (isEmpty()) {
    T _void_ret;
    return _void_ret;
  }

  return _aucBuffer[_iTail];
}

template <int N, typename T>
int RingBufferNGeneric<N, T>::nextIndex(int index)
{
  return (uint32_t)(index + 1) % N;
}

template <int N, typename T>
bool RingBufferNGeneric<N, T>::isFull()
{
  return (_numElems == N);
}

}

#endif /* _RING_BUFFER_ */
#endif /* __cplusplus */