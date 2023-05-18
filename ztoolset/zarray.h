#ifndef ZARRAY_H
#define ZARRAY_H

//#define __USE_NEW_STD_ALLOCATOR__

#include <config/zconfig.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/zmem.h>  // for zfree()

#include <ztoolset/zerror_min.h>

#ifdef __USE_ZTHREAD__
    #include <zthread/zmutex.h>
#endif

#include <cassert>

#include <memory.h>

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

//#include <ztoolset/zdatabuffer.h>

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
class ZArray
{

private: allocator<_Tp> Allocator;
/*
 *  Data Attributes
 */
public :
  _Tp * Tab=nullptr ;

  ssize_t ZCurrentNb=0 ;        // current number of elements : 0 if ZArray is empty lastIdx() returns -1
  const size_t ZElementSize=sizeof(_Tp); // for Qt debug interface purpose

protected:
  size_t ZAllocation=0 ;        // allocated number of elements
  size_t ZAllocatedSize=0 ;     // allocated size in bytes (excepted one char of highvalue mark )
  size_t ZReallocQuota=0 ;      // reallocation quota in number of elements to reallocate each reallocation operation
  size_t ZInitialAllocation=0;  // keep initial allocation for reset purpose : ZClear will get back to this memory allocation
  _Tp ZReturn ;               // memory zone for returning element (WARNING: not to be used with multi-thread)

//  bool _hasChanged=false;
private :
//  void *ZPtr =nullptr;
  long ZIdx=0 ;                 // Last index accessed

#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
  ZMutex _Mutex ;
#endif
public:

   ZArray():_Mutex() {
     _mInit(_cst_default_allocation,_cst_realloc_quota);
   }
   ZArray (size_t pInitialAlloc,
       size_t pReallocQuota) : _Mutex() {
     _mInit(pInitialAlloc,pReallocQuota);
   }

   ~ZArray();

 //  ZArray(ZArray &) = delete;                // can not copy
 //  ZArray& operator = (ZArray&)=delete;     // can not copy
//#ifdef __USE_ZARRAY_COPY_CONTRUCTOR__

   ZArray(ZArray& pIn):_Mutex()
   {
        _mInit(_cst_default_allocation,_cst_realloc_quota);
       _copyFrom(pIn);
       return;
   }
   ZArray( ZArray&& pIn):_Mutex()
   {
     //       _mInit(); /* init with default allocation values */
     _mInit(_cst_default_allocation,_cst_realloc_quota);
     _copyFrom(pIn);
     return;
   }
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

/*   void resetChanged() {_hasChanged=false;}
   void setChanged () {_hasChanged=true;}

   bool hasChanged()  {return _hasChanged;}
*/
   bool exists(long pIdx){if ((pIdx<0)||(pIdx>lastIdx())) return false; return true;}

//#else
#ifdef __COMMENT__
   ZArray(ZArray &) = delete;                // can not copy
   ZArray& operator = (ZArray&)=delete;     // can not copy
#endif
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
     _Tp*       it  = (_Tp*)__l.begin();  // raw pointer!
       _Tp* const end =(_Tp*) __l.end();    // raw pointer!

       for (; it != end; ++it)
         this->push((_Tp&)*it);
    return *this;
   }

   _Tp* data()  _GLIBCXX_NOEXCEPT
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
  _Tp & operator [] (const long pIndex) {
      if (pIndex<ZCurrentNb)
        return (Tab[pIndex]);
      fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
      abort();
  }

  const _Tp & operator []  (const long pIndex) const {
    if (pIndex<ZCurrentNb)
      return (Tab[pIndex]);
    fprintf(stderr,"ZArray-E-IVIDXVAL invalid index value out of array boundary.\n");
    abort();
  }

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

/** Iterators */

   class iterator
   {
   public:
     typedef _Tp value_type;
     typedef _Tp& reference;
//     typedef _Tp* pointer;
     typedef forward_iterator_tag iterator_category;
     typedef int difference_type;
     iterator() {}
     iterator(ZArray<_Tp> *pArray) : Array(pArray),Index(pArray->ZCurrentNb) { }
     iterator(ZArray<_Tp> &pArray) : Array(&pArray),Index(pArray.ZCurrentNb) { }
     iterator(const iterator &pIn) {_copyFrom(pIn); }
     iterator& _copyFrom(const iterator& pIn) {
       Array=pIn.Array;
       Index=pIn.Index;
       return *this;
     }
     iterator& operator=(const iterator& pIn) {return _copyFrom(pIn);}

     iterator& operator++() {
       if (Index ==  Array->ZCurrentNb)
         abort();
       Index++;
       return *this;
     }

     iterator operator++(int) {
       auto retval(*this);
       Index++;
       return retval;
     }

     iterator& operator--() {
       if (Index != 0)
        Index--;
       return *this;
     }
     iterator operator--(int) {
       auto retval(*this);
       Index--;
       return retval;
     }

     iterator& operator+=(int pVal) {
       Index+= pVal;
       return *this;
     }
     iterator& operator-=(int pVal) {
       Index-= pVal;
       return *this;
     }

     _Tp* getPtr() {return (Array->Tab + Index) ; }
     const _Tp* getConstPtr() const  {return (Array->Tab + Index) ; }

     _Tp& operator*() { return *(Array->Tab + Index) ; }
     _Tp* operator->() { return (Array->Tab + Index) ; }

     bool operator==(const iterator& pIn) { return (Array->Tab == pIn.Array->Tab) && (Index==pIn.Index); }
     bool operator!=(const iterator& pIn) { return (Array->Tab != pIn.Array->Tab) || (Index!=pIn.Index);  }

     iterator operator-(const iterator& pIn) {
       /* only if both iterators are for the same ZArray object */
    assert((Array->Tab == pIn.Array->Tab) && (Index==pIn.Index));
    iterator wReturn=*this ;

    if (wReturn.Index == 0)
        {
         fprintf(stderr,"iterator::operator-  invalid operation, cannot have negative value for index.\n");
          abort();
        }
    wReturn.Index -= pIn.Index;
    return wReturn;
    }//operator-

     iterator operator+(const iterator& pIn) {
      /* only if both iterators are for the same ZArray object */
       assert((Array->Tab == pIn.Array->Tab) && (Index==pIn.Index));

       iterator wReturn=*this ;

       wReturn.Index += pIn.Index;
       if (wReturn.Index > end().Index)
         return end();
       return wReturn;
     }//operator+

    iterator operator+(const long pIn) {
       iterator wReturn=*this ;
       wReturn.Index += pIn;
       if (wReturn.Index > end().Index)
         return end();
       return wReturn;
     } //operator+


     iterator operator-(const long pIn) {
       iterator wReturn=*this ;
       wReturn.Index -= pIn;
       if (wReturn.Index < 0)
         return begin();
       return wReturn;
     } //operator+

     void setEnd() {Index=Array->ZCurrentNb;}

     iterator begin() {Index=0 ; return *this;}
     iterator end() {Index=Array->ZCurrentNb ; return *this;}
   protected:
     ZArray<_Tp>* Array=nullptr;
     long Index=0;
   }; // iterator

   iterator begin() {
     return std::move(iterator (*this).begin());
   }

   iterator end() {
     return iterator (*this).end();
   }

   class const_iterator
   {
   public:
     typedef _Tp value_type;
     typedef _Tp& reference;
     //     typedef _Tp* pointer;
     typedef forward_iterator_tag iterator_category;
     typedef int difference_type;
     const_iterator() {}
     const_iterator(ZArray<_Tp> *pArray) : Array(pArray),Index(pArray->ZCurrentNb) { }
     const_iterator(ZArray<_Tp> &pArray) : Array(&pArray),Index(pArray.ZCurrentNb) { }
     const_iterator(const iterator &pIn) {_copyFrom(pIn); }
     const_iterator& _copyFrom(const const_iterator& pIn) {
       Array=pIn.Array;
       Index=pIn.Index;
       return *this;
     }
     const_iterator& operator=(const iterator& pIn) {return _copyFrom(pIn);}

     const_iterator& operator++() {
       if (Index ==  Array->ZCurrentNb)
         abort();
       Index++;
       return *this;
     }

     const_iterator operator++(int) {
       auto retval(*this);
       Index++;
       return retval;
     }

     const_iterator& operator--() {
       if (Index != 0)
         Index--;
       return *this;
     }
     const_iterator operator--(int) {
       auto retval(*this);
       Index--;
       return retval;
     }
     const_iterator& operator+=(int pVal) {
       Index+= pVal;
       return *this;
     }
     const_iterator& operator-=(int pVal) {
       Index-= pVal;
       return *this;
     }
//     _Tp* getPtr() {return (Array->Tab + Index) ; }
     const _Tp* getPtr() const  {return (Array->Tab + Index) ; }

     const _Tp& operator*() { return *(Array->Tab + Index) ; }
     const _Tp* operator->() { return (Array->Tab + Index) ; }

     bool operator==(const const_iterator& pIn) { return (Array->Tab == pIn.Array->Tab) && (Index==pIn.Index); }
     bool operator!=(const const_iterator& pIn) { return (Array->Tab != pIn.Array->Tab) || (Index!=pIn.Index);  }

     const_iterator operator-(const const_iterator& pIn) {
       /* only if both iterators are for the same ZArray object */
       assert((Array->Tab == pIn.Array->Tab) && (Index==pIn.Index));
       const_iterator wReturn=*this ;

       if (wReturn.Index == 0)
       {
         fprintf(stderr,"iterator::operator-  invalid operation, cannot have negative value for index.\n");
         abort();
       }
       wReturn.Index -= pIn.Index;
       return wReturn;
     }//operator-

     const_iterator operator+(const const_iterator& pIn) {
       /* only if both iterators are for the same ZArray object */
       assert((Array->Tab == pIn.Array->Tab) && (Index==pIn.Index));

       const_iterator wReturn=*this ;

       wReturn.Index += pIn.Index;
       if (wReturn.Index > end().Index)
         return end();
       return wReturn;
     }//operator+

     const_iterator operator+(const long pIn) {
       const_iterator wReturn=*this ;
       wReturn.Index += pIn;
       if (wReturn.Index > end().Index)
         return end();
       return wReturn;
     } //operator+


     const_iterator operator-(const long pIn) {
       const_iterator wReturn=*this ;
       wReturn.Index -= pIn;
       if (wReturn.Index < 0)
         return begin();
       return wReturn;
     } //operator+

     void setEnd() {Index=Array->ZCurrentNb;}

     const_iterator begin() {Index=0 ; return *this;}
     const_iterator end() {Index=Array->ZCurrentNb ; return *this;}

   protected:
     ZArray<_Tp>* Array=nullptr;
     long Index=0;
   }; // const_iterator


/** end Iterators */

/** =================================================================
 *      Methods to be protected when using ZSortedArray
 */

    long push(_Tp &pElement) ;
    long _pushNoLock(_Tp &pElement);
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
    long moveLeft (size_t pDest, size_t pOrig,size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)
    long moveRight (size_t pDest, size_t pOrig,size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)
private:
    long _move (size_t pDest, size_t pOrig, ssize_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)
    /** @brief _shiftRight shift array right from one element starting at pOrig- Element at pOrig is duplicated to pOrig + 1 */
    long _shiftRight(long pOrig);
    /** @brief _shiftLeft shift array left from one element starting at pOrig. Element at pOrig - 1 is lost. */
    long _shiftLeft(long pOrig);
    long _moveRight(long pDest, long pOrig, long pNumber=1);
    long _moveLeft(long pDest, long pOrig, long pNumber=1);
public:

    long swap (size_t pDest, size_t pOrig,  size_t pNumber=1);  // forbidden if ZKeyArray (because of possible Joins)

    long insert (_Tp &pElement, size_t pIdx);
    long insert (_Tp* pElement,size_t pIdx) {return(insert((_Tp&)(*pElement),pIdx));}
    long insert (_Tp *pElement, size_t pIdx, size_t pNumber);
    long erase(size_t pIdx,size_t pNumber=1);

    long _insertNoLock(_Tp &pElement, size_t pIdx);
    long _insertNoLock(_Tp *pElement, size_t pIdx, size_t pNumber=1);
    long _eraseNoLock(size_t pIdx,size_t pNumber=1);

           //!< @brief trim adjusts the memory size of the ZArray to the current contained elements + realloc quota
    /** ! @brief last returns the ZArray content of the last element of the ZArray. If ZArray is empty, it issues a fatal error and terminates.
     */
    _Tp &last(void) { if (size()>0)
                            return(Tab[ZCurrentNb-1]);
                       fprintf(stderr,"ZArray::last-F-OUTOFBOUND empty ZArray, Out of boundaries\n");
                       abort() ;
                    }

  //! @brief ZArray::lastIdx returns the index for the last ZArray element : WARNING:  when ZArray is empty, returns -1
    long lastIdx (void) {return (isEmpty()?-1:(ZCurrentNb-1));}


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



    long newBlankElement(size_t pNumber=1, bool pLock=false, void (*pClear)(_Tp &)=nullptr);

    bool isEmpty(void) {return (ZCurrentNb==0);}


    /* for class  objects */
#ifdef __COMMENT__
    _Tp & assign (typename std::enable_if<(std::is_class<_Tp>::value),_Tp> &pValue, const long pIdx)
        {
        if ((size_t)pIdx >= ZAllocation)
        {
          fprintf (stderr,"ZArray::assign-F-OVERFLW Index <%ld> bypasses allocation <%ld>\n",pIdx,ZAllocation);
          abort();
        }
        memset(&Tab[pIdx],0,sizeof(_Tp)); /* cannot invoke constructor but can set memory surface to zero */
        Tab[pIdx] = pValue;               /* copy constructor and operator = must be defined if class */
      //  _hasChanged=true;
        return Tab[pIdx];
        }

    /* for other objects than class */
    _Tp & assign (typename std::enable_if<!(std::is_class<_Tp>::value),_Tp> &pValue, const long pIdx)
        {
          if ((size_t)pIdx >= ZAllocation)
          {
            fprintf (stderr,"ZArray::assign-F-OVERFLW Index <%ld> bypasses allocation <%ld>\n",pIdx,ZAllocation);
            abort();
          }
          memset(&Tab[pIdx],0,sizeof(_Tp)); /* cannot invoke constructor but can set memory surface to zero */
          Tab[pIdx] = pValue;               /* copy constructor and operator = must be defined if class */
        //  _hasChanged=true;
          return Tab[pIdx];
        }

#endif // __COMMENT__


//    void clearValue (const long pIdx);

    _Tp & reverse (long pIdx);

    size_t bzero (size_t pOrig=0, long pNumber=-1, bool pLock=true);
    void reset(void);
    void clear(bool pLock=true);

    ZArray<_Tp>* clone(void);

 //   void copy(ZArray<_Tp> &pZCopied)  {copy(static_cast<const ZArray<_Tp>> (pZCopied));}
    ZArray<_Tp> & _copyFrom(ZArray<_Tp> &pIn)  ;



    /** @brief size  size of current ZArray : number of element in the current ZArray (and NOT bytes size) see getAllocatedSize*/
    long size(void) const ;
    /** @brief count  size of current ZArray : number of element in the current ZArray (and NOT bytes size) see getAllocatedSize*/
    long count(void) const;
    /** @brief elementSize  returns the size in bytes of one _Tp element i.e. the size of one row fo the array */
    long elementSize(void) ;
    /** @brief usedSize  returns the size in bytes of one _Tp element (size of one row of the array). Alias for getCurrentSize()*/
    long usedSize(void) ;

    size_t getAllocation (void) const {return (ZAllocation);} /*!< returns the number of elements currently being allocated (and not those used) */
    size_t getQuota(void) const {return (ZReallocQuota);} /*!< returns the reallocation quota for the ZArray : Number of elements that will be allocated each time ZArray needs additional space */
    size_t getInitalAllocation() const {return ZInitialAllocation;}
    /** @brief getAllocatedSize returns the size in bytes of the memory currently allocated to the ZArray instance */
    size_t getAllocatedSize(void) const {return(ZAllocatedSize);}
    /** @brief  getCurrentSize returns the size in bytes taken by current number of elements */
    ssize_t getCurrentSize(void) const { return (sizeof(_Tp)*ZCurrentNb);}

    void setAllocationValues(size_t pAllocation,size_t pReallocQuota) {setAllocation(pAllocation); setQuota(pReallocQuota);}
    inline void setQuota (size_t pQuota) {ZReallocQuota=pQuota;} /** !< sets the new reallocation quota for the ZArray in number of elements */
    void setAllocation (size_t pAlloc, bool pLock=true);
    void setInitialAllocation(size_t pInitialAllocation,bool pLock=true);
    long allocateCurrentElements (const ssize_t pAlloc);

    /** @brief getDataPtr returns the current pointer to data content. Warning : this pointer is not stable and is to be changed whenever reallocation occurs */
    inline void* getDataPtr() const {return (void*)Tab;}

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
    void addCheck(long pAdd);


    _Tp* _baseAllocate(long pAlloc) {
    Tab=Allocator.allocate(pAlloc);
    ZAllocation = pAlloc;
    ZAllocatedSize = sizeof(_Tp)*pAlloc;
//    memset(Tab,0,ZAllocatedSize); // No there are not only atomic / trivial objects managed
    return Tab;
    }

    void _deallocate() {
      if (Tab==nullptr)
        return;
      Allocator.deallocate(Tab,ZAllocation);
      Tab=nullptr;
    }
    void _deallocate(_Tp* pPtr,size_t pAllocation) {
      if (Tab==nullptr)
        return;
      Allocator.deallocate(pPtr,pAllocation);
      Tab=nullptr;
    }


    void allocate(long pAlloc)
    {
#if __DEBUG_LEVEL__  > 4
      if (ZAllocation >= pAlloc )
      {
        fprintf (stderr,"ZArray::allocate-W-Warning requested new allocation <%ld> not greater than existing allocation <%ld> \n",pAlloc,ZAllocation);
      }
#endif // _DEBUG_LEVEL

      if (pAlloc < ZCurrentNb) {
#if __DEBUG_LEVEL__  > 1
        fprintf (stderr,"ZArray::allocate-W-ERRALLOC requested new allocation <%ld> not greater than existing number of elements within ZArray <%ld> .\n"
                        "You should suppress elements from the ZArray and use trim/resize facility to reduce the memory allocated (ZArray::trim).\n",
            pAlloc,ZCurrentNb);
        //        pTab=Tab;
        return ;
#endif // _DEBUG_LEVEL
      }

      if (Tab==nullptr) {
        Tab=_baseAllocate(pAlloc);
        return;
      }
      /* Must trim ? */
      if (pAlloc < ZAllocation) {
        if (pAlloc < ZCurrentNb) {
          pAlloc = ZCurrentNb;
        }
        resize(pAlloc);
        return;
      }

      Tab=extend(pAlloc-ZAllocation);
      return ;
    }// allocate



    _Tp* _baseRealloc(size_t pSize){
      _Tp* wTab_old=Tab;
      size_t wAllocation_old = ZAllocation;
      Tab =_baseAllocate(pSize);
      for (long wi=0;wi < ZCurrentNb ; wi++) {
        Allocator.construct(&Tab[wi],wTab_old[wi]);
        Allocator.destroy(&wTab_old[wi]);
//        Tab[wi] = std::move<_Tp>(wTab_old[wi]);
        //Tab[wi] = wTab_old[wi];
      }
      Allocator.deallocate(wTab_old,wAllocation_old);
      return Tab;
    }

    _Tp* extend(long pExtent) {
      if (Tab==nullptr) {
        return _baseAllocate(pExtent);
      }
      return _baseRealloc(ZAllocation + pExtent);
    } // extend


    _Tp & assign (_Tp &pValue, const long pIdx)
    {
      if (Tab==nullptr)
        _mInit(cst_ZRF_default_allocation,cst_ZRF_default_extentquota);
      if ((size_t)pIdx >= ZAllocation) {
        fprintf (stderr,"ZArray::assign-F-OVERFLW Index <%ld> bypasses allocation <%ld>\n",pIdx,ZAllocation);
        abort();
      }
      Allocator.construct(&Tab[pIdx],pValue);
      return Tab[pIdx];
    }//assign


    _Tp & assignReturn ( const long pIdx);

    _Tp *trim(ssize_t pReallocQuota=-1);
    _Tp *resize(ssize_t pNewAllocation);

#ifdef __DEPRECATED__
    void _sizeAllocate(size_t pSizeAlloc);
#endif
    void _setContent (void *pContent,size_t pSize) {memmove(Tab,pContent,pSize); return;}



    unsigned int Check=cst_ZCHECK;

}; // class ZArray


/* --------------- Begin developed methods ------------------*/



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

template <typename _Tp>
void ZArray<_Tp>::_mInit(size_t pInitialAlloc,
                         size_t pReallocQuota)
{
    _deallocate();
    ZAllocation = 0;
    ZInitialAllocation=pInitialAlloc ;
    ZReallocQuota = pReallocQuota ;
    ZCurrentNb = 0 ;  // Empty ZArray : size() == 0
    allocate (ZInitialAllocation);
//    memset (Tab,0,ZAllocatedSize);

    ZIdx=0;
/*
#ifdef __USE_ZTHREAD__  //   in any case (__ZTHREAD_AUTOMATIC__ or not) we have to define a ZMutex
    _Mtx = new ZMutex;
#endif
*/
}

template <typename _Tp> ZArray<_Tp>::~ZArray()
{
  if (Tab!=nullptr) {
    if ( ZCurrentNb > 0)
        erase(0,ZCurrentNb);
    _deallocate(Tab,ZAllocation);
//      Allocator.deallocate(Tab,ZAllocation);
  }

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
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    pIn.lock();
#endif

//    this->clear(false);
    setQuota(pIn.ZReallocQuota);
    allocate(pIn.ZAllocation);
    ZCurrentNb = pIn.ZCurrentNb ;
    for (long wi=0;wi<pIn.ZCurrentNb;wi++)
      assign (pIn.Tab[wi],wi);
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
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
    if ((pIdx <0)||(pIdx>=ZCurrentNb))
                        {
                        fprintf (stderr,"ZArray::reverse-F-outofboundaries Fatal error : given index <%ld> is out of ZArray boundaries\n",pIdx);
                        std::cerr.flush();
                        }
#endif
    return (Tab[ZCurrentNb-pIdx-1]);
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
long ZArray<_Tp>::moveRight (size_t pDest, size_t pOrig, size_t pNumber)
{
  long wRet=0;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  wRet=_moveRight(pDest,pOrig,pNumber);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return wRet ;
}// moveRight


template <typename _Tp>
long ZArray<_Tp>::moveLeft (size_t pDest, size_t pOrig, size_t pNumber)
{
  long wRet=0;

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  wRet=_moveLeft(pDest,pOrig,pNumber);
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return wRet ;
}// moveLeft
template <typename _Tp>
long ZArray<_Tp>::_move (size_t pDest, size_t pOrig, ssize_t pNumber)
{
  if (pDest > pOrig)
    return _moveRight(pDest,pOrig,pNumber);
  return _moveLeft(pDest,pOrig,pNumber);
}// _move

template <typename _Tp>
/**
 * @brief ZArray<_Tp>::_moveRight moves pNumber Elements right from position pOrig to pPosition pDest
 */
long ZArray<_Tp>::_moveRight (long pDest, long pOrig, long pNumber)
{
  if ((pDest==pOrig) || (pNumber==0)) {
    ZIdx = pDest;
    return (pDest);
  }
  if (pNumber < 0) {
    fprintf(stderr,"ZArray::_moveRight-F-INVVALUE Invalid value <%ld>",pNumber );
    abort();
  }

  if (pOrig >= ZCurrentNb) {
    fprintf(stderr,"ZArray::_moveRight-F-OUTBOUND error moving array zone from <%ld> while max number of element is <%ld>",pOrig,ZCurrentNb );
    abort(); // pOrig is out of boundaries
  }
  if (pDest < pOrig) {
    fprintf(stderr,"ZArray::_moveRight-F-INVVALUE Impossible to move data RIGHT from source <%ld> to destination <%ld>\n",pDest,pOrig );
    abort();
  }

  addCheck(pNumber);                       // make room for moving elements if necessary


  /* duplicate memory zone */

  _Tp* wDupB=(_Tp*)malloc ((ZCurrentNb-pOrig)*sizeof(_Tp));
  _Tp* wDup=wDupB;
  long wD=pOrig;
  for (long wi=0;wi <ZCurrentNb-pOrig;wi++) {
    memmove(&wDupB[wi],&Tab[wD++],sizeof(_Tp));
  }
  /* effective copy */
  long wi=0;
  for (long wDest=pDest ; wDest < ZCurrentNb+pNumber ; wDest++) {
    memmove(&Tab[pDest],&wDupB[wi++],sizeof(_Tp));
  }
  free(wDupB);
  ZIdx = pDest ;

  return pDest ;
}// _moveRight

template <typename _Tp>
long ZArray<_Tp>::_shiftRight (long pOrig)
{

  if (pOrig >= ZCurrentNb) {
    fprintf(stderr,"ZArray::_shiftRight-F-OUTBOUND error moving array zone from <%ld> while max number of element is <%ld>",pOrig,ZCurrentNb );
    abort(); // pOrig is out of boundaries
  }

  addCheck(ZCurrentNb + 1 );

  long wDest = ZCurrentNb + 1;
  long wOrig = ZCurrentNb;
  while ( wOrig >= pOrig ) {
    memmove(&Tab[wDest--],&Tab[wOrig--],sizeof(_Tp));
  }
  memset(&Tab[pOrig],0,sizeof(_Tp));
  return ZIdx = pOrig ;
}// _shiftRight

template <typename _Tp>
long ZArray<_Tp>::_shiftLeft (long pOrig)
{

  if (pOrig  < 1) {
    fprintf(stderr,"ZArray::_shiftLeft-F-OUTBOUND Error shifting left array zone from <%ld> \n", pOrig );
    abort(); // pOrig is out of boundaries
  }

  if (pOrig >= ZCurrentNb) {
    fprintf(stderr,"ZArray::_shiftLeft-F-OUTBOUND Error shifting left array zone from <%ld> while current number of element is <%ld> \n", pOrig, ZCurrentNb );
    return (0);
  }
  if (ZCurrentNb < 2) {
    fprintf(stderr,"ZArray::_shiftLeft-F-OUTBOUND Error shifting left array zone from <%ld> \n", pOrig );
    return 0;
  }

  long wDest = pOrig - 1;
  long wOrig = pOrig ;
  while ( wOrig < ZCurrentNb ) {
    memmove(&Tab[wDest++],&Tab[wOrig++],sizeof(_Tp));
  }
  memset(&Tab[wOrig],0,sizeof(_Tp));
  ZCurrentNb --;
  return ZIdx = wOrig ;
}// _shiftLeft

template <typename _Tp>
long ZArray<_Tp>::_moveLeft (long pDest, long pOrig, long pNumber)
{
  if ((pDest==pOrig) || (pNumber==0)) {
    ZIdx = pDest;
    return (pDest);
  }
  if (pNumber < 0) {
    fprintf(stderr,"ZArray::_moveRight-F-INVVALUE Invalid value <%ld>",pNumber );
    abort();
  }
  if (pDest > pOrig) {
    fprintf(stderr,"ZArray::_moveLeft-F-INVVALUE Impossible to move data LEFT from source <%ld> to destination <%ld>\n",pDest,pOrig );
    abort();
  }

  if ((pDest - pNumber) < 0) {
    fprintf(stderr,"ZArray::_moveLeft-F-OUTBOUND error moving array left zone from <%ld> of number of elements <%ld>",pDest,pNumber );
    abort();
  }

  for (long wi=0 ; wi < pNumber  ; wi++) {
    memmove(&Tab[pDest+wi],&Tab[pOrig+wi],sizeof(_Tp));
  }

  return ZIdx = pDest ;
}// _moveLeft

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
#if __CRASH_ON_FATAL__
        __ABORT__ ;
#endif
      return -1 ; // out of boundaries for
  }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  if ((pDest+pNumber)>ZCurrentNb) {
    addCheck((pDest+pNumber)-ZCurrentNb);
//    memset (&Tab[ZCurrentNb],0,(size_t)(pNumber-(ZAllocation-ZCurrentNb)));
  }

  _Tp *wTemporary=Allocator.allocate(pNumber);
  for (long wi=0;wi < pNumber;wi++) {
    wTemporary[wi]=std::move(Tab[pOrig+wi]);
  }
  for (long wi=0;wi < pNumber;wi++) {
    Tab[pOrig+wi]=std::move(Tab[pDest+wi]);
  }
  for (long wi=0;wi < pNumber;wi++) {
    Tab[pDest+wi]=std::move(wTemporary[wi]);
  }
  Allocator.deallocate(wTemporary,pNumber);
/*
  _Tp *wPt=(_Tp*) malloc (pNumber*(sizeof(_Tp)));
  memmove(wPt,&Tab[pOrig],(size_t)(pNumber*(sizeof(_Tp))));
  memmove(&Tab[pOrig],&Tab[pDest],(size_t)(pNumber*(sizeof(_Tp))));
  memmove(&Tab[pDest],wPt,(size_t)(pNumber*(sizeof(_Tp))));
  zfree(wPt);
*/
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
long ZArray<_Tp>::insert(_Tp &pElement, size_t pIdx)
{
  if (pIdx>ZCurrentNb) {
    return (-1);
  }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif

  ZIdx=_insertNoLock(pElement,  pIdx);
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZIdx);
}// insert

template <typename _Tp>
long ZArray<_Tp>::_insertNoLock(_Tp &pElement, size_t pIdx)
{
  if (pIdx>ZCurrentNb) {
    return (-1);
  }

  addCheck(1);
  ZCurrentNb++;

  if (ZCurrentNb > 0){
    _shiftRight(pIdx); // make room for element (no lock)
  }

  assign(pElement,pIdx);

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
  if (pIdx >= ZCurrentNb)
      return (-1);

#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  pIdx = _insertNoLock(pElement,pIdx,pNumber);
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
    return (-1);

  ZIdx = pIdx ;
  addCheck(pNumber);
  if (ZCurrentNb > 0)
    _moveRight((pIdx + pNumber),pIdx,pNumber); // make room for element

  ZCurrentNb = ZCurrentNb + pNumber;
  for (long wi=0;wi < pNumber;wi++)
    assign(pElement[wi],pIdx + wi);
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
  if (pIdx==lastIdx())
    return pop();
  if ((long)(pIdx+pNumber)>ZCurrentNb)
    return -1;

#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  pIdx=_eraseNoLock(pIdx,pNumber);

#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (pIdx);
}// erase

template <typename _Tp>
long ZArray<_Tp>::_eraseNoLock(size_t pIdx,size_t pNumber)
{
  if (pIdx==ZCurrentNb-1)
    return _pop_nolock();
  if ((long)(pIdx+pNumber)>(ZCurrentNb-1))
    return -1;

  /* destroy elements to be erased */
  for (long wi=pIdx;(wi < wi+pNumber)&&(wi < ZCurrentNb-1);wi++)
    Allocator.destroy(Tab+wi);

  ZIdx = pIdx ;

 _Tp* wPtrDest = &Tab[pIdx];
 _Tp* wPtrOrig = &Tab[pIdx + pNumber];

  size_t wNbMove = ZCurrentNb - pIdx - pNumber ;

  memmove(wPtrDest,wPtrOrig,wNbMove*sizeof(_Tp)); // restructure space to delete
//  for (long wi = 0; wi < wNbMove ; wi++)
//    Tab[pIdx+wi]=std::move(Tab[pIdx + pNumber + wi]);

  _Tp* wPtr = &Tab[ZCurrentNb - pNumber ];
  memset(wPtr,0,sizeof(_Tp)*pNumber);// set available space to zero in the End of ZArray (recuperated space)

  ZCurrentNb = ZCurrentNb - pNumber ;
  return (ZIdx=pIdx);
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

    memset(&Tab[pOrig],0,(size_t)(wN*(sizeof(_Tp))));
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
    _eraseNoLock(0,ZCurrentNb);
    ZCurrentNb=0 ;
    ZIdx = 0;
    resize (ZInitialAllocation);
//    memset (ZPtr,0,ZAllocatedSize);
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


/** @brief addCheck
 *  Controls impact of adding pAdd Elements on the existing current context
 *  Takes the appropriate reallocation actions :
 *   - if remaining not used elements are sufficient : nothing
 *   - allocate ZReallocateQuota elements if Quota is sufficient
 *   - allocate the appropriate number of elements + 1 in other case
 *
 *  This is allocator type neutral.
 *
 */
template <typename _Tp>
void ZArray<_Tp>::addCheck(long pAdd)
{
  /* following is necessary when assign() applies to a ZArray itself and it has been zeroed */
  if (ZAllocation == 0){
    fprintf(stderr,"ZArray::addCheck-W-ALLOCNULL ZAllocation is zero. Forced to _cst_default_allocation.\n");
    allocate (_cst_default_allocation) ;
  }
  if (ZReallocQuota == 0) {
    fprintf(stderr,"ZArray::addCheck-W-QUOTANULL ZReallocQuota is zero. Forced to _cst_realloc_quota.\n");
    ZReallocQuota=_cst_realloc_quota;
  }
long wFreeSlots = ZAllocation-ZCurrentNb  ;  // size remaining to fill
  if (wFreeSlots > pAdd)
            return ;

long wReallocValue = ZReallocQuota;

  while ((wFreeSlots + wReallocValue) <= pAdd)
    wReallocValue += ZReallocQuota;

  extend (wReallocValue) ;

    return ;
} // ZSizeCheck




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
template <typename _Tp>
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
  memset(&Tab[wI],0,pAlloc*sizeof(_Tp));
  ZCurrentNb=pAlloc;
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (wI);
}  // allocateCurrentElements



template<class _Tp>
inline _Tp & ZArray<_Tp>::assignReturn (const long pIdx)
{
  return ZReturn = Tab[pIdx];
}

/**
 * @brief ZArray<_Tp>::trim resizes ZArray allocated memory to the current number of elements plus one reallocation quota elements
 *  memory allocator neutral
 */
template <typename _Tp>
_Tp* ZArray<_Tp>::trim(ssize_t pReallocQuota)
{
  if (pReallocQuota < 0)
    ZAllocation = ZCurrentNb + ZReallocQuota ;
  else
    ZAllocation = ZCurrentNb + pReallocQuota ;

  return allocate (ZAllocation);
}



/**
 * @brief resize resizes ZArray allocated memory to at least current number of elements.
 *
 *  pNewAllocation :
 *    - is < 0 : allocated space is set to current number of elements plus one reallocation quota
 *    - is less than current number of elements : allocated space is set to current number of elements
 *    - is greater than current number of elements :  allocated space is set to pNewAllocation
 *
 *  memory allocator neutral
 *
 */
template <typename _Tp>
_Tp* ZArray<_Tp>::resize(ssize_t pNewAllocation)
{
  if (pNewAllocation < 0) {
    return _baseRealloc (ZCurrentNb + ZReallocQuota);
  }
  if (pNewAllocation < ZCurrentNb) {
    return _baseRealloc (ZCurrentNb);
  }
  return _baseRealloc (pNewAllocation);
}


/* ========================== End memory allocation =================== */

template<typename _Tp>
long ZArray<_Tp>::bsearch (const void *pKey, size_t pSize, size_t pOffset)
{
long widx = 0;
long wistart = -1;
const char *wKey = (const char *)pKey;
const char* wDataPtr=(const char*)Tab;
long wHighIdx = (long)size()*(long)sizeof(_Tp);

    for (long wi=pOffset; wi <wHighIdx ;wi++)
            {
            if (wKey[widx]==wDataPtr[wi])
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
const char *wDataPtr = (const char *)Tab;
long wHighIdx = (long)size()*(long)sizeof(_Tp);
char ToCompareFromKey ;
char ToCompareFromArray;

    for (long wi=pOffset; wi <wHighIdx ;wi++)
            {
            ToCompareFromArray = wDataPtr[wi];
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
  if (pAlloc<(size()+ZReallocQuota)) {
    if (ZVerbose)
      fprintf(stderr,"ZArray::setAllocation-W-BADALLOC Warning allocation given <%ld> is less than current number of elements <%ld> + allocation quota <%ld>. Trimming the ZArray...\n",
                        pAlloc,
                        ZCurrentNb,
                        ZReallocQuota);
    resize(pAlloc) ;

#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
    if (pLock)
      _Mutex.unlock();
#endif
    return ;
  }//if (pAlloc<(size()+ZReallocQuota

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
      {
      long wMax=ZCurrentNb+pNumber;
      for (long wi=ZCurrentNb; wi < wMax ; wi++)
            pClear(Tab[wi]);  // call it
      }
      else
        memset(&Tab[ZCurrentNb],0,sizeof(_Tp)*pNumber);
//        else // else memset dangerously the whole _TP data type
//            memset(&Data[ZCurrentNb],0,sizeof(_Tp)*pNumber); // this could destroy underneeth constants
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
//    Data[ZCurrentNb]=pElement;
  size_t wR =ZCurrentNb;
    ZCurrentNb ++ ;
    assign(pElement,wR);
//    memmove(&Data[ZCurrentNb],&pElement,sizeof(_Tp));
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return(wR) ;
} // push

template <typename _Tp>
long ZArray<_Tp>::_pushNoLock( _Tp &pElement)
{
  addCheck(1) ;
  if (ZCurrentNb<0)
    ZCurrentNb=0;
  assign(pElement,ZCurrentNb);
  size_t wR =ZCurrentNb;
  ZCurrentNb ++ ;
  return(wR) ;
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
long ZArray<_Tp>::push(_Tp &&pElement)
{
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif
    addCheck(1) ;
    if (ZCurrentNb<0)
            ZCurrentNb=0;
    assign(pElement,ZCurrentNb);
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


    memmove(Tab,&pElement,sizeof(_Tp));

//    memmove (&ZReturn,   &Data[ZCurrentNb],sizeof(_Tp));
//    ZReturn = Tab[ZCurrentNb];
    assignReturn(ZCurrentNb);
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
////  _hasChanged=true;
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
//        memmove (&ZReturn,   &Data[ZCurrentNb],sizeof(_Tp));
//    ZReturn = Data[ZCurrentNb];
    assignReturn(ZCurrentNb);
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
        _Mutex.unlock();
#endif
    return (ZReturn);
} // popR

template <typename _Tp>
_Tp & ZArray<_Tp>::_popR_nolock(void)
{
 // _hasChanged=true;
  if (ZCurrentNb<1)
  {
    fprintf (stderr,"popR-F-INVZCURRNB Fatal error : index of ZArray <%ld> is < 1 \n",
        ZCurrentNb);
    abort();
  }

  ZCurrentNb -- ;
  assignReturn(ZCurrentNb);
  return (ZReturn);
} // popR_nolock

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
    *pReturn = Tab[ZCurrentNb];
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
_Tp &ZArray<_Tp>::_popRP_nolock(_Tp* pReturn)
{
  if (ZCurrentNb<1)
            {
        fprintf(stderr,"popRP-F-NOELEMENTS Fatal error no elements within ZArray while pop method has been invoked.\n"  );
        exit (EXIT_FAILURE);
            }

    ZCurrentNb -- ;
//    memmove (pReturn,   &Data[ZCurrentNb],sizeof(_Tp));
    *pReturn = Tab[ZCurrentNb];
    return (*pReturn);
} // _popRP_nolock


/** @brief pop erases the last element for the ZArray. Returns the new array size or -1 if empty.
 *
 * @return new array size after pop operation or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop(void)
{
  if (ZCurrentNb < 1) {
    return -1;
  }
#if __USE_ZTHREAD__  & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _pop_nolock();

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZCurrentNb);
} // pop

/** @brief ZArray<_Tp>::_pop_nolock same as pop() but with no mutex lock */
template <typename _Tp>
long ZArray<_Tp>::_pop_nolock(void)
{
  if (ZCurrentNb<1) {
    return -1;
  }

  Allocator.destroy(&last());
  memset (&last(),0,sizeof(_Tp)); // set poped space to zero
  ZCurrentNb -- ;

  return (ZCurrentNb);
} // _pop_nolock


/** @brief pop_front Erases the first element. Returns the new array size or -1 if empty.
 * @return new array size or -1 if array was empty
 *
 */
template <typename _Tp>
long ZArray<_Tp>::pop_front(void)
{
    if (ZCurrentNb<1) {
      return -1;
    }

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif

  _pop_front_nolock();

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZCurrentNb);
} // pop_front

template <typename _Tp>
long ZArray<_Tp>::_pop_front_nolock(void)
{
  if (ZCurrentNb<1) {
    return -1;
  }

  Allocator.destroy(Tab); /* invoke destructor for first element to delete */
  for (long wi=0;wi < ZCurrentNb-1;wi++)
    memmove(&Tab[wi+1],&Tab[wi],sizeof(_Tp));   /* simple move of other elements */
  memset(&last(),0,sizeof(_Tp));  // set available space to zero
  ZCurrentNb -- ;

  return (ZCurrentNb);
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
    if (ZCurrentNb<1)
            {
            fprintf (stderr,"ZArray<_Tp>::popR_front-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
            abort();
            }
////  _hasChanged=true;
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
        _Mutex.lock();
#endif

  assignReturn(0);
  _pop_front_nolock();

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
  return (ZReturn); // bad thing when being used with ZThreads
} // popR_front

template <typename _Tp>
_Tp &ZArray<_Tp>::_popR_front_nolock(void)

{
  if (ZCurrentNb<1)
  {
    fprintf (stderr,"ZArray<_Tp>::popR_front-F-IVPOPFRNT  Invalid popR_front operation ; no elements within ZArray.\n");
    abort();
  }

  assignReturn(0);
  _pop_front_nolock();

  return (ZReturn); // bad thing when being used with ZThreads
} // _popR_front_nolock


/** @brief popRP_front ThreadSafe: Pops out the first array element and returns a pointer to the poped element's content.
 *                  returns NULL if array is empty.
 *
 *
 */
template <typename _Tp>
_Tp &ZArray<_Tp>::popRP_front(_Tp*pReturn)
{
  if (ZCurrentNb<1) {
    fprintf(stderr,"popRP_front-F-NOELEMENTS Fatal error no elements within ZArray while popRP_front method has been invoked.\n" );
    abort();
  }

#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.lock();
#endif
  _popRP_front_nolock();
#if __USE_ZTHREAD__ & __ZTHREAD_AUTOMATIC__
  _Mutex.unlock();
#endif
    return (*pReturn);
} // popRP_front

template <typename _Tp>
_Tp &ZArray<_Tp>::_popRP_front_nolock(_Tp*pReturn)
{
  if (ZCurrentNb<1) {
    fprintf(stderr,"popRP_front-F-NOELEMENTS Fatal error no elements within ZArray while popRP_front method has been invoked.\n" );
    abort();
  }
//  _hasChanged=true;
  memset(pReturn,0,sizeof(_Tp));
  Allocator.construct(pReturn);
  pReturn[0]=Tab[0];
  _pop_front_nolock();

  return (*pReturn);
} // popRP_front
template <typename _Tp>
long
ZArray<_Tp>::searchforValue (long pOffset, long pSize, const void *pValue) //! sequential search (costly) for a value
{
  const unsigned char* wPtrArray;
  const unsigned char* wPtrValue = static_cast<const unsigned char*>(pValue);
  for (long wi=0;wi<size();wi++) {
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
    for (long wi=0;wi<pZArray->size();wi++)
        {
        _exportConvert(pZArray->Tab[wi],&wEltPtr[wi]);
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


#if __USE_ZTHREAD__
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
    pZArray->bzero(0,-1,false);                                   // no lock
    pZArray->setQuota(wZAE.ExtentQuota);
    pZArray->setInitialAllocation(wZAE.InitialAllocation,false);  // no lock
    if (wZAE.NbElements>(ssize_t)wZAE.AllocatedElements)
        {
        pZArray->setAllocation(wZAE.NbElements,false);
        }
    pZArray->newBlankElement(wZAE.NbElements);

    _TpIn* wEltPtr_In=(_TpIn*)(pPtrIn+sizeof(ZAExport));
    _TpOut* wEltPtr_Tab= pZArray->Tab;
    _TpOut wElt;
    for (long wi=0;wi<wZAE.NbElements;wi++)
        {
        _importConvert(wEltPtr_Tab[wi],&wEltPtr_In[wi]);
        }// for


#ifdef __USE_ZTHREAD__
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
  wZAE.NbElements = ZCurrentNb;
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
