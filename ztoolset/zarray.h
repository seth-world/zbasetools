#ifndef ZARRAY_H
#define ZARRAY_H

/*
 *   ===========WARNING :   Do not include zerror.h in this include file  ========================
 *                         Do not use ZException in this include file
 */

//#define __USE_NEW_STD_ALLOCATOR__

#include <cassert>
#include <memory.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <initializer_list>

#include <config/zconfig.h>

#include "zlimit.h"
#include "zmem.h"  // for zfree()

#include "zerror_min.h"
#include "zbaseparameters.h"

//extern class ZGeneralParameters GeneralParameters;

#include <QList>

#ifdef __USE_ZTHREAD__
    #include <zthread/zmutex.h>
#endif


#include "zatomicconvert.h"




namespace zbs {

using namespace std;

/**
 * @brief The ZAExport struct
 *
@verbatim
                      +-------> DataOffset
  offset              |
  0                   |----------AllocatedSize-----------------|
  XXXXXXXXXXXXXXXXXXXX..........................................

<--- ZAExport -----><-------- ZArray flat data -------------->
            .Fullsize
            .AllocatedSize
            .AllocatedElements
            .ExtentQuota
            .InitialAllocation
            .NbElements
            .DataOffset

        @endverbatim



            */
#pragma pack(push)
#pragma pack(1)
template <class _Tp>
class ZArray;


class ZAExport
{
public:
  ZAExport()=default;

  uint32_t  StartSign=cst_ZMSTART;
  uint16_t  EndianCheck=cst_EndianCheck_Normal;
  size_t    FullSize=0;        //!< full size in byte of the export (ZAExport + size of exported data -see DataSize )
  size_t    AllocatedSize=0;   //!< ZArray Allocated Size
  size_t    CurrentSize=0;     //!< ZArray CurrentSize : current number of elements * size of one element
  size_t    DataSize=0;        //!< Size of ZArray exported data or data to import : equals to sizeof(_TpOut)*NbElements
  size_t    AllocatedElements=0; //!< ZArray Number of allocated elements
  size_t    ExtentQuota=0;     //!< ZArray ExtentQuota value
  size_t    InitialAllocation=0;
  ssize_t   NbElements=0;      //!< ZArray NbElements value

  void setFromPtr(const unsigned char* &pPtrIn);

  ZAExport& _copyFrom(const ZAExport& pIn);

  ZAExport(const ZAExport& pIn) {_copyFrom(pIn);}

  ZAExport& operator = (const ZAExport& pIn) {return _copyFrom(pIn);}

  template <class _Tp>
  void setZArray(ZArray<_Tp>* pArray);


  void _convert ();

  void serialize();
  void deserialize();

  bool isReversed()     {if (EndianCheck==cst_EndianCheck_Reversed) return true; return false;}
  bool isNotReversed()  {if (EndianCheck==cst_EndianCheck_Normal) return true; return false;}

};
#pragma pack(pop)




/** @addtogroup NOTUSABLEWITHINDEX This function/method is not usable with ZArrays
 * @{
 * @warning This function/method is not usable with ZArrays that involve index managements because of the possible Index operations that may occur.
 *                                  Non limitative list of these ZArray child is : ZSortedArray, ZAM, ZIndex,...
 * @}
 */

/** @brief The ZArray class This template class allows dynamic arrays objects. Dynamic means that it could be extended or trimmed dynamically.
 *
 *  @warning Note to developpers : it is forbidden use Stack (a zbs utility) within following classes because Stack management itself uses ZArrays
 *
 *  @section ZArrayAccess Accessing ZArray elements' data
 *
 * Both syntaxes are valid
 *
 * 1.       name[index]
 *
 * 2.       name.Tab[index]
 *
 * 3.       name.Data[index]
 *
 *
 * @warning  brackets operators are overloaded for this occasion. It might cause unpredictable result if not used in compliance with the overloading conditions.
 *
 * @note
 * Important notice : size() gives the current number of elements populating the ZArray.
 * When ZArray is empty : size is set to -1.
 * Using an empty ZArray will cause a failure (see message from stderr).
 *
 * Using pointer to ZArray :
 *      You can access data within ZArray pointers using previous 2. and 3. as described below :
 *      - name->Tab[index]
 *      - name->Data[index]
 *
 * @section ZArrayParameters ZArrays parameters
 * ZArray parameters at creation time impacts on how ZArray will allocate memory.
 *
 * - pInitialAlloc
 *
 *  Initial allocation. defaulted to constant _cst_default_allocation.
 *  This value represents the number of elements initially allocated to the array.
 *  This value is stored in ZArray object, so that, when ZArray is cleared, memory allocation is set by computing this minimum value.
 *  @note this is not a memory size, but a number of elements. The size may be computed by pInitialAlloc * sizeof(_Tp)
 *
 * - pReallocQuota
 *  Reallocation quota. defaulted to constant _cst_realloc_quota.
 *  This value is used as a number of elements.
 *  When ZArray memory is exhausted, then ZArray will automatically reallocate memory using this value to compute the memory size to reallocate.
 *  @note this is not a memory size, but a number of elements. The size may be computed by pReallocQuota * sizeof(_Tp)
 *
 *
 * Warning : ZArray class is not copyable
 *
 *
 */

typedef long ZArrayIndex_type;

using namespace std;

template <typename _Tp>
class ZArray : protected QList<_Tp>
{
public :

  friend class ZIndexItem;
  friend class ZIndexCollection;
  friend class DirMap;


  typedef QList<_Tp> _Base;

  using _Base::isEmpty;
  using _Base::count;
  using _Base::size;
  using _Base::operator[];
//  using _Base::operator<<;
//  using _Base::swap;
//  using _Base::iterator;

protected:
  size_t ZReallocQuota=0 ;      // reallocation quota in number of elements to reallocate each reallocation operation
  size_t ZAllocation=0 ;        //  space for number of elements currently allocated
  size_t ZInitialAllocation=0;  // keep initial allocation for reset purpose : ZClear will get back to this memory allocation
  _Tp ZReturn ;               // memory zone for returning element (WARNING: not to be used with multi-thread)

private :
  long ZIdx=0 ;                 // Last index accessed

#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
  ZMutex _Mutex ;
#endif
public:

  ZArray() ;
  ZArray (size_t pInitialAlloc,
         size_t pReallocQuota) ;

   ~ZArray();

 //  ZArray(ZArray &) = delete;                // can not copy
 //  ZArray& operator = (ZArray&)=delete;     // can not copy
//#ifdef __USE_ZARRAY_COPY_CONTRUCTOR__

   ZArray(ZArray& pIn);
   ZArray( ZArray&& pIn);

   ZArray& operator = (ZArray&pIn)
    {
//       _mInit(); /* init with default allocation values */
       _copyFrom(pIn);
       return *this;
    }
   ZArray& operator = (ZArray&&pIn)
   {
//      _mInit(); /* init with default allocation values */
      _copyFrom(pIn);
      return *this;
   }


   bool exists(long pIdx){if ((pIdx<0)||(pIdx>lastIdx())) return false; return true;}

   void _mInit(size_t pInitialAlloc=_cst_default_allocation,
                size_t pReallocQuota=_cst_realloc_quota);



   ZArray(_Tp const& __a) //_GLIBCXX_NOEXCEPT
   {
       _mInit(_cst_default_allocation,_cst_realloc_quota);
       push(__a);
   }


   ZArray(std::initializer_list<_Tp> l)
   {
    _mInit(_cst_default_allocation,_cst_realloc_quota); /* initialize with default allocation values */
    _Tp*       it  =(_Tp*) l.begin();  // raw pointer!
    _Tp* const end = (_Tp*)l.end();    // raw pointer!

    for (; it != end; ++it)
       this->push((_Tp&)*it);
   }


   /**
    *  @brief  %ZArray list assignment operator.
    *  @param  __l  An initializer_list.
    *
    *  This function fills a %ZArray with copies of the elements in the
    *  initializer list @a __l.
    *
    *  Note that the assignment completely changes the %ZArray and
    *  that the resulting %ZArray's size is the same as the number
    *  of elements assigned.
    */
   ZArray&
   operator=(std::initializer_list<_Tp> __l)
   {
     _Tp*       it  = (_Tp*)__l.begin();  // raw pointer!
       _Tp* const end =(_Tp*) __l.end();    // raw pointer!

       for (; it != end; ++it)
         this->push((_Tp&)*it);
    return *this;
   }

/*==============================operators overload===========================================
 *      Overloaded index operator [] for simplification purposes & direct access to data elements
 */

   /** operators to be overloaded for ZIndex */

   /**
    * @brief operator [] gives direct access to ZArray row data using its index rank. \n
    *                   this operator is overloaded for some child to return a constant reference in order to prevent modifying ZArray content by this mean.
    */
/*  _Tp & operator [] ( long pIndex) {
    if ((pIndex >= 0)&&(pIndex < _Base::size())) {
      return (Tab(pIndex));
    }
      fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
      abort();
  }
  const _Tp & operator []  (long pIndex) const {
    if ((pIndex >= 0)&&(pIndex <  _Base::size()))
      return (TabConst(pIndex));
    fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
    abort();
  }
*/
  _Tp & Tab(long pIndex) {
    if ((pIndex >= 0)&&(pIndex <  _Base::size()))
      return (_Base::operator[](pIndex));
    fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
    abort();
  }

  const _Tp & TabConst(long pIndex) const {
    if ((pIndex >= 0)&&(pIndex <  _Base::size()))
      return (_Base::at(pIndex));
    fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
    abort();
  }


  /**
    * @brief operator << pops the ZArray and returns a reference to the result within the field of type _Tp mentionned as left argument
    * @return
    */
   friend _Tp& operator << (_Tp &pRes,ZArray<_Tp> &pZA) {
        return(pZA.popRP(&pRes));
   }
   /**
     * @brief operator << alias for push() method.
     * @return ZArray object instance
     */
   ZArray<_Tp>& operator << ( _Tp pInput) {
        push(pInput);
       return *this;
   }


//   friend  ZArray<_Tp>* operator << (ZArray<_Tp>* pFirst,_Tp&& pSecond );

   /**
    * @brief operator -- alias for pop() method
    * @return size of array after pop operationn or -1 if array was empty
    */
   long operator -- (void) {return(pop());}

/** end operator to be overloaded for ZIndex */


/** =================================================================
 *      Methods to be protected when using ZSortedArray
 */

   long push(_Tp &pElement) ;
   long pushVal(_Tp pElement) ;
   long _pushNoLock(_Tp &pElement);
   long push(const _Tp& pElement);
   long _pushNoLock(const _Tp &pElement);
   long push(_Tp &&pElement);
   long pushConst(const _Tp &&pElement);
   long push_front (_Tp pElement) ;

   long insert (_Tp &pElement, size_t pIdx);
   long insert(_Tp &&pElement, size_t pIdx);

    long insert (_Tp *pElement, size_t pIdx, size_t pNumber);

    long erase(size_t pIdx);

    long erase(size_t pIdx,size_t pNumber);

    long _insertNoLock(_Tp &pElement, size_t pIdx);
    long _insertNoLock(_Tp *pElement, size_t pIdx, size_t pNumber);
    long _eraseNoLock(size_t pIdx,size_t pNumber=1);

           //!< @brief trim adjusts the memory size of the ZArray to the current contained elements + realloc quota
    /** ! @brief last returns the ZArray content of the last element of the ZArray. If ZArray is empty, it issues a fatal error and terminates.
     */
    _Tp &last(void)
    {
      if (_Base::size() < 1) {
        fprintf(stderr,"ZArray::last-F-OUTOFBOUND empty ZArray, Out of boundaries\n");
        abort() ;
      }
#ifdef __ZTHREAD_AUTOMATIC__
      _Mutex.lock();
#endif
      ZReturn = _Base::last();
#ifdef __ZTHREAD_AUTOMATIC__
      _Mutex.unlock();
#endif
      return ZReturn;
    }

  //! @brief ZArray::lastIdx returns the index for the last ZArray element : WARNING:  when ZArray is empty, returns -1
    long lastIdx (void) {return (_Base::isEmpty()?-1:(_Base::size()-1));}


//========================End methods to be protected=============

    long pop(void);     /* erase last  */
    long _pop_nolock(void);
    long pop_front(void);   /* erase first  */
    long _pop_front_nolock(void);

    _Tp& popR(void);            /* pop with return : erase last and return erased */
    _Tp& _popR_nolock(void);
    _Tp& popR_front(void);      /* pop front with return : erase first and return erased  */
    _Tp& _popR_front_nolock(void);

    _Tp & popRP(_Tp* pReturn);    /* pop with return thread safe */
    _Tp& _popRP_nolock(_Tp* pReturn);

    _Tp & popRP_front(_Tp* pReturn);  /* pop front with return thread safe */
    _Tp & _popRP_front_nolock(_Tp* pReturn);


#ifdef __DEPRECATED__
    long newBlankElement(size_t pNumber=1, bool pLock=false, void (*pClear)(_Tp &)=nullptr);
#endif

    long swap (size_t pDest, size_t pOrig,size_t pNumber);
    long swap (size_t pDest, size_t pOrig);

//    void clearValue (const long pIdx);

    _Tp & reverse (long pIdx);

//    size_t bzero (size_t pOrig=0, long pNumber=-1, bool pLock=true);
    void reset(void);
    void clear(bool pLock=true);

    ZArray<_Tp>* clone(void);

 //   void copy(ZArray<_Tp> &pZCopied)  {copy(static_cast<const ZArray<_Tp>> (pZCopied));}
    ZArray<_Tp> & _copyFrom(ZArray<_Tp> &pIn)  ;


    /** @brief elementSize  returns the size in bytes of one _Tp element i.e. the size of one row fo the array */
    long elementSize(void) ;
    /** @brief usedSize  returns the size in bytes of one _Tp element (size of one row of the array). Alias for getCurrentSize()*/
    long usedSize(void) ;

    size_t getAllocation (void) const {return (ZAllocation);} /*!< returns the number of elements currently being allocated (and not those used) */
    size_t getQuota(void) const {return (ZReallocQuota);} /*!< returns the reallocation quota for the ZArray : Number of elements that will be allocated each time ZArray needs additional space */
    size_t getInitalAllocation() const {return ZInitialAllocation;}
    /** @brief getAllocatedSize returns the THEORICAL size in bytes of the memory currently allocated to the ZArray instance */
    size_t getAllocatedSize(void) const {return(ZAllocation*sizeof(_Tp));}
    /** @brief  getCurrentSize returns the size in bytes taken by current number of elements */
    ssize_t getCurrentSize(void) const { return (sizeof(_Tp)*_Base::size());}

    void setAllocationValues(size_t pAllocation,size_t pReallocQuota) {setAllocation(pAllocation); setQuota(pReallocQuota);}
    inline void setQuota (size_t pQuota) {ZReallocQuota=pQuota;} /** !< sets the new reallocation quota for the ZArray in number of elements */
    void setAllocation (size_t pAlloc, bool pLock=true);
    void setInitialAllocation(size_t pInitialAllocation,bool pLock=true);
    long allocateCurrentElements (const ssize_t pAlloc);

    /** @brief getDataPtr returns the current pointer to data content. Warning : this pointer is not stable and is to be changed whenever reallocation occurs */
//    inline void* getDataPtr() const {return (void*)Tab;}

/** @brief ZArray<_Tp>::search  searches with binary search for byte sequence contained in pKey of size pSize starting at ZArray row index pIdx.
 *                             it returns the ZArray row index where it has been found or -1 if not found.
 *
 *                             For successive searches, it is of the responsibility of user to manage the indexes
 *
 * @param pKey      void pointer to byte sequence to search for
 * @param pSize     byte sequence length
 * @param pIdx      row index to start the search from
 * @return
 */
    long search (const void *pKey, size_t pSize, long pIdx=0);

    long searchforValue (long pOffset,long pSize, const void *pValue);    //! sequential search (costly) for a value


//-------------- end search routines--------------------------


    //  ------------------------ Export Import utilities---------------------------

/**
  @brief ZArray export import utilities


  Beginning           +-------> DataOffset
    |
    0                   |----------AllocatedSize-----------------|
    XXXXXXXXXXXXXXXXXXXX..........................................

        <--- ZAExport -----><-------- ZArray flat data -------------->
                  .Fullsize
                  .AllocatedSize
                  .AllocatedElements
                  .ExtentQuota
                  .NbElements
                  .DataOffset

*/

  ZAExport getZAExport(void) const;


//---------Mutex methods---------------
public:
#ifdef __USE_ZTHREAD__
    void lock(void)     {_Mutex.lock();}
    void unlock(void)   {_Mutex.unlock();}
#endif  //__USE_ZTHREAD__

/** ==================== memory allocation management ====================== */

protected :
  /**
     * @brief allocate  set memory allocation to pAlloc number of elements
     * @param pAlloc
     */
    void allocate(long pAlloc)
    {
      if (pAlloc < _Base::size()) {
        fprintf (stderr,"ZArray::allocate-W-ERRALLOC requested new allocation <%ld> not greater than existing number of elements within ZArray <%ld> .\n"
                        "You should suppress elements from the ZArray and use trim/resize facility to reduce the memory allocated (ZArray::trim).\n",
            pAlloc,_Base::size());
        return ;
      }

      if (ZAllocation==0) {
        _Base::reserve(pAlloc);
        ZAllocation = pAlloc;
        return;
      }
      /* Must trim ? */
      if (pAlloc < ZAllocation) {
        if (pAlloc < _Base::size()) {
          pAlloc = _Base::size();
          ZAllocation = pAlloc;
          return;
        }
        _Base::reserve(pAlloc);
        ZAllocation = pAlloc;
        return;
      }

       _Base::reserve(pAlloc);
       ZAllocation = pAlloc;
      return ;
    }// allocate

    /**
     * @brief extend  extends allocation to current allocation plus pExtent in terms of number of elements
     * @param pExtent
     */
    void extend(long pExtent)
    {
      if (isEmpty()) {
        _Base::reserve(ZAllocation + pExtent);
        ZAllocation += pExtent;
        return;
      }
      if (pExtent < 0) {

        if ((ZAllocation + pExtent) < _Base::size()) {
          return;
        }
        _Base::reserve(ZAllocation + pExtent);
        ZAllocation += pExtent;
        return;
      }

      /* Must trim ? */
// if (pExtent < _Base::capacity()) {
      if ((ZAllocation + pExtent) < _Base::size()) {
        return;
      }

      _Base::reserve(ZAllocation + pExtent);
      ZAllocation = ZAllocation + pExtent;
      return ;
    }// allocate

/**
 * @brief trim resizes ZArray allocated memory to the current number of elements plus one reallocation quota elements
 *  memory allocator neutral
 */

    void  trim()
    {
      _Base::reserve(_Base::size()+ZReallocQuota);
      ZAllocation = _Base::size()+ZReallocQuota;
    }



}; // class ZArray


/* --------------- Begin developed methods ------------------*/
template <typename _Tp>
#ifdef __USE_ZTHREAD__
ZArray<_Tp>::ZArray():_Mutex() {
#else
#pragma message ("ZArray: Thread management is disactivated for template ZArray")
ZArray<_Tp>::ZArray() {
#endif
    _mInit(_cst_default_allocation,_cst_realloc_quota);
}

template <typename _Tp>
#ifdef __USE_ZTHREAD__
ZArray<_Tp>::ZArray (size_t pInitialAlloc, size_t pReallocQuota) : _Mutex() {
#else
#pragma message ("ZArray: Thread management is disactivated for template ZArray")
ZArray<_Tp>::ZArray (size_t pInitialAlloc, size_t pReallocQuota) {
#endif
    _Tp wBlank;
    push(wBlank);
    _mInit(pInitialAlloc,pReallocQuota);
    pop();
}
template <typename _Tp>
#ifdef __USE_ZTHREAD__
ZArray<_Tp>::ZArray(ZArray& pIn):_Mutex()
#else
#pragma message ("ZArray: Thread management is disactivated for template ZArray")
ZArray<_Tp>::ZArray( ZArray& pIn)
#endif
{
    _mInit(_cst_default_allocation,_cst_realloc_quota);
    _copyFrom(pIn);
    return;
}
template <typename _Tp>
#ifdef __USE_ZTHREAD__
ZArray<_Tp>::ZArray( ZArray&& pIn):_Mutex()
#else
#pragma message ("ZArray: Thread management is disactivated for template ZArray")
ZArray<_Tp>::ZArray( ZArray&& pIn)
#endif
{
    //       _mInit(); /* init with default allocation values */
    _mInit(_cst_default_allocation,_cst_realloc_quota);
    _copyFrom(pIn);
    return;
}
/**
 * operator << overload that cannot be put as friend within class due to template limitations
 */
#ifndef __OPERATOR_ZARRAY_PUSH_OVERLOAD__
#define __OPERATOR_ZARRAY_PUSH_OVERLOAD__

template <typename _Tp>
ZArray<_Tp>& operator << (ZArray<_Tp> *pZA,const _Tp &pRes)
{
    return(pZA->push(pRes));
}
template <typename _Tp>
ZArray<_Tp>& operator << (ZArray<_Tp> *pZA, _Tp &pRes)
{
    return(pZA->push(pRes));
}
template <typename _Tp>
/**
  * @brief operator << pops the ZArray (on the left side) mentionned as a POINTER and returns the result within the field of type _Tp mentionned as left argument
  * @return the content of the poped ZArray value within the mentionned field of type _Tp
  */
_Tp& operator << (_Tp &pRes,ZArray<_Tp> *pZA)
     {
     return(pZA->popRP(&pRes));
     }

#endif // __OPERATOR_ZARRAY_PUSH_OVERLOAD__


/** -------------------------------end operator << overload---------------------------------*/

template <typename _Tp>
void ZArray<_Tp>::_mInit(size_t pInitialAlloc,
                         size_t pReallocQuota)
{
    ZInitialAllocation=pInitialAlloc ;
    ZReallocQuota = pReallocQuota ;
    _Base::reserve(pInitialAlloc);
    ZAllocation = pInitialAlloc;
    ZIdx=0;
}

template <typename _Tp>
ZArray<_Tp>::~ZArray()
{

}//DTOR


template <typename _Tp>
/**
 * @brief ZArray<_Tp>::clone returns a pointer to a newly allocated ZArray that is a strict clone (structure and content) of the current ZArray.
 *                           the cloned ZArray MUST be deleted by user to free memory.
 */
ZArray<_Tp>*
ZArray<_Tp>::clone(void)
{
    ZArray<_Tp> *wRet= new ZArray<_Tp>(*this);
    return (wRet);
}// clone

/**
 * @brief ZArray<_Tp>::copy  Copies to the current ZArray<_Tp> a similar ZArray<_Tp> (structure and data).
 * sets up ZArray parameters with those of the copied ZArray.
 * Clears content of current ZArray and replaces it with the copied ZArray's content.
 *      see also @ref clone()
 *
 * @param pZCopied the ZArray to be copied. It must be of the same template type than the receiving current ZArray.
 * @return
 */
template <typename _Tp>
ZArray<_Tp>&
ZArray<_Tp>::_copyFrom (ZArray<_Tp> &pIn)
{
#ifdef __ZTHREAD_AUTOMATIC__
    pIn.lock();
#endif

    this->clear(false);
    setQuota(pIn.ZReallocQuota);
    allocate(pIn.ZAllocation);

    for (long wi=0;wi<pIn.count();wi++)
      _pushNoLock(pIn[wi]);
#ifdef __ZTHREAD_AUTOMATIC__
    pIn.unlock();
#endif
    return *this;
}


template <typename _Tp>
/**
 * @brief ZArray::reverse returns the ZArray data in reverse rank order (0 corresponds to the last ZArray index, etc.)
 * @param pIdx
 * @return
 */
inline _Tp & ZArray<_Tp>::reverse (long pIdx)
{
#if __DEBUG_LEVEL__ > 0
  if ((pIdx <0) || (pIdx >= size())) {
    fprintf (stderr,"ZArray::reverse-F-outofboundaries Fatal error : given index <%ld> is out of ZArray boundaries\n",pIdx);
    std::cerr.flush();
  }
#endif
  return (at(size()-pIdx-1));
}
/** @brief move  moves the pNumber elements of the dynamic array from pOrig index to pDest index.
 *          pOrig content remains unchanged except if there is an overlap between destination and origin
 *          pOrig value must be included in the existing index values i.e. [0;ZCurrentNb[ (ZCurrentNb excluded)
 *          pDest value must be included in the existing index values or must equal ZCurrenNb. In this case the values will be added at the tail of array.
 *
 *          adds allocation following allocation rules for new elements if required (using @ref addCheck).
 *
 *          move may replace the destination mentionned
 *
 *      @see NOTUSABLEWITHINDEX
 *
 *  @return pDest if successful -1 if input parametes are errored
 *
 *
 */

/** @brief swap  exchange the pNumber elements of the dynamic array from pOrig index to pDest index.
 *          pOrig content remains unchanged except if there is an overlap between destination and origin
 *          pOrig value must be comprized in the existing index values i.e. [0;ZCurrentNb[ (ZCurrentNb excluded)
 *          pDest value must be comprized in the existing index values or must equal ZCurrenNb. In this case the values will be added at the tail of array.
 *
 *          adds allocation following allocation rules for new elements if required (@ref addCheck).
 *          if destination of swap (pDest) is either partially or totally outside the current number of elements,
 *          then the additional fraction of elements will be zeroed BEFORE being swapped.
 *
 *  @see NOTUSABLEWITHINDEX
 *
 *  @return pDest if successful -1 if input parametes are errored
 *
 *
 */
template <typename _Tp>
long ZArray<_Tp>::swap (size_t pDest, size_t pOrig,size_t pNumber)
{
  if (pDest==pOrig) {
    return pDest;
  }
//    if (((pOrig+pNumber)>lastIdx())||(pOrig<0)||(pDest<0)||(pDest>lastIdx()))
  if (((pOrig+pNumber)>lastIdx())||(pDest>lastIdx())) {// pOrig & pDest are unsigned longs so a negative longs will produce an outofboundaries error nevertheless
#if __DEBUG_LEVEL__ > 0
      fprintf (stderr, "ZArray::swap-E-OUTBOUND Error, given index <%ld> plus number of elements to insert <%ld> are out of ZArray boundaries. Last index is <%ld>\n"
                        "                        OR destination index <%ld> is out of ZArray boundaries.\n",
                pOrig,
                pNumber,
                lastIdx(),
                pDest );
#endif

      return -1 ; // out of boundaries for
  }
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  ZArray<_Tp> wB;
  long wF1 = pDest + pNumber;
  long wj=pOrig;
  for (long wi=pDest;wi < wF1;wi++){
     _Base::swapItemsAt(wi,wj);
  }
  ZIdx = pDest ;
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return pDest ;
}// swap

template <typename _Tp>
long ZArray<_Tp>::swap (size_t pDest, size_t pOrig)
{
  if (pDest==pOrig) {
    return pDest;
  }

  if ((pOrig > lastIdx())||(pDest>lastIdx())) {// pOrig & pDest are unsigned longs so a negative longs will produce an outofboundaries error nevertheless
#if __DEBUG_LEVEL__ > 0
    fprintf (stderr, "ZArray::swap-E-OUTBOUND Error, given index <%ld> plus one is out of ZArray boundaries. Last index is <%ld>\n"
                    "                        OR destination index <%ld> is out of ZArray boundaries.\n",
        pOrig,
        lastIdx(),
        pDest );
#endif

    return -1 ; // out of boundaries for
  }
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _Base::swapItemsAt(pDest,pOrig);

  ZIdx = pDest ;
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return pDest ;
}// swap

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::insert inserts pNumber elements pointed by pElements at index pIdx after having shifted appropriately the ZArray elements
 * @param pElement
 * @param pIdx
 * @param pNumber
 * @returnpIdx if successful -1 if input parametes are errored : index out of range
 */
long ZArray<_Tp>::insert(_Tp &pElement, size_t pIdx)
{
  if (pIdx >= _Base::size()) {
    return push(pElement);
  }
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _Base::insert(pIdx,pElement);
  ZIdx = pIdx;
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZIdx);
}// insert

template <typename _Tp>
/**
 *   same as previous but for contants
 */
long ZArray<_Tp>::insert(_Tp &&pElement, size_t pIdx)
{
    if (pIdx >= _Base::size()) {
        return push(pElement);
    }
#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.lock();
#endif

    _Base::insert(pIdx,pElement);
    ZIdx = pIdx;
#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.unlock();
#endif
    return (ZIdx);
}// insert


template <typename _Tp>
long ZArray<_Tp>::_insertNoLock(_Tp &pElement, size_t pIdx)
{
  if (pIdx > _Base::size()) {
    return _pushNoLock(pElement);
  }
   _Base::insert(pIdx,pElement);
  ZIdx=pIdx;
  return (pIdx);
}// insert

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::insert inserts pNumber elements pointed by pElements at index pIdx after having shifted appropriately the ZArray elements
 * @param pElement
 * @param pIdx
 * @param pNumber
 * @returnpIdx if successful -1 if input parametes are errored : index out of range
 */
long ZArray<_Tp>::insert(_Tp *pElement, size_t pIdx,size_t pNumber)
{
  if (pIdx >= _Base::size()) {
      return (-1);
  }

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  for (long wi=0; wi < pNumber;wi++)
    pIdx = _insertNoLock(pElement[wi],pIdx);
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (pIdx);
}// insert


/** @brief erase  suppresses the pNumber elements of the dynamic array starting at and including pIdx index.
 *
 * @return the erased index position if successful
 *          -1 if input parametes are errored
 *
 *
 */
template <typename _Tp>
long ZArray<_Tp>::erase(size_t pIdx,size_t pNumber)
{
  if ((long)(pIdx+pNumber)>_Base::size())
    return -1;

  if (pIdx==lastIdx())
    return pop();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  pIdx=_eraseNoLock(pIdx,pNumber);

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (pIdx);
}// erase

/** @brief erase  suppresses the pNumber elements of the dynamic array starting at and including pIdx index.
 *
 * @return the erased index position if successful
 *          -1 if input parametes are errored
 *
 *
 */
template <typename _Tp>
long ZArray<_Tp>::erase(size_t pIdx)
{
  if (pIdx >_Base::size())
    return -1;

  if (pIdx==lastIdx())
    return pop();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _Base::removeAt(pIdx);

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (pIdx);
}// erase

template <typename _Tp>
long ZArray<_Tp>::_eraseNoLock(size_t pIdx,size_t pNumber)
{
  if (pIdx==_Base::size()-1)
    return _pop_nolock();
  if ((long)(pIdx+pNumber)>(_Base::size()-1))
    return -1;
  for (long wi=pIdx;(wi < wi+pNumber)&&(wi < _Base::size()-1);wi++)
    _Base::removeAt(pIdx);

  return (ZIdx=pIdx);
}



template <typename _Tp>
/**
 * @brief ZArray<_Tp>::clear reset the ZArray to the minimum memory(ZInitialAllocation), resets the number of elements to zero, set the allocated surface to zero.
 */
void ZArray<_Tp>::clear(bool pLock)
{
#ifdef __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.lock();
#endif
    //    _Base::clear();
    while (_Base::size() > 0)
      _Base::removeLast();
    ZIdx = 0;
    allocate( ZInitialAllocation);
//    memset (ZPtr,0,ZAllocatedSize);
#ifdef __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.unlock();
#endif
    return ;
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::reset resets the number of elements to zero without deallocating the used memory.
 */
void ZArray<_Tp>::reset(void)
{
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  ZIdx = 0;

  while (_Base::size() > 0)
    _Base::removeLast();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return ;
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::usedSize returns current used space size in bytes
 * @return
 */
inline long ZArray<_Tp>::usedSize(void)
{
  return (_Base::size()*sizeof(_Tp));
}


template <typename _Tp>
/**
 * @brief ZArray<_Tp>::elementSize returns the size in bytes of one _Tp element (size of one row of the array)
 * @return
 */
inline long ZArray<_Tp>::elementSize(void)
{
    return (sizeof(_Tp));
}









/* ========================== End memory allocation =================== */



template <typename _Tp>
/**
 * @brief setAllocation  modifies the number of allocated Elements to the given number.
 *                       setAllocation "makes room" for new elements WITHOUT changing the current number of elements within the ZArray
 *                       @see allocateCurrentElements
 *
 * Checks if given allocation is compatible with current ZArray content.
 *
 * @param pAlloc the new number of elements that the ZArray could contain.
 */
void ZArray<_Tp>::setAllocation (size_t pAlloc,bool pLock)
{
#ifdef __ZTHREAD_AUTOMATIC__
  if (pLock)
      _Mutex.lock();
#endif
  if (pAlloc<(_Base::size()+ZReallocQuota)) {
      if (BaseParameters->VerboseBasic())
      fprintf(stderr,"ZArray::setAllocation-W-BADALLOC Warning allocation given <%ld> is less than current number of elements <%ld> + allocation quota <%ld>. Trimming the ZArray...\n",
                        pAlloc,
                        _Base::size(),
                        ZReallocQuota);
    _Base::reserve(pAlloc) ;

#ifdef __ZTHREAD_AUTOMATIC__
    if (pLock)
      _Mutex.unlock();
#endif
    return ;
  }//if (pAlloc<(size()+ZReallocQuota

  allocate(pAlloc);
#ifdef __ZTHREAD_AUTOMATIC__
  if (pLock)
    _Mutex.unlock();
#endif
                return;
}  // setAllocation
template <typename _Tp>
void
ZArray<_Tp>::setInitialAllocation(size_t pInitialAllocation,bool pLock)
{
    ZInitialAllocation=pInitialAllocation;
//    if (pInitialAllocation > _Base::capacity())
    setAllocation(pInitialAllocation,pLock);
    return;
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push(_Tp &pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
  long wR=_pushNoLock(pElement);

#ifdef __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return(wR) ;
} // push

template <typename _Tp>
long ZArray<_Tp>::pushVal(_Tp pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.lock();
#endif
    long wR=_pushNoLock(pElement);

#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.unlock();
#endif
    return(wR) ;
} // pushVal
template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push(const _Tp &pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  long wR=_pushNoLock(pElement);

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return(wR) ;
} // push

template <typename _Tp>
long ZArray<_Tp>::_pushNoLock( _Tp &pElement)
{
  _Base::append(pElement);
  return lastIdx();
} // push
template <typename _Tp>
long ZArray<_Tp>::_pushNoLock( const _Tp &pElement)
{
  _Base::append(pElement);
  return lastIdx();
} // push

//template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */

long ZArray<_Tp>::push( _Tp &&pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  _Base::append(pElement);
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return(lastIdx()) ;
} // push with litteral

template <typename _Tp>
long ZArray<_Tp>::pushConst(const _Tp &&pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.lock();
#endif
    _Base::append(pElement);
#ifdef __ZTHREAD_AUTOMATIC__
    _Mutex.unlock();
#endif
    return(lastIdx()) ;
} // push with litteral
template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push_front inserts pElement as the first element of ZArray. The elements collection is shifted appropriately. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push_front(_Tp pElement)
{
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  _Base::prepend(pElement);
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (0) ;  // index for inserted element (if needed)
} // push_front


/** @brief popR : NOT ThreadSafe: erases the last element and return a pointer to the poped element's content (that remains available but not within the ZArray)
 *
 *  WARNING : Not Thread safe.
 *
 *  @return a reference to poped (removed) element : poped element stays at disposal until ZArray object itself is removed
 *
 */
template <typename _Tp>
_Tp & ZArray<_Tp>::popR(void)
{


  if (_Base::size()<1) {
      fprintf (stderr,"popR-F-INVZCURRNB Fatal error : index of ZArray <%d> is < 1 \n",
      _Base::size());
      abort();
  }
#ifdef __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
      ZReturn = _Base::last();
      _Base::removeLast();
#ifdef __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZReturn);
} // popR

template <typename _Tp>
_Tp & ZArray<_Tp>::_popR_nolock(void)
{
 // _hasChanged=true;
  if (_Base::size()<1)
  {
    fprintf (stderr,"_popR_nolock-F-INVZCURRNB Fatal error : index of ZArray <%ld> is < 1 \n",
        _Base::size());
    abort();
  }

  ZReturn = _Base::last();
  _Base::removeLast();
  return ZReturn;
} // popR_nolock

/** @brief popRP : ThreadSafe: erases the last element and returnsthe poped element's content into pReturn (that remains available but not within the ZArray)
 * @warning Not Thread safe.
 *  @return a reference to poped (removed) element content
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP(_Tp* pReturn)
{
  if (_Base::size() < 1) {
    fprintf (stderr,"popRP-F-INVZCURRNB Fatal error : index of ZArray <%d> is < 1 \n",
        _Base::size());
    abort();
  }

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  *pReturn = _Base::takeLast();
#ifdef __ZTHREAD_AUTOMATIC__
   _Mutex.unlock();
#endif
  return (*pReturn);
} // popRP

/** @brief popRP_NL : pop result no lock : erases the last element and returns the poped element's content into pReturn (that remains available but not within the ZArray)
 * @warning Not Thread safe.
 *  @return a reference to poped (removed) element content
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::_popRP_nolock(_Tp* pReturn)
{
  if (_Base::size() < 1) {
    fprintf (stderr,"_popRP_nolock-F-INVZCURRNB Fatal error : index of ZArray <%ld> is < 1 \n",
        _Base::size());
    abort();
  }
  return _Base::takeLast();
} // _popRP_nolock


/** @brief pop erases the last element for the ZArray. Returns the new array size or -1 if empty.
 *
 * @return new array size after pop operation or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop(void)
{
  if (_Base::size() < 1) {
    return -1;
  }
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  _pop_nolock();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (_Base::size());
} // pop

/** @brief ZArray<_Tp>::_pop_nolock same as pop() but with no mutex lock */
template <typename _Tp>
long ZArray<_Tp>::_pop_nolock(void)
{
  if (_Base::size()<1) {
    return -1;
  }
  _Base::removeLast();

  return (_Base::size());
} // _pop_nolock


/** @brief pop_front Erases the first element. Returns the new array size or -1 if empty.
 * @return new array size or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop_front(void)
{
  if (_Base::size()<1) {
      return -1;
    }

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _pop_front_nolock();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (_Base::size());
} // pop_front

template <typename _Tp>
long ZArray<_Tp>::_pop_front_nolock(void)
{
  if (_Base::size()<1) {
    return -1;
  }
  _Base::removeAt(0);

  return (_Base::size());
} // _pop_front_nolock

/** @brief popR_front NOT ThreadSafe: Pop with return. Pops out the first array element and returns a pointer to the poped element's content.
 *                  returns NULL if array is empty.
 *
 *  WARNING : Not Thread safe.
 *
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popR_front(void)

{
  if (_Base::size()<1) {
    fprintf (stderr,"ZArray<_Tp>::popR_front-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
    abort();
  }

#ifdef __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
  ZReturn = _Base::takeFirst();

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZReturn); // bad thing when being used with ZThreads
} // popR_front

template <typename _Tp>
_Tp &ZArray<_Tp>::_popR_front_nolock(void)

{
  if (_Base::size()<1) {
    fprintf (stderr,"ZArray<_Tp>::_popR_front_nolock-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
    abort();
  }
  return _Base::takeFirst();
} // _popR_front_nolock


/** @brief popRP_front ThreadSafe: Pops out the first array element and returns a pointer to the poped element's content.
 *                  returns NULL if array is empty.
 *
 *
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP_front(_Tp*pReturn)
{
  if (_Base::size()<1) {
    fprintf (stderr,"ZArray<_Tp>::popRP_front-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
    abort();
  }

#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  _popRP_front_nolock(pReturn);
#ifdef __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
    return (*pReturn);
} // popRP_front

template <typename _Tp>
_Tp &ZArray<_Tp>::_popRP_front_nolock(_Tp*pReturn)
{
  if (_Base::size()<1) {
    fprintf (stderr,"ZArray<_Tp>::_popRP_front_nolock-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
    abort();
  }

  *pReturn = _Base::takeFirst();

  return (*pReturn);
} // _popRP_front_nolock

template <typename _Tp>
long
ZArray<_Tp>::searchforValue (long pOffset, long pSize, const void *pValue) //! sequential search (costly) for a value
{
  for (long wi=0;wi < _Base::size();wi++) {
      if (memcmp(&_Base::at(wi)+pOffset,pValue,pSize)==0)
                              return(wi);
  }
  return (-1);
}// _searchforValue


//
//-------------Functions---------------
//

//=============Export Import==================================

/**
 * @brief ZAexport Exports to a flat memory zone the content of a ZArray corresponding to its Allocated space (including elements that are not yet used),
 *                 including its descriptive header ZAExport.
 *  This routine allocates the required memory size to export the data.
 * @warning The allocated memory has to be freed later on. This is the responsibility of the caller to free this memory buffer.
 *  NB: at this stage ZDataBuffer is not supposed to be defined. This is the reason why memory is allocated and needs to be freed.
 * @param|out] pOutBuffer  ZArray exported data including ZAExport header
 * @param[out] pBufferSize The total size to exported ZArray in a flat format (including free space at the end of ZArray)
 * @param[in]  _exportConvert conversion routine (Endian) for exporting/importing one element of type _Tp
 *              first argument is a reference to a _Tp element to convert
 *              second argument is a pointer to a _Tp argument converted by the routine
 * @return -1 in case of memory allocation error, or pBuffer if everything went OK
*/
template <class _TpIn , class _TpOut>
unsigned char*
ZAexport(ZArray<_TpIn>* pZArray,
          unsigned char* &pOutBuffer,
          size_t &pBufferSize,
          _TpOut (*_exportConvert)(_TpIn&,_TpOut*)) // Routine to convert a single element of ZArray
                                                     // first arg  : element to convert
                                                     // second arg : buffer to receive converted element
{
#ifdef __USE_ZTHREAD__
    pZArray->lock();
#endif
    ZAExport wZAE=pZArray->getZAExport();

    wZAE.DataSize =  (wZAE.NbElements * sizeof (_TpOut));
    wZAE.FullSize = sizeof(ZAExport) + wZAE.DataSize;

    pBufferSize=wZAE.FullSize;
    pOutBuffer = (unsigned char*)malloc(wZAE.FullSize);
    memset (pOutBuffer,0,wZAE.FullSize);

    if (pOutBuffer==nullptr){
      fprintf(stderr,"ZArray::_export> ******Memory allocation error****\n");
      abort();
    }

    wZAE.serialize();
    memmove(pOutBuffer,&wZAE,sizeof(ZAExport));

    _TpOut* wEltPtr=(_TpOut*)(pOutBuffer+sizeof(ZAExport));
    for (long wi=0;wi<pZArray->size();wi++) {
      _exportConvert(pZArray->Tab(wi),&wEltPtr[wi]);
    }

#ifdef __USE_ZTHREAD__
    pZArray->unlock();
#endif

    return pOutBuffer;
}//_exportAllocated

template <class _Tp>
void ZAExport::setZArray(ZArray<_Tp>* pZArray)
{
  pZArray->clear();
  pZArray->setAllocation(AllocatedElements,false);         // no lock

  pZArray->setQuota(ExtentQuota);
  pZArray->setInitialAllocation(InitialAllocation,false);  // no lock
  if (NbElements>(ssize_t)AllocatedElements)
  {
    pZArray->setAllocation(NbElements,false);
  }
//  pZArray->newBlankElement(NbElements);
}



/* for export with ZDataBuffer as result see zdatabuffer.h -> template ZAexportCurrentZDB()

*/
template <class _TpIn , class _TpOut>
/**
 * @brief ZAimport Imports a ZArray from a flat ZArray content. Sets up all ZArrays characteristics and imports data from a flat content.
 * Nota Bene : input pointer is updated after import operation to point on the first byte after imported data.
 * _TpIn is data structure with export format (not aligned : see #pragma pack() clauses)
 * _TpOut is data structure with aligned format
 *
 * @param pBuffer a pointer to a flat data memory zone with first leading block ZAExport, giving ZArray parameters and size and pointer to data
 * @return the size of pBuffer consummed per the import operation : how much bytes have been used from pBuffer for importing ZArray.
 *         This is equal to ZAExport::FullSize
 */

static ZStatus ZAimport(ZArray<_TpOut>* pZArray,
                 const unsigned char* &pPtrIn,
                  ssize_t &pSize,
                 _TpOut (*_importConvert)(_TpOut&,_TpIn*),// Routine to convert a single element of ZArray for import
                 // first arg  : element to receive converted data to integrate into ZArray
                 // second arg : input buffer to convert element : format is not aligned struct
                 ZAExport* pZAE=nullptr) // if not nullptr uses given ZAExport block
{


#ifdef __ZTHREAD_AUTOMATIC__
    pZArray->lock();
#endif
    pZArray->clear(false); // clear with no lock
//    pZArray->setChanged();
    ZAExport wZAE;
    wZAE.setFromPtr(pPtrIn); /* pPtrIn is updated */
    wZAE.deserialize();

//    ZAExport* ZAE=pZAE;
    bool wZAECreated=false;
    if (pZAE== nullptr)
            {
//            ZAE = new ZAExport;
            wZAECreated=true;
            }
//    (*ZAE) = pZArray->ZAEimport(pPtrIn);

    if (wZAE.StartSign!=cst_ZMSTART)
      {
      fprintf(stderr,"ZArray::ZAimport-E-CORRUPT cannot find valid start sign <%X> while <%X> is required.\n",wZAE.StartSign,cst_ZMSTART);
      pSize=-1;
      return ZS_CORRUPTED;
      }

    pZArray->setAllocation(wZAE.AllocatedElements,false);         // no lock
//    pZArray->bzero(0,-1,false);                                   // no lock
    pZArray->setQuota(wZAE.ExtentQuota);
    pZArray->setInitialAllocation(wZAE.InitialAllocation,false);  // no lock
    if (wZAE.NbElements>(ssize_t)wZAE.AllocatedElements)
        {
        pZArray->setAllocation(wZAE.NbElements,false);
        }



//    pZArray->newBlankElement(wZAE.NbElements);

    _TpIn* wEltPtr_In=(_TpIn*)(pPtrIn+sizeof(ZAExport));
//    _TpOut* wEltPtr_Tab= pZArray->Tab;
    _TpOut wEltOut;
    for (long wi=0;wi<wZAE.NbElements;wi++)
        {
        _importConvert(wEltOut,&wEltPtr_In[wi]);
        pZArray->push(wEltOut);
        }// for


#ifdef __ZTHREAD_AUTOMATIC__
    pZArray->unlock();
#endif

    pSize = wZAE.FullSize;
    pPtrIn += wZAE.DataSize; // update pointer
    if (pZAE)
      pZAE->_copyFrom( wZAE );
    return ZS_SUCCESS;
}//ZAimport
/*   moved to zdatabuffer.h
template <class _TpIn , class _TpOut>
static size_t ZAimportZDB(ZArray<_TpOut>* pZArray,
                          const ZDataBuffer &pBuffer,
                          _TpOut (*_importConvert)(_TpOut&,_TpIn*))// Routine to convert a single element of ZArray for import
                          // first arg  : element to receive converted data to integrate into ZArray
                          // second arg : input buffer to convert element : format is not aligned struct
{
  return ZAImport(pZArray,pBuffer.Data,_importConvert,nullptr);
}//ZAimportZDB
*/

/**
 *
 */
template <class _Type>
/**
 * @brief _searchforValue
 *                  searches sequentially within an array for a value (sequence of unsigned char ) defined by an offset and a length (bytes).
 * @param pZArray the ZArray to search for
 * @param pOffset offset from the beginning of _Type ZArray record
 * @param pSize   length in bytes of the section to search within _Type ZArray record
 * @param pValue  a void* pointer to the value <further recasted in unsigned char*> to search for
 * @return
 */

long
_searchforValue (ZArray<_Type> &pZArray,long pOffset,long pSize, void *pValue) // to be tested before becoming a standard
{
    unsigned char* wPtrArray;
    unsigned char* wPtrValue = static_cast<unsigned char*>(pValue);
    for (long wi=0;wi<pZArray.size();wi++)
            {
            wPtrArray=(unsigned char *)(&pZArray[wi]);
            wPtrArray += pOffset;
            if (memcmp(wPtrArray,wPtrValue,pSize)==0)
                                    return(wi);
            }
    return (-1);
}// _searchforValue




template <class _Tp>
ZAExport
ZArray<_Tp>::getZAExport(void) const
{
  ZAExport wZAE;
  wZAE.AllocatedSize = getAllocatedSize();   // get the total allocated size
  wZAE.CurrentSize = getCurrentSize();     // get the current size corresponding to currently stored elements

  wZAE.DataSize = 0 ;  // Exported data size : must be defined by ZAExport routine : requires sizeof(_TpOut)*NbElements

  wZAE.FullSize = 0 ; // must be defined by export routine : is sizeof(ZAExport) + DataSize

  wZAE.AllocatedElements = getAllocation();
  wZAE.InitialAllocation = getInitalAllocation();
  wZAE.ExtentQuota = getQuota();
  wZAE.NbElements = _Base::size();
  return wZAE;
}
/**
 * @brief getZAExport_U exports a ZAExport structure with Intermediate format (reversedByteOrder whether necessary)
 * @return
 */
/*template <class _Tp>
ZAExport
ZArray<_Tp>::getZAExport_U(void)
{
  ZAExport wZAE=getZAExport();

  wZAE.AllocatedSize = reverseByteOrder_Conditional<size_t>(wZAE.AllocatedSize);   // get the total allocated size
  wZAE.CurrentSize = reverseByteOrder_Conditional<size_t>(wZAE.CurrentSize);     // get the current size corresponding to currently stored elements

  wZAE.DataSize = reverseByteOrder_Conditional<size_t>(wZAE.DataSize);     // taking the AllocatedSize

  wZAE.FullSize = reverseByteOrder_Conditional<size_t>(wZAE.FullSize);
  wZAE.AllocatedElements = reverseByteOrder_Conditional<size_t>(wZAE.AllocatedElements);
  wZAE.ExtentQuota = reverseByteOrder_Conditional<size_t>(wZAE.ExtentQuota);
  wZAE.InitialAllocation = reverseByteOrder_Conditional<size_t>(wZAE.InitialAllocation);
  wZAE.NbElements = reverseByteOrder_Conditional<ssize_t> (wZAE.NbElements);
  wZAE.DataOffset = reverseByteOrder_Conditional<size_t> (wZAE.DataOffset);
  return wZAE;
}

template <class _Tp>
ZAExport&
ZArray<_Tp>::convertZAExport_I(ZAExport& pZAE)
{
  pZAE.AllocatedSize = reverseByteOrder_Conditional<size_t>(pZAE.AllocatedSize);   // get the total allocated size
  pZAE.CurrentSize = reverseByteOrder_Conditional<size_t>(pZAE.CurrentSize);     // get the current size corresponding to currently stored elements

  pZAE.DataSize = reverseByteOrder_Conditional<size_t>(pZAE.DataSize);     // taking the AllocatedSize

  pZAE.FullSize = reverseByteOrder_Conditional<size_t>(pZAE.FullSize);
  pZAE.AllocatedElements = reverseByteOrder_Conditional<size_t>(pZAE.AllocatedElements);
  pZAE.ExtentQuota = reverseByteOrder_Conditional<size_t>(pZAE.ExtentQuota);
  pZAE.InitialAllocation = reverseByteOrder_Conditional<size_t>(pZAE.InitialAllocation);
  pZAE.NbElements = reverseByteOrder_Conditional<ssize_t> (pZAE.NbElements);
  pZAE.DataOffset = reverseByteOrder_Conditional<size_t> (pZAE.DataOffset);
  return pZAE;
}
*/
ZAExport ZAEimport(const unsigned char* pBuffer);



template <typename _Tp>
class ZQueue : protected ZArray<_Tp>
{
public:
typedef ZArray<_Tp> _Base;
    ZQueue (size_t pInitialAlloc=_cst_default_allocation,
                   size_t pReallocQuota=_cst_realloc_quota):ZArray<_Tp>(pInitialAlloc,pReallocQuota) {}
    ~ZQueue() {}

    using _Base::push;
    using _Base::pop;
    using _Base::popR;
    using _Base::size;
    using _Base::last;
    using _Base::isEmpty;

    using _Base::searchforValue;
    using _Base::search;

    using _Base::operator << ;
    using _Base::operator -- ;
    using _Base::operator [] ;

};


template <class _Tp>
ZArray<_Tp>& operator += (ZArray<_Tp>& pFirst,ZArray<_Tp>& pSecond )
{
    for (long wi=0;wi<pSecond.size();wi++)
            pFirst << pSecond[wi];
}

template <class _Tp>
ZArray<_Tp>& operator += (ZArray<_Tp>& pFirst,ZArray<_Tp>&& pSecond )
{
    for (long wi=0;wi<pSecond.size();wi++)
            pFirst << pSecond[wi];
}


template <class _Tp>
ZArray<_Tp>* operator << (ZArray<_Tp>* pFirst,_Tp&& pSecond )
{
                pFirst->push( pSecond);

    pFirst->push(pSecond);
    return pFirst;
}


} // namespace zsb
#endif // ZARRAY_H
