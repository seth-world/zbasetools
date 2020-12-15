#ifndef ZARRAY_H
#define ZARRAY_H

#include <zconfig.h>

#include <ztoolset/zmem.h>  // for _free()

#include <ztoolset/zerror_min.h>
#ifdef __USE_ZTHREAD__
    #include <zthread/zmutex.h>
#endif

//extern ZVerbose_Base ZVerbose;

/*
 *   ===========WARNING :   Do not include zerror.h in this include file  ========================
 *                         Do not use ZException in this include file
 */
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <ztoolset/zlimit.h>

#include <ztoolset/zatomicconvert.h>
//#include <ztoolset/ztoolset_common.h>
#include <initializer_list>



namespace zbs {
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
#pragma pack(0)
    struct ZAExport {
        size_t FullSize;        //!< full size in byte of the export (ZAExport + ZArray data space )
        size_t AllocatedSize;   //!< ZArray Allocated Size
        size_t CurrentSize;     //!< ZArray CurrentSize : current number of elements * size of one element
        size_t DataSize;        //!< Size of ZArray data to import
        size_t AllocatedElements; //!< ZArray Number of allocated elements
        size_t ExtentQuota;     //!< ZArray ExtentQuota value
        size_t InitialAllocation;
        ssize_t NbElements;     //!< ZArray NbElements value
        size_t DataOffset;      //!< Offset to ZArray Data relative to begining of export data (starting 0)
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
 * 3.       name.Data.Tab[index]
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
 *      - name->Data.Tab[index]
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
template<class _Tp> class ZArray;


template <class _TpIn , class _TpOut>
unsigned char*
_ZAexportAllocated(ZArray<_TpIn>* pZArray,
                 unsigned char* &pBuffer,
                 size_t &pBufferSize,
                 _TpOut (*_exportConvert)(_TpIn&,_TpOut*));

typedef long ZArrayIndex_type;

template <typename _Tp>
class ZArray
{
public:

//    ZArray(){_mInit(_cst_default_allocation,_cst_realloc_quota);}
   ZArray (size_t pInitialAlloc=_cst_default_allocation,
           size_t pReallocQuota=_cst_realloc_quota)
                {_mInit(pInitialAlloc,pReallocQuota);}

//   ZArray(size_t pCount,_Tp pInitValue);

   ~ZArray();

 //  ZArray(ZArray &) = delete;                // can not copy
 //  ZArray& operator = (ZArray&)=delete;     // can not copy
//#ifdef __USE_ZARRAY_COPY_CONTRUCTOR__

   ZArray(ZArray& pIn)
   {
//       _mInit(); /* init with default allocation values */
       _cloneFrom(pIn);
       return;
   }
   ZArray& operator = (const ZArray&pIn)
    {
//       _mInit(); /* init with default allocation values */
       _cloneFrom(pIn);
       return *this;
    }
   ZArray& operator = (const ZArray&&pIn)
   {
//      _mInit(); /* init with default allocation values */
      _cloneFrom(pIn);
      return *this;
   }
/*
   void _free(void* pPtr) // local to ZArray
   {
       if (pPtr!=nullptr)
               free(pPtr);
       pPtr=nullptr;
       return;
   }
*/
   bool exists(long pIdx){if ((pIdx<0)||(pIdx>lastIdx())) return false; return true;}

//#else
#ifdef __COMMENT__
   ZArray(ZArray &) = delete;                // can not copy
   ZArray& operator = (ZArray&)=delete;     // can not copy
#endif
    void _mInit(size_t pInitialAlloc=_cst_default_allocation,
                size_t pReallocQuota=_cst_realloc_quota);




   ZArray(_Tp const& __a) _GLIBCXX_NOEXCEPT
   {
       _mInit();
       push(__a);
   }


   ZArray(std::initializer_list<_Tp> l)
   {
       _mInit(); /* initialize with default allocation values */
     const _Tp*       it  = l.begin();  // raw pointer!
     const _Tp* const end = l.end();    // raw pointer!

     for (; it != end; ++it)
       this->push(*it);
   }

   void addValues(size_t pCount,_Tp pInitValue);

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
       const _Tp*       it  = __l.begin();  // raw pointer!
       const _Tp* const end = __l.end();    // raw pointer!

       for (; it != end; ++it)
         this->push(*it);
    return *this;
   }

   _Tp* data() _GLIBCXX_NOEXCEPT
   {return Tab;}

   const _Tp* data() const _GLIBCXX_NOEXCEPT
   {return Tab;}
/*==============================operators overload===========================================
 *      Overloaded index operator [] for simplification purposes & direct access to data elements
 */

   /** operators to be overloaded for ZIndex */

   /**
    * @brief operator [] gives direct access to ZArray row data using its index rank. \n
    *                   this operator is overloaded for some child to return a constant reference in order to prevent modifying ZArray content by this mean.
    */
  _Tp & operator [] (const long pIndex)
            {
            if (pIndex<ZCurrentNb)
                    return (Data.Tab[pIndex]);
            fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
            }// we must be able to access to this operator function from the inherited class
  const _Tp & operator []  (const long pIndex) const { return (Data.Tab[pIndex]);}   // we must be able to access to this operator function from the inherited class

  /**
    * @brief operator << pops the ZArray and returns a reference to the result within the field of type _Tp mentionned as left argument
    * @return
    */
   friend _Tp& operator << (_Tp &pRes,ZArray<_Tp> &pZA)
        {
        return(pZA.popRP(&pRes));
        }
   /**
     * @brief operator << alias for push() method.
     * @return rank of the element that have been pushed
     */
   long operator << (const _Tp pInput)
        {
        return push_back(pInput);
        }


//   friend  ZArray<_Tp>* operator << (ZArray<_Tp>* pFirst,_Tp&& pSecond );

   /**
    * @brief operator -- alias for pop() method
    * @return size of array after pop operationn or -1 if array was empty
    */
   long operator -- (void) {return(pop());}

   /** end operator to be overloaded for ZIndex */
/*=================================================================
 *      Methods to be protected when using ZSortedArray
 */

    long push(_Tp &pElement) ;
//    long push(const _Tp& pElement);
    long push(  _Tp&& pElement);

    /** @brief push_back Alias for push() */
    long push_back (_Tp pElement) {return push(pElement);}

    long push_front (_Tp pElement) ;

 /*   long push_front (const _Tp pElement) {
                    const _Tp &wElement=pElement;
                    return(push_front(wElement)); };
*/
    long move (size_t pDest, size_t pOrig,size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)
private:
    long _move (size_t pDest, size_t pOrig,size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)
public:

    long swap (size_t pDest, size_t pOrig,  size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)

    long insert (const _Tp &pElement, size_t pIdx);
    long insert (_Tp* pElement,size_t pIdx) {return(insert((const _Tp&)(*pElement),pIdx));}
    long insert (_Tp *pElement, size_t pIdx, size_t pNumber);
    long erase(size_t pIdx,size_t pNumber=1);

    long _insertNoLock(_Tp *pElement, size_t pIdx, size_t pNumber=1);
    long _eraseNoLock(size_t pIdx,size_t pNumber=1);

    void trim (void);       //!< @brief trim adjusts the memory size of the ZArray to the current contained elements + realloc quota
    /** ! @brief last returns the ZArray content of the last element of the ZArray. If ZArray is empty, it issues a fatal error and terminates.
     */
    _Tp &last(void) { if (size()>0)
                            return(Data.Tab[ZCurrentNb-1]);
                       fprintf(stderr,"ZArray::last-F-OUTOFBOUND empty ZArray, Out of boundaries\n");
                       abort() ;
                    }

  //! @brief ZArray::lastIdx returns the index for the last ZArray element : WARNING:  when ZArray is empty, returns -1
    long lastIdx (void) {return (isEmpty()?-1:(ZCurrentNb-1));}


//========================End methods to be protected=============

    long pop(void);
    long _pop(void);
    long pop_front(void);

    _Tp& popR(void);
    _Tp& popR_front(void);
    _Tp& popRP_NL(_Tp* pReturn);



    _Tp & popRP(_Tp* pReturn);
    _Tp & popRP_front(_Tp* pReturn);



    long newBlankElement(size_t pNumber=1, bool pLock=false, void (*pClear)(_Tp &)=nullptr);

    bool isEmpty(void) {return (ZCurrentNb==0);}

    _Tp & reverse (long pIdx);

    size_t bzero (size_t pOrig=0, long pNumber=-1, bool pLock=true);
    void reset(void);
    virtual void clear(bool pLock=true);

    ZArray<_Tp>* clone(void);

 //   void copy(ZArray<_Tp> &pZCopied)  {copy(static_cast<const ZArray<_Tp>> (pZCopied));}
    void _cloneFrom(const ZArray<_Tp> &pZCopied)  ;


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

/**
 * @brief _getExportAllocatedSize returns the total size that will be required for an _exportAllocated operation of the current ZArray
 * @return
 */
    ssize_t _getExportAllocatedSize(void) {return (getAllocatedSize()+sizeof(ZAExport)+1);}

/**
 * @brief _getExportCurrentSize returns the total size that will be required for an _exportAllocated operation of the current ZArray
 * @return
 */
    ssize_t _getExportCurrentSize(void) {return (getCurrentSize()+sizeof(ZAExport)+1);}


ZAExport getZAExport(void)
{
    ZAExport wZAE;
    wZAE.AllocatedSize = getAllocatedSize();   // get the total allocated size
    wZAE.CurrentSize = getCurrentSize();     // get the current size corresponding to currently stored elements

    wZAE.DataSize = wZAE.AllocatedSize;     // taking the AllocatedSize

    wZAE.FullSize = wZAE.DataSize+sizeof(ZAExport)+1;

    wZAE.AllocatedElements = getAllocation();
    wZAE.InitialAllocation = getInitalAllocation();
    wZAE.ExtentQuota = getQuota();
    wZAE.NbElements = ZCurrentNb;
    wZAE.DataOffset = sizeof(wZAE);
    return wZAE;
}
/**
 * @brief getZAExport_U exports a ZAExport structure with Intermediate format (reversedByteOrder whether necessary)
 * @return
 */
ZAExport getZAExport_U(void)
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
ZAExport& convertZAExport_I(ZAExport& pZAE)
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
ZAExport ZAEimport(unsigned char* pBuffer)
{
    ZAExport wZAE;
    ZAExport* pZAE=(ZAExport*)pBuffer;
    wZAE.AllocatedSize = reverseByteOrder_Conditional<size_t>(pZAE->AllocatedSize);   // get the total allocated size
    wZAE.CurrentSize = reverseByteOrder_Conditional<size_t>(pZAE->CurrentSize);     // get the current size corresponding to currently stored elements

    wZAE.DataSize = reverseByteOrder_Conditional<size_t>(pZAE->DataSize);     // taking the AllocatedSize

    wZAE.FullSize = reverseByteOrder_Conditional<size_t>(pZAE->FullSize);
    wZAE.AllocatedElements = reverseByteOrder_Conditional<size_t>(pZAE->AllocatedElements);
    wZAE.ExtentQuota = reverseByteOrder_Conditional<size_t>(pZAE->ExtentQuota);
    wZAE.InitialAllocation = reverseByteOrder_Conditional<size_t>(pZAE->InitialAllocation);
    wZAE.NbElements = reverseByteOrder_Conditional<ssize_t> (pZAE->NbElements);
    wZAE.DataOffset = reverseByteOrder_Conditional<size_t> (pZAE->DataOffset);

    return wZAE;
}

    size_t sizeAllocated(void) ;
    /** @brief size  size of current ZArray : number of element in the current ZArray (and NOT bytes size) see getAllocatedSize*/
    long size(void) const ;
    /** @brief count  size of current ZArray : number of element in the current ZArray (and NOT bytes size) see getAllocatedSize*/
    long count(void) const;
    /** @brief elementSize  returns the size in bytes of one _Tp element i.e. the size of one row fo the array */
    long elementSize(void) ;
    /** @brief usedSize  returns the size in bytes of one _Tp element (size of one row of the array). Alias for getCurrentSize()*/
    long usedSize(void) ;

    size_t getAllocation (void) {return (ZAllocation);} /*!< returns the number of elements currently being allocated (and not those used) */
    size_t getQuota(void) {return (ZReallocQuota);} /*!< returns the reallocation quota for the ZArray : Number of elements that will be allocated each time ZArray needs additional space */
    size_t getInitalAllocation() {return ZInitialAllocation;}
    /** @brief getAllocatedSize returns the size in bytes of the memory currently allocated to the ZArray instance */
    size_t getAllocatedSize(void){return(ZAllocatedSize);}
    /** @brief  getCurrentSize returns the size in bytes taken by current number of elements */
    ssize_t getCurrentSize(void) { return (sizeof(_Tp)*ZCurrentNb);}

    void setAllocationValues(size_t pAllocation,size_t pReallocQuota) {setAllocation(pAllocation); setQuota(pReallocQuota);}
    inline void setQuota (size_t pQuota) {ZReallocQuota=pQuota;} /** !< sets the new reallocation quota for the ZArray in number of elements */
    void setAllocation (size_t pAlloc, bool pLock=true);
    void setInitialAllocation(size_t pInitialAllocation,bool pLock=true);
    long allocateCurrentElements (const ssize_t pAlloc);

    /** @brief getDataPtr returns the current pointer to data content. Warning : this pointer is not stable and is to be changed whenever reallocation occurs */
    inline void* getDataPtr() {return Data.zptr_void;}

//------------- search routines---------------------------

    long bsearchCurrentIdx;

    long bsearchCaseRegardless (const void *pKey, size_t pSize, size_t pOffset);  //!< @brief ZArray::bsearch binary search BUT when it founds a character compares CASE REGARDLESS. returns an offset from pOffset within the whole ZArray considered as a bulk of bytes
    long searchCaseRegardless (const void *pKey, size_t pSize, long pIdx);
    long bsearch (const void *pKey,size_t pSize,size_t pOffset=0);                //!< @brief ZArray::bsearch binary search. returns an offset from pOffset within the whole ZArray considered as a bulk of bytes
    long getifo (long pOffset);                                             //!< @brief ZArray::getifo get index from offset : converts the offset to a ZArray index

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


//---------Mutex methods---------------
public:
#ifdef __USE_ZTHREAD__
    void lock(void)     {_Mutex.lock();}
    void unlock(void)   {_Mutex.unlock();}
#endif  //__USE_ZTHREAD__

private :
    void addCheck(long pAdd);
    void allocate(long pAlloc) ;
    void _sizeAllocate(size_t pSizeAlloc);
    void _setContent (void *pContent,size_t pSize) {memmove(Data,pContent,pSize); return;}
/*
 *  Data Attributes
 */

public :
    union ZUData { char *zptr_char;
                   void *zptr_void;
                    _Tp *Tab ;
                 } Data ;

    _Tp * Tab=nullptr  ;

    ssize_t ZCurrentNb ;        // current number of elements : 0 if ZArray is empty lastIdx() returns -1


protected:
    size_t ZAllocation ;        // allocated number of elements
    size_t ZAllocatedSize ;     // allocated size in bytes (excepted one char of highvalue mark )
    size_t ZReallocQuota ;      // reallocation quota in number of elements to reallocate each reallocation operation
    size_t ZInitialAllocation;  // keep initial allocation for reset purpose : ZClear will get back to this memory allocation
    _Tp ZReturn ;               // memory zone for returning element (WARNING: not to be used with multi-thread)

private :
    void *ZPtr =nullptr;
    long ZIdx=0 ;                 // Last index accessed

#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
    ZMutex _Mutex ;
#endif

}; // class ZArray

/**
 * operator << overload that cannot be put as friend within class due to template limitations
 */
#ifndef __OPERATOR_ZARRAY_PUSH_OVERLOAD__
#define __OPERATOR_ZARRAY_PUSH_OVERLOAD__

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
/*
template <typename _Tp>
ZArray<_Tp>::ZArray()
{

    return;
}*/
template <typename _Tp>
void ZArray<_Tp>::addValues(size_t pCount,_Tp pInitValue)
{
    while (pCount--)
            push(pInitValue);
    return;
}
#ifdef __COMMENT__
template <typename _Tp>
ZArray<_Tp>::ZArray(size_t pInitialAlloc, size_t pReallocQuota)
{
    _mInit(pInitialAlloc,pReallocQuota);
/*    ZPtr=NULL ;
    Data.zptr_void = NULL;
    Tab = NULL;
    ZAllocation = 0;

    ZInitialAllocation=pInitialAlloc ;
    ZReallocQuota = pReallocQuota ;
    ZCurrentNb = 0 ;  // Empty ZArray : size() == 0

    allocate (ZInitialAllocation);
    memset (Tab,0,ZAllocatedSize);

    ZIdx=0;*/
/*
#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
    _Mtx = new ZMutex;
#endif
*/
    return ;
}
#endif // __COMMENT__

template <typename _Tp>
void ZArray<_Tp>::_mInit(size_t pInitialAlloc,
                         size_t pReallocQuota)
{
    _free(ZPtr);

    ZPtr=nullptr ;
    Data.zptr_void = nullptr;
    Tab = nullptr;
    ZAllocation = 0;

    ZInitialAllocation=pInitialAlloc ;
    ZReallocQuota = pReallocQuota ;
    ZCurrentNb = 0 ;  // Empty ZArray : size() == 0

    allocate (ZInitialAllocation);
    memset (Tab,0,ZAllocatedSize);

    ZIdx=0;
/*
#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
    _Mtx = new ZMutex;
#endif
*/
}

template <typename _Tp> ZArray<_Tp>::~ZArray()
{
//#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
//   _Mutex.lock();  // cannot do that : mutex object already deleted
//    delete _Mtx;
//#endif
    if (ZPtr!=NULL)
            {
            free (ZPtr);
            ZPtr=NULL;
            }

// #ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
//    _Mutex.unlockBesttry();
//    delete _Mtx;
//#endif
}//DTOR


template <typename _Tp>
/**
 * @brief ZArray<_Tp>::clone returns a pointer to a newly allocated ZArray that is a strict clone (structure and content) of the current ZArray.
 *                           the cloned ZArray MUST be deleted by user to free memory.
 */
ZArray<_Tp>*
ZArray<_Tp>::clone(void)
{
    ZArray<_Tp> *wRet= new ZArray<_Tp>(ZAllocation,ZReallocQuota);
    memmove (wRet->Data.zptr_void,Data.zptr_void,ZAllocatedSize);
    wRet->ZCurrentNb = ZCurrentNb;
    wRet->ZIdx=ZIdx;
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
void
ZArray<_Tp>::_cloneFrom (const ZArray<_Tp> &pCopied)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
//        pCopied.lock(
    const ZArray<_Tp>* wC=&pCopied;
    const_cast<ZArray<_Tp>*>(wC)->lock();
#endif

    this->clear(false);
    this->setQuota(pCopied.ZReallocQuota);
    this->allocate(pCopied.ZAllocation);
    this->ZCurrentNb = pCopied.ZCurrentNb ;
//    memmove (Tab,pCopied.Tab,ZAllocatedSize);
    memset (Tab,0,ZAllocatedSize);
    memmove (Tab,pCopied.Tab,(pCopied.ZCurrentNb*sizeof(_Tp)));

#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
//        pCopied.unlock();
        const_cast<ZArray<_Tp>*>(wC)->unlock();
#endif

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
    if ((pIdx <0)||(pIdx>=ZCurrentNb))
                        {
                        fprintf (stderr,"ZArray::reverse-F-outofboundaries Fatal error : given index <%ld> is out of ZArray boundaries\n",pIdx);
                        std::cerr.flush();
                        }
#endif
    return (Data.Tab[ZCurrentNb-pIdx-1]);
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
template <typename _Tp>
long ZArray<_Tp>::move (size_t pDest, size_t pOrig, size_t pNumber)
{

long wRet=0;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
        wRet=_move(pDest,pOrig,pNumber);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return wRet ;
}// move

template <typename _Tp>
long ZArray<_Tp>::_move (size_t pDest, size_t pOrig, size_t pNumber)
{
    if (pDest==pOrig)
            {
            ZIdx = pDest;
            return (0);
            }
 //   if ((pOrig<0)||(pDest<0))
 //                               {
 //                               return -1 ;  // pOrig or pDest are out of boundaries
 //                               }
    if (ZCurrentNb==0)
            {
            if (pOrig>ZCurrentNb)
                    {
                    return -1; // pOrig is out of boundaries
                    }
            }
            else
            {
            if (pOrig+pNumber > ZCurrentNb)
                        {
                        return -1 ; // source is out of boundaries
                        }
            } // else

    if ((pDest+pNumber)>ZCurrentNb)
                {
                long wA=(pDest+pNumber)-ZCurrentNb ;
                addCheck(wA);                       // make room for moving elements if necessary
//                ZCurrentNb=ZCurrentNb + wA ;        // new current number of ZArray elements
                }

    memmove(&Data.Tab[pDest],&Data.Tab[pOrig],(size_t)(pNumber*(sizeof(_Tp))));
    ZIdx = pDest ;

    return pDest ;
}// _move


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
    if (pDest==pOrig)
            {
            return pDest;
            }
//    if (((pOrig+pNumber)>lastIdx())||(pOrig<0)||(pDest<0)||(pDest>lastIdx()))
        if (((pOrig+pNumber)>lastIdx())||(pDest>lastIdx())) // pOrig & pDest are unsigned longs so a negative longs will produce an outofboundaries error nevertheless
                            {
                            #if __DEBUG_LEVEL__ > 0
                                fprintf (stderr, "ZArray::swap-E-OUTBOUND Error, given index <%ld> plus number of elements to insert <%ld> are out of ZArray boundaries. Last index is <%ld>\n"
                                                 "                        OR destination index <%ld> is out of ZArray boundaries.\n",
                                         pOrig,
                                         pNumber,
                                         lastIdx(),
                                         pDest
                                         );
                            #endif
                            #if __CRASH_ON_FATAL__
                                    _ABORT__ ;
                            #endif
                            return -1 ; // out of boundaries for
                            }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
        if ((pDest+pNumber)>ZCurrentNb)
                {
                addCheck((pDest+pNumber)-ZCurrentNb);
                memset (&Data.Tab[ZCurrentNb],0,(size_t)(pNumber-(ZAllocation-ZCurrentNb)));
                }

    _Tp *wPt=(_Tp*) malloc (pNumber*(sizeof(_Tp)));
    memmove(wPt,&Data.Tab[pOrig],(size_t)(pNumber*(sizeof(_Tp))));
    memmove(&Data.Tab[pOrig],&Data.Tab[pDest],(size_t)(pNumber*(sizeof(_Tp))));
    memmove(&Data.Tab[pDest],wPt,(size_t)(pNumber*(sizeof(_Tp))));
    free(wPt);

    ZIdx = pDest ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
long ZArray<_Tp>::insert(const _Tp &pElement, size_t pIdx)
{
    if (pIdx>ZCurrentNb)
                {
                return (-1);
                }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1);
    if (ZCurrentNb>0)
            {
            move((pIdx+1),pIdx,(ZCurrentNb-pIdx)); // make room for element
            }

    ZCurrentNb=ZCurrentNb + 1;
    memmove(&Data.Tab[pIdx],&pElement,(sizeof(_Tp)));
    ZIdx=pIdx;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
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
    if (pIdx>ZCurrentNb)
                {
                return (-1);
                }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    ZIdx = pIdx ;
    addCheck(pNumber);
    if (ZCurrentNb>0)
            {
//            ZCurrentNb=ZCurrentNb + pNumber;  // done in 'move'
            move((pIdx+pNumber),pIdx,(ZCurrentNb-pIdx)); // make room for element
            }
//            else
            ZCurrentNb=ZCurrentNb + pNumber;  // done in 'move'
    memmove(&Data.Tab[pIdx],pElement,(sizeof(_Tp)*pNumber));
    ZIdx=pIdx;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
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
long ZArray<_Tp>::_insertNoLock(_Tp *pElement, size_t pIdx,size_t pNumber)
{
    if (pIdx>ZCurrentNb)
                {
                return (-1);
                }

    ZIdx = pIdx ;
    addCheck(pNumber);
    if (ZCurrentNb>0)
            {
//            ZCurrentNb=ZCurrentNb + pNumber;  // done in 'move'
            move((pIdx+pNumber),pIdx,(ZCurrentNb-pIdx)); // make room for element
            }
//            else
            ZCurrentNb=ZCurrentNb + pNumber;  // done in 'move'
    memmove(&Data.Tab[pIdx],pElement,(sizeof(_Tp)*pNumber));
    ZIdx=pIdx;
    return (pIdx);
}// insertNoLock
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

    if ((long)(pIdx+pNumber)>ZCurrentNb)
                {
                return (-1);
                }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    ZIdx = pIdx ;
    _move(pIdx,pIdx+pNumber,(ZCurrentNb-pIdx-pNumber));

_Tp* wPtr = &last()-(sizeof(_Tp)*(pNumber-1));
    memset(wPtr,0,sizeof(_Tp));  // set available space to zero in the End of ZArray (recuperated space)

    ZCurrentNb = ZCurrentNb -pNumber ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (pIdx);
}// insert

template <typename _Tp>
long ZArray<_Tp>::_eraseNoLock(size_t pIdx,size_t pNumber)
{
    if ((long)(pIdx+pNumber)>ZCurrentNb)
                {
                return (-1);
                }
    ZIdx = pIdx ;
    _move(pIdx,pIdx+pNumber,(ZCurrentNb-pIdx-pNumber));

_Tp* wPtr = &last()-(sizeof(_Tp)*(pNumber-1));
    memset(wPtr,0,sizeof(_Tp));  // set available space to zero in the End of ZArray (recuperated space)

    ZCurrentNb = ZCurrentNb -pNumber ;
    return (pIdx);
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::bzero highwater marking (sets to binary zero) of array elements starting at pOrig position for pNumber elements (origin is zero).
 * if pNumber elements is omitted or is mentionned with value -1, then all elements from pOrig until end of array are zeroed.
 * if pOrig is omitted (-1) then start of the array is retained.
 * @warning pOrig and pNumber are NOT sizes but element ranks
 *
 * @param[in] pOrig   rank of element to start zeroing (including this one)
 * @param[in] pNumber number of elements to zero
 * @return pOrig (>=0) or -1 if operation failed
 */
size_t ZArray<_Tp>::bzero (size_t pOrig, long pNumber,bool pLock)
{
    long wN=pNumber ;
    if (wN<0)
            wN=ZCurrentNb-pOrig ;
    if ((size_t)(pOrig+wN)>ZCurrentNb)
                            return -1 ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.lock();
#endif

    memset(&Data.Tab[pOrig],0,(size_t)(wN*(sizeof(_Tp))));
    ZIdx = pOrig ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.unlock();
#endif
    return pOrig ;
}// swap


template <typename _Tp>
/**
 * @brief ZArray<_Tp>::clear reset the ZArray to the minimum memory(ZInitialAllocation), resets the number of elements to zero, set the allocated surface to zero.
 */
void ZArray<_Tp>::clear(bool pLock)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.lock();
#endif

    memset (ZPtr,0,ZAllocatedSize);
    ZCurrentNb=0 ;
    ZIdx = 0;
    allocate (ZInitialAllocation);
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    if (!(ZCurrentNb<0))
            {
            ZCurrentNb=0 ;
            ZIdx = 0;
            }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
    return (ZCurrentNb*sizeof(_Tp));
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

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::size returns the current number of elements of the ZArray (not the last rank)\n
 *  Alias for count()
 * @return
 */
inline long ZArray<_Tp>::size(void) const
{
    return (ZCurrentNb);
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::count returns the current number of elements of the ZArray (not the last rank)
 * @return
 */
inline long ZArray<_Tp>::count(void) const
{
    return (ZCurrentNb);
}


template <typename _Tp>
/**
 * @brief ZArray<_Tp>::sizeAllocated returns the size in bytes of the memory currently allocated to the ZArray instance
 * @return
 */
inline size_t ZArray<_Tp>::sizeAllocated(void)
{
    return (ZAllocatedSize);
}

/** @brief addCheck
 *  Controls impact of adding pAdd Elements on the existing current context
 *  Takes the appropriate reallocation actions :
 *   - if remaining not used elements are sufficient : nothing
 *   - allocate ZReallocateQuota elements if Quota is sufficient
 *   - allocate the appropriate number of elements + 1 in other case
 */
template <typename _Tp>
void ZArray<_Tp>::addCheck(long pAdd)
{

long wN = ZAllocation-ZCurrentNb  ;  // size remaining to fill
    if (wN > pAdd)
            {
            return ;
            }
long wNN = (wN + (long) ZReallocQuota) - pAdd ;
    if (wNN<=0)
            {
            allocate(ZAllocation+pAdd+1);
            return ;
            }
    allocate (ZAllocation+ZReallocQuota) ;

    return ;
} // ZSizeCheck

/** @brief allocate  dynamic memory allocation / reallocation
 *                  pAlloc is the rough total overall number of array element to be allocated before a reallocQuota memory extension.\n
 *                  And NOT a number of element to add to existing allocation.\n
 *                  Nota Bene: pAlloc CANNOT be less than the existing number of element within the ZArray
 */
template <typename _Tp>
void ZArray<_Tp>::allocate(long pAlloc)
{
#if __DEBUG_LEVEL__  > 4
    if (ZAllocation >= pAlloc )
                {
                fprintf (stderr,"ZArray::allocate-W-Warning requested new allocation <%ld> not greater than existing allocation <%ld> \n",pAlloc,ZAllocation);
                }
#endif // _DEBUG_LEVEL

    if (pAlloc < ZCurrentNb)
                {
#if __DEBUG_LEVEL__  > 1
                fprintf (stderr,"ZArray::allocate-W-ERRALLOC requested new allocation <%ld> not greater than existing number of elements within ZArray <%ld> .\n"
                         "You should suppress elements from the ZArray and use trimming facility to reduce the memory allocated (ZArray::trim).\n",
                         pAlloc,ZCurrentNb);
                return;
#endif // _DEBUG_LEVEL
                }
    ZAllocatedSize = sizeof(_Tp)*pAlloc;
    ZAllocation = pAlloc;

    Data.zptr_void =realloc(ZPtr,(ZAllocatedSize+sizeof(_HighValue)+1));
    if (Data.zptr_void==nullptr)
                {
                fprintf(stderr,"ZArray::allocate-F-MEMERR *** Fatal error : cannot reallocate memory of size %ld\n",
                        ZAllocatedSize+sizeof(_HighValue)+1);
                free (ZPtr); // free former memory block we wanted to extend
                ZPtr=nullptr;
                abort();
                }
    ZPtr= Data.zptr_void ;

    Data.zptr_char[ZAllocatedSize] = _HighValue ;

    Tab = (_Tp *)Data.zptr_void ;

}// ZAllocate

/** @brief allocate  dynamic memory allocation / reallocation allocates memory with a size in bytes given by pSizeAlloc (in place of a number of elements)
 *                  pSizeAlloc is the rough total size in bytes to be allocated before a reallocQuota memory extension.\n
 *
 * @note Number of allocated elements (ZAllocation) is computed to be the truncated value of size to allocate/ size of one element.
 */
template <typename _Tp>
void ZArray<_Tp>::_sizeAllocate(size_t pSizeAlloc)
{

    ZAllocatedSize = pSizeAlloc;
    ZAllocation = pSizeAlloc/sizeof(_Tp);

    Data.zptr_void =realloc(ZPtr,(ZAllocatedSize+sizeof(_HighValue)+1));
    if (Data.zptr_void==nullptr)
                {
                fprintf(stderr,"ZArray::_sizeAllocate-F-MEMERR *** Fatal error : cannot reallocate memory of size %ld\n",
                        ZAllocatedSize+sizeof(_HighValue)+1);
                abort();
                }

    ZPtr= Data.zptr_void ;
    Data.zptr_char[ZAllocatedSize] = _HighValue ;

    Tab = (_Tp *)Data.zptr_void ;

}// ZAllocate

template<typename _Tp>
long ZArray<_Tp>::bsearch (const void *pKey, size_t pSize, size_t pOffset)
{
long widx = 0;
long wistart = -1;
const char *wKey = (const char *)pKey;
long wHighIdx = (long)size()*(long)sizeof(_Tp);

    for (long wi=pOffset; wi <wHighIdx ;wi++)
            {
            if (wKey[widx]==Data.zptr_char[wi])
                    {
                    if (wistart==-1)
                                wistart=wi;
                    widx++;
                    if (widx==pSize)
                        {
                        return (wistart);
                        }
                    continue;
                    }// if wKey
            widx=0;
            wistart=-1;
            }
return (-1) ;
}



template<typename _Tp>
long ZArray<_Tp>::bsearchCaseRegardless (const void *pKey, size_t pSize, size_t pOffset)
{
long widx = 0;
long wistart = -1;
const char *wKey = (const char *)pKey;
long wHighIdx = (long)size()*(long)sizeof(_Tp);
char ToCompareFromKey ;
char ToCompareFromArray;

    for (long wi=pOffset; wi <wHighIdx ;wi++)
            {
            ToCompareFromArray = Data.zptr_char[wi];
            ToCompareFromKey = wKey[widx];
            if ((ToCompareFromKey >=cst_lowercase_begin)&&(ToCompareFromKey <= cst_lowercase_end))
                                                                ToCompareFromKey -= cst_upperization ;

            if ((ToCompareFromArray>=cst_lowercase_begin)&&(ToCompareFromArray <= cst_lowercase_end))
                                                                ToCompareFromArray -= cst_upperization ;

            if (ToCompareFromKey==ToCompareFromArray)
                    {
                    if (wistart==-1)
                                wistart=wi;
                    widx++;
                    if (widx==pSize)
                        {
                        return (wistart);
                        }
                    continue;
                    }// if wKey
            widx=0;
            wistart=-1;
            }
return (-1) ;
}


template<typename _Tp>
/**
 * @brief ZArray<_Tp>::getifo get index(rank) from binary offset
 * @param[in] pOffset binary offset
 * @return rank corresponding to offset. -1 if offset is out of boundaries.
 */
long ZArray<_Tp>::getifo (long pOffset)
{

long z1;
    if (pOffset<0)
                return(pOffset);
    z1 = pOffset/(long)sizeof(_Tp);
    if (z1 >lastIdx()) return (-1);
    return (z1);
}

template<typename _Tp>
/**
 * @brief ZArray<_Tp>::search  searches with binary search for byte sequence contained in pKey of size pSize starting at ZArray row index pIdx.
 *                             it returns the ZArray row index where it has been found or -1 if not found.
 *
 *                             For successive searches, it is of the responsibility of user to manage the indexes
 *
 * @param pKey      void pointer to byte sequence to search for
 * @param pSize     byte sequence length
 * @param pIdx      row index to start the search from
 * @return
 */
long ZArray<_Tp>::search (const void *pKey,size_t pSize,long pIdx)
{

bsearchCurrentIdx = getifo(bsearch(pKey,pSize, (long)(pIdx*sizeof(_Tp))) );
return (bsearchCurrentIdx) ;
}


template<typename _Tp>
/**
 * @brief ZArray<_Tp>::search  searches with binary search for byte sequence contained in pKey of size pSize starting at ZArray row index pIdx.
 *                             it returns the ZArray row index where it has been found or -1 if not found.
 *
 *                             For successive searches, it is of the responsibility of user to manage the indexes
 *
 * @param pKey      void pointer to byte sequence to search for
 * @param pSize     byte sequence length
 * @param pIdx      row index to start the search from
 * @return
 */
long ZArray<_Tp>::searchCaseRegardless (const void *pKey,size_t pSize,long pIdx)
{

bsearchCurrentIdx = getifo(bsearchCaseRegardless(pKey,pSize, (long)(pIdx*sizeof(_Tp))) );
return (bsearchCurrentIdx) ;
}



template <typename _Tp>
/**
 * @brief ZArray<_Tp>::trim resizes ZArray allocated memory to the current number of elements plus one reallocation quota elements
 */
void ZArray<_Tp>::trim(void)
{
    ZAllocation = ZCurrentNb + ZReallocQuota ;
    allocate (ZAllocation);
    return ;
}

template <typename _Tp>
/**
 * @brief allocateCurrentElements  modifies the number of allocated Elements to the given number of Elements
 *  - Enlarge the number of current contained elements to the given number of Elements
 *  - Initialize the allocated space to binary 0
 *  - If the ZArray already contains elements :
 *   + Allocates the difference,
 *   + initialize the allocated elements (and not the existing ones) to binary 0.
 *  - Returns the actual newly allocated (and initialized) number of elements.
 *  - Checks if given allocation is compatible with current ZArray content.
 *
 * @param[in] pAlloc the new number of elements that the ZArray could contain.
 */
long ZArray<_Tp>::allocateCurrentElements (const ssize_t pAlloc)
{
    if (pAlloc<1)
                {
                fprintf(stderr,"ZArray::allocateCurrentElements-E-INVPAR Error : Invalid requested allocation <%ld>. Must be greater than 0.\n",
                        pAlloc);
                return (-1);
                }
    if (pAlloc <= ZCurrentNb)
                {
                fprintf(stderr,"ZArray::allocateCurrentElements-E-CURGTALLOC Error current number of elements <%ld> is greater than request <%ld>\n",
                        ZCurrentNb,
                        pAlloc);
                return (-1);
                }



#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    setAllocation(pAlloc,false);  // setAllocation has its own Mx lock
    long wI = (ZCurrentNb<0)?0:ZCurrentNb;
    memset(&Data.Tab[wI],0,pAlloc*sizeof(_Tp));
    ZCurrentNb=pAlloc;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
                 return (wI);
}  // allocateCurrentElements



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
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.lock();
#endif
    if (pAlloc<(size()+ZReallocQuota))
                    {

//    #if __DEBUG_LEVEL__ >0
            if (ZVerbose)
                fprintf(stderr,"ZArray::setAllocation-W-BADALLOC Warning allocation given <%ld> is less than current number of elements <%ld> + allocation quota <%ld>. Trimming the ZArray...\n",
                        pAlloc,
                        ZCurrentNb,
                        ZReallocQuota);
//    #endif

                trim() ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
                if (pLock)
                    _Mutex.unlock();
#endif
                return ;
                    }

                allocate(pAlloc);
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
    if (pInitialAllocation>ZAllocation)
        setAllocation(pInitialAllocation,pLock);
    return;
}

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::newBlankElement creates a new Element set to BINARY ZERO at the bottom of ZArray. This zeroed Element becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 * @param pClear static function or method to clear the newly created _Tp element.
 *   If not exist, _Tp structure is not cleared to zero after being added.
 */
long ZArray<_Tp>::newBlankElement(size_t pNumber, bool pLock, void (*pClear)(_Tp&))
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
       if (pLock)
           _Mutex.lock();
#endif
    addCheck(pNumber) ;
    if (ZCurrentNb<0)
            ZCurrentNb=0;

    if (pClear)  // if there is a clear function
            pClear(Data.Tab[ZCurrentNb]);  // call it
//        else // else memset dangerously the whole _TP data type
//            memset(&Data.Tab[ZCurrentNb],0,sizeof(_Tp)*pNumber); // this could destroy underneeth constants
    size_t wR =ZCurrentNb;
    ZCurrentNb +=pNumber ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
        _Mutex.unlock();
#endif
    return(wR) ;
} // newBlankElement

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push( _Tp &pElement)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1) ;
    if (ZCurrentNb<0)
            ZCurrentNb=0;
    Data.Tab[ZCurrentNb]=pElement;
//    memmove(&Data.Tab[ZCurrentNb],&pElement,sizeof(_Tp));
    size_t wR =ZCurrentNb;
    ZCurrentNb ++ ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return(wR) ;
} // push
//template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */
/*long ZArray<_Tp>::push(_Tp& pElement)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1) ;
    if (ZCurrentNb<0)
            ZCurrentNb=0;
    memmove(&Data.Tab[ZCurrentNb],&pElement,sizeof(_Tp));
    size_t wR =ZCurrentNb;
    ZCurrentNb ++ ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return(wR) ;
} // push with reference*/
template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push pushes pElement at the top of ZArray. pElement becomes the ZArray last element. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push(_Tp &&pElement)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1) ;
    if (ZCurrentNb<0)
            ZCurrentNb=0;
    Data.Tab[ZCurrentNb]=pElement;
//    memmove(&Data.Tab[ZCurrentNb],&pElement,sizeof(_Tp));
    size_t wR =ZCurrentNb;
    ZCurrentNb ++ ;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return(wR) ;
} // push with litteral
template <typename _Tp>
/**
 * @brief ZArray<_Tp>::push_front inserts pElement as the first element of ZArray. The elements collection is shifted appropriately. Returns the new number of elements.
 * @param pElement
 */
long ZArray<_Tp>::push_front(_Tp pElement)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1) ;
    ZCurrentNb ++ ;
    move (1,0,(ZCurrentNb-1));

    memmove(Data.Tab,&pElement,sizeof(_Tp));

    memmove (&ZReturn,   &Data.Tab[ZCurrentNb],sizeof(_Tp));
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
    if (ZCurrentNb<1)
            {
            fprintf (stderr,"popR-F-INVZCURRNB Fatal error : index of ZArray <%ld> is < 1 \n",
                     ZCurrentNb);
            abort();
            }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    ZCurrentNb -- ;
    //    memmove (ZReturn,   &ZPtr[ZCurrentNb],sizeof(_Tp));
        memmove (&ZReturn,   &Data.Tab[ZCurrentNb],sizeof(_Tp));
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZReturn);
} // popR


/** @brief popRP : ThreadSafe: erases the last element and returnsthe poped element's content into pReturn (that remains available but not within the ZArray)
 * @warning Not Thread safe.
 *  @return a reference to poped (removed) element content
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP(_Tp* pReturn)
{
    if (ZCurrentNb<1)
            {
        fprintf(stderr,"popRP-F-NOELEMENTS Fatal error no elements within ZArray while pop method has been invoked.\n"  );
        abort();
            }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    ZCurrentNb -- ;
    memmove (pReturn,   &Data.Tab[ZCurrentNb],sizeof(_Tp));
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (*pReturn);
} // popRP

/** @brief popRP_NL : pop result no lock : erases the last element and returns the poped element's content into pReturn (that remains available but not within the ZArray)
 * @warning Not Thread safe.
 *  @return a reference to poped (removed) element content
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP_NL(_Tp* pReturn)

{
    if (ZCurrentNb<1)
            {
        fprintf(stderr,"popRP-F-NOELEMENTS Fatal error no elements within ZArray while pop method has been invoked.\n"  );
        exit (EXIT_FAILURE);
            }
    ZCurrentNb -- ;
    memmove (pReturn,   &Data.Tab[ZCurrentNb],sizeof(_Tp));
    return (*pReturn);
} // popRP


/** @brief pop erases the last element for the ZArray. Returns the new array size or -1 if empty.
 *
 * @return new array size after pop operation or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop(void)
{
    if (ZCurrentNb<1)
                {
                return -1;
                }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    memset (&last(),0,sizeof(_Tp)); // set poped space to zero

    ZCurrentNb -- ;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZCurrentNb);
} // pop

/** @brief ZArray<_Tp>::_pop same as pop() but with no mutex lock */
template <typename _Tp>

long ZArray<_Tp>::_pop(void)
{
    if (ZCurrentNb<1)
                {
                return -1;
                }

    memset (&last(),0,sizeof(_Tp)); // set poped space to zero

    ZCurrentNb -- ;

    return (ZCurrentNb);
} // _pop


/** @brief pop_front Erases the first element. Returns the new array size or -1 if empty.
 * @return new array size or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop_front(void)
{
    if (ZCurrentNb<1)
            {
            return -1;
            }
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif

    //memmove (&ZPtr[0],&ZPtr[1],size_t (ZCurrentNb*sizeof(_Tp)));
    memmove (&Data.Tab[0],   &Data.Tab[1],size_t(sizeof(_Tp)*(ZCurrentNb-1)));
    memset(&last(),0,sizeof(_Tp));  // set available space to zero
    ZCurrentNb -- ;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZCurrentNb);
} // pop_front

/** @brief popR_front NOT ThreadSafe: Pop with return. Pops out the first array element and returns a pointer to the poped element's content.
 *                  returns NULL if array is empty.
 *
 *  WARNING : Not Thread safe.
 *
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popR_front(void)

{
    if (ZCurrentNb<1)
            {
            fprintf (stderr,"ZArray<_Tp>::popR_front-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
            abort();
            }
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    memmove (&ZReturn,   &Data.Tab[0],sizeof(_Tp));

    memmove (&Data.Tab[0], &Data.Tab[1],size_t(sizeof(_Tp)*(ZCurrentNb-1)));
    memset(&last(),0,sizeof(_Tp));  // set available space to zero

    ZCurrentNb -- ;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZReturn); // bad thing when being used with ZThreads
} // popR_front


/** @brief popRP_front ThreadSafe: Pops out the first array element and returns a pointer to the poped element's content.
 *                  returns NULL if array is empty.
 *
 *  WARNING : Not Thread safe.
 *
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP_front(_Tp*pReturn)
{
    if (ZCurrentNb<1)
            {
            fprintf(stderr,"popRP_front-F-NOELEMENTS Fatal error no elements within ZArray while popRP_front method has been invoked.\n" );
            abort();
            }
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    memmove (pReturn,   &Data.Tab[0],size_t (sizeof(_Tp)));

    memmove (&Data.Tab[0], &Data.Tab[1],size_t(sizeof(_Tp)*ZCurrentNb));
    memset(&last(),0,sizeof(_Tp));  // set available space to zero
    ZCurrentNb -- ;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (*pReturn);
} // popR_front


template <typename _Tp>
long
ZArray<_Tp>::searchforValue (long pOffset, long pSize, const void *pValue) //! sequential search (costly) for a value
{
    const unsigned char* wPtrArray;
    const unsigned char* wPtrValue = static_cast<const unsigned char*>(pValue);
    for (long wi=0;wi<size();wi++)
            {
            wPtrArray=(const unsigned char *)(&Tab[wi]);
            wPtrArray += pOffset;
            if (memcmp(wPtrArray,wPtrValue,pSize)==0)
                                    return(wi);
            }
    return (-1);
}// _searchforValue


//
//-------------Functions---------------
//

//=============Export Import==================================

/**
 * @brief _ZAexportAllocated Exports to a flat memory zone the content of a ZArray corresponding to its Allocated space (including elements that are not yet used),
 *                          including its descriptive header ZAExport.
 *  This routine allocates the required memory size to export the data.
 * @warning The allocated memory has to be freed later on. This is the responsibility of the caller to free this memory buffer.
 *  NB: at this stage ZDataBuffer is not supposed to be defined. This is the reason why memory is allocated and needs to be freed.
 * @param|out] pBuffer     ZArray exported data including ZAExport header
 * @param[out] pBufferSize The total size to exported ZArray in a flat format (including free space at the end of ZArray)
 * @param[in]  _exportConvert conversion routine (Endian) for exporting/importing one element of type _Tp
 *              first argument is a reference to a _Tp element to convert
 *              second argument is a pointer to a _Tp argument converted by the routine
 * @return -1 in case of memory allocation error, or pBuffer if everything went OK
*/
template <class _TpIn , class _TpOut>
unsigned char*
_ZAexportAllocated(ZArray<_TpIn>* pZArray,
                   unsigned char* &pBuffer,
                   size_t &pBufferSize,
                   _TpOut (*_exportConvert)(_TpIn&,_TpOut*)) // Routine to convert a single element of ZArray
                                                     // first arg  : element to convert
                                                     // second arg : buffer to receive converted element
{
#ifdef __USE_ZTHREAD__
    pZArray->lock();
#endif
    ZAExport ZAE;
    size_t wDataSize;
//        unsigned char* wReturn;
    ZAE.AllocatedSize = pZArray->getAllocatedSize();   // get the total allocated size
    ZAE.CurrentSize = pZArray->getCurrentSize();     // get the current size corresponding to currently stored elements

    wDataSize=ZAE.DataSize = ZAE.AllocatedSize;     // taking the AllocatedSize

    ZAE.FullSize = ZAE.DataSize+sizeof(ZAExport)+1;
    pBufferSize=ZAE.FullSize;

    ZAE.AllocatedElements = pZArray->getAllocation();
    ZAE.InitialAllocation = pZArray->getInitalAllocation();
    ZAE.ExtentQuota = pZArray->getQuota();
    ZAE.NbElements = pZArray->ZCurrentNb;
    pBuffer = (unsigned char*)malloc(ZAE.FullSize);

    if (pBuffer==nullptr)
                {
                fprintf(stderr,
                        "ZArray::_export> ******Memory allocation error****\n");
                abort();
                }
    ZAE.DataOffset = sizeof(ZAExport);
    memset (pBuffer,0,ZAE.FullSize);
    pZArray->convertZAExport_I(ZAE);
    memmove(pBuffer,&ZAE,sizeof(ZAExport));
    _TpOut* wEltPtr=(_TpOut*)(pBuffer+ZAE.DataOffset);
    for (long wi=0;wi<pZArray->size();wi++)
        {
        _exportConvert(pZArray->Tab[wi],wEltPtr);
        wEltPtr++;
        }

#ifdef __USE_ZTHREAD__
    pZArray->unlock();
#endif

    return pBuffer;
}//_exportAllocated

/**
 * @brief ZAexportCurrent this static routine exports (serialize) to a flat memory zone
 *          the content of a ZArray corresponding to its current number of elements, including its descriptive header ZAExport.
 *
 *  This routine allocates the required memory size to export the data.
 *  NB: at this stage ZDataBuffer is not supposed to be defined. This is the reason why memory is allocated and needs to be freed by callee.
 *
 *  In case of memory allocation error, pBuffer is set to nullptr, errno is set to ENOMEM and pBufferSize is set to 0
 *
 *  ZArray data is serialized to big endian values using mandatory _exportConvert routine.
 *
 * @warning The allocated memory has to be freed later on. This is the responsibility of the caller to free this memory buffer.
 *
 * @param|out] pBuffer     ZArray exported data including ZAExport header
 * @param[out] pBufferSize The total size to exported ZArray in a flat format (strictly limitated to used space in ZArray)
 * @param[in]  _exportConvert conversion routine (Endian) for exporting/importing one element of type _Tp
 *              first argument is a reference to a _Tp element to convert
 *              second argument is a pointer to a _Tp argument converted by the routine
 * @return      a pointer to allocated memory block containing the exported (serialized) ZArray data if everything went OK
 *              nullptr in case of memory allocation error
*/
template <class _TpIn , class _TpOut>
static
unsigned char* ZAexportCurrent(ZArray<_TpIn>* pZArray,
                                unsigned char* &pBuffer,
                                size_t &pBufferSize,
                                _TpOut (*_exportConvert)(_TpIn&,_TpOut*)) // Routine to convert a single element of ZArray
                                                                  // first arg  : element to convert
                                                                  // second arg : buffer to receive converted element
{
#ifdef __USE_ZTHREAD__
    pZArray->lock();
#endif
    ZAExport ZAE;
    size_t wDataOffset=0;
//        unsigned char* wReturn;
    ZAE.AllocatedSize = pZArray->getAllocatedSize();   // get the total allocated size
    ZAE.CurrentSize = pZArray->getCurrentSize();     // get the current size corresponding to currently stored elements

    ZAE.DataSize = ZAE.CurrentSize;     // taking the CurrentSize

    ZAE.FullSize = ZAE.DataSize+sizeof(ZAExport)+1;
    pBufferSize=ZAE.FullSize;

    ZAE.AllocatedElements = pZArray->getAllocation();
    ZAE.InitialAllocation = pZArray->getInitalAllocation();
    ZAE.ExtentQuota = pZArray->getQuota();
    ZAE.NbElements = pZArray->ZCurrentNb;
    pBuffer = (unsigned char*)malloc(ZAE.FullSize);
    if (pBuffer == nullptr) {
        errno = ENOMEM;
        pBufferSize = 0;
        return nullptr;
        }
    memset (pBuffer,0,ZAE.FullSize);

    wDataOffset=ZAE.DataOffset = sizeof(ZAExport);
    memset (pBuffer,0,ZAE.FullSize);
    pZArray->convertZAExport_I(ZAE);
    memmove(pBuffer,&ZAE,sizeof(ZAExport));
    _TpOut* wEltPtr=(_TpOut*)(pBuffer+wDataOffset);
    for (long wi=0;wi<pZArray->size();wi++)
        {
        _exportConvert(pZArray->Tab[wi],&wEltPtr[wi]);
        }

#ifdef __USE_ZTHREAD__
    pZArray->unlock();
#endif

    return pBuffer;
}//_exportCurrent

template <class _TpIn , class _TpOut>
/**
 * @brief ZAimport Imports a ZArray from a flat ZArray content. Sets up all ZArrays characteristics and imports data from a flat content.
 *
 * _TpIn is data structure with export format (not aligned : see #pragma pack() clauses)
 * _TpOut is data structure with aligned format
 *
 * @param pBuffer a pointer to a flat data memory zone with first leading block ZAExport, giving ZArray parameters and size and pointer to data
 * @return the size of pBuffer consummed per the import operation : how much bytes have been used from pBuffer for importing ZArray.
 *         This is equal to ZAExport::FullSize
 */

size_t ZAimport(ZArray<_TpOut>* pZArray,
                 unsigned char* pBuffer,
                 _TpOut (*_importConvert)(_TpOut&,_TpIn*),// Routine to convert a single element of ZArray for import
                 // first arg  : element to receive converted data to integrate into ZArray
                 // second arg : input buffer to convert element : format is not aligned struct
                 ZAExport* pZAE=nullptr) // if not nullptr uses given ZAExport block
{


#if __USE_ZTHREAD__
    pZArray->lock();
#endif
    pZArray->clear(false); // clear with no lock
    ZAExport* ZAE=pZAE;
    bool wZAECreated=false;
    if (pZAE== nullptr)
            {
            ZAE = new ZAExport;
            wZAECreated=true;
            }
    (*ZAE) = pZArray->ZAEimport(pBuffer);
    pZArray->setAllocation(ZAE->AllocatedElements,false);  // no lock
    pZArray->bzero(0,-1,false);// no lock
    pZArray->setQuota(ZAE->ExtentQuota);
    pZArray->setInitialAllocation(ZAE->InitialAllocation,false); // no lock
    if (ZAE->NbElements>(ssize_t)ZAE->AllocatedElements)
        {
        pZArray->setAllocation(ZAE->NbElements,false);
        }
    pZArray->newBlankElement(ZAE->NbElements);
//    pZArray->ZCurrentNb = ZAE.NbElements;
    _TpIn* wEltPtr_In=(_TpIn*)(pBuffer+ZAE->DataOffset);
    _TpOut* wEltPtr_Tab= pZArray->Tab;
    _TpOut wElt;
    for (long wi=0;wi<ZAE->NbElements;wi++)
        {
        _importConvert(wEltPtr_Tab[wi],&wEltPtr_In[wi]);
//        memmove(&wEltPtr_Tab[wi],&wElt,sizeof(_TpOut));
        }// for


#ifdef __USE_ZTHREAD__
    pZArray->unlock();
#endif
    if (!wZAECreated)
                return ZAE->FullSize;
    size_t wFullSize= ZAE->FullSize;
    delete ZAE;
    return wFullSize;
}//_import




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
_searchforValue (zbs::ZArray<_Type> &pZArray,long pOffset,long pSize, void *pValue) // to be tested before becoming a standard
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
