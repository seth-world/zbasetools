#ifndef _UTFSTRCTX_CPP_
#define _UTFSTRCTX_CPP_

#include <ztoolset/zerror.h>
#include <ztoolset/utfstrctx.h>

#include <string.h> // for memmove, malloc,...

//===============utfSCErrQueue=============================
 utfSCErr_struct& utfSCErr_struct::operator = (utfSCErr_struct& pErr)
 {
     memmove(this,&pErr,sizeof(utfSCErr_struct));
     return *this;
 }

void
utfSCErrQueue::newError(size_t pUnitOffset,
                        size_t pSize,
                        UST_Status_type pStatus,
                        utfSCEToFrom_type pInOut,
                        utf32_t pCodepoint,
                        ZCNV_Action_type pAction)
{
    utfSCErr_struct wError;
    wError.UnitOffset=pUnitOffset;
    wError.Size=pSize;
    wError.Status=pStatus;
    wError.ToFrom=pInOut;
    wError.codePoint=pCodepoint;
    wError.Action=pAction;
    push(wError);
    return;
}

void
utfSCErrQueue::push (utfSCErr_struct& pContent)
{
    Size++;
    size_t newByteSize= sizeof(utfSCErr_struct)*Size;
    Queue=(utfSCErr_struct*)realloc (Queue,newByteSize);
//    memmove(&Queue[Size-1],pContent,sizeof(utfSCErr_struct));
    Queue[Size-1]=pContent;
}
utfSCErr_struct
utfSCErrQueue::pop (void)
{
utfSCErr_struct wReturn;
//    memmove(&wReturn, &Queue[Size-1],sizeof(utfSCErr_struct));
    wReturn = Queue[Size-1];
    Size--;
    size_t newByteSize= sizeof(utfSCErr_struct)*Size;
    Queue=(utfSCErr_struct*)realloc (Queue,newByteSize);
    return wReturn;
}
void
utfSCErrQueue::clear (void)
{
    if (Queue)
            free(Queue);
    Queue=nullptr;
    Size=0;
}
utfSCErr_struct* utfSCErrQueue::getLast(void)
{
    return &Queue[Size-1];
}
utfSCErr_struct* utfSCErrQueue::get(size_t pIndex)
{
    if (pIndex>=Size)
                    return nullptr;
    return &Queue[pIndex];
}

utfSCErrQueue*
utfSCErrQueue::_clone (void)
{

    utfSCErrQueue* wNewQueue= new utfSCErrQueue;
    size_t wByteSize= sizeof(utfSCErr_struct)*Size;
    wNewQueue->Queue=(utfSCErr_struct*)malloc (wByteSize);
    if (!wNewQueue->Queue)
      abort();
    memmove(wNewQueue->Queue,Queue,wByteSize);
    return wNewQueue;
}

void
utfSCErrQueue::dump(FILE* pOutput)
{
    fprintf (pOutput,
             "_________Error Queue__________\n");
    if (!Queue)
        fprintf(pOutput,
                "Error Queue is empty\n");
    else
    {
    fprintf(pOutput,
            "%5s %9s %9s %9s %22s %20s %8s\n",
            "dir.",
            "unit off.",
            "byte off."
            "unit size"
            "status (code - desc)",
            "action (ZCNV_Action)",
            "codePoint");
    for (size_t wi=0;wi<Size;wi++)
    {
        fprintf(pOutput,
                "%5s %9ld %9ld %9ld %6X %15s %20s %8X\n",
                Queue->ToFrom?"To":"From",
                Queue->UnitOffset,
                Queue->ByteOffset,
                Queue->Size,
                Queue->Status, decode_UST(Queue->Status),
                Queue->Action,
                Queue->codePoint);
    }
    }
    fprintf (pOutput,
             "______________________________\n");
    return;
}//utfSCErrQueue::dump

//==============utfStrCtx===================================

void utfStrCtx::init(void)
{
    CtxCmd::clear();

    Start=0;    
    Effective=0;

    InPtr=0;
    InUnitOffset=0;
    InByteOffset=0;

    OutPtr = 0;
    OutByteOffset = 0;
    OutUnitOffset = 0;

    Strlen=0;
    CharUnits=0;
    Status=0;

    CodePoint=0;
    Count=0;
    EndianRequest=0;
    LittleEndian=0;
    ErrFrom=0;
    ErrTo=0;
    Substit=0;
    Escaped=0;
    Skipped=0;

    BOM=ZBOM_NoBOM;

    if (ErrorQueue)
         delete ErrorQueue;

    _free(SavedChunk);
    SavedChunkSize=0;
    SavedChunkFullSize=0;

    Cloned=false;    // not to be reset
}//utfStrCtx::init

void utfStrCtx::reset(void)
{
    Start=0;
    Effective=0;

    InPtr=0;
    InUnitOffset=0;
    InByteOffset=0;

    OutPtr = 0;
    OutByteOffset = 0;
    OutUnitOffset = 0;

    Strlen=0;
    CharUnits=0;
    Status=0;
    Reverse=0;

    CodePoint=0;
    Count=0;
    EndianRequest=0;
    LittleEndian=0;
    ErrFrom=0;
    ErrTo=0;
    Substit=0;
    Escaped=0;
    Skipped=0;

    BOM=ZBOM_NoBOM;

    if (ErrorQueue)
         delete ErrorQueue;

    _free(SavedChunk);
    SavedChunkSize=0;
    SavedChunkFullSize=0;
}//utfStrCtx::reset

void utfStrCtx::resetIn(void)
{
    Start=0;
    Effective=0;

    InPtr=0;
    InUnitOffset=0;
    InByteOffset=0;

    Strlen=0;
    Status=0;
    CodePoint=0;

}//utfStrCtx::resetIn

void utfStrCtx::resetOut(void)
{
    OutPtr = 0;
    OutByteOffset = 0;
    OutUnitOffset = 0;

    Status=0;

    CodePoint=0;
    Count=0;
}//utfStrCtx::resetOut


utfStrCtx* utfStrCtx::_from(utfStrCtx* pInCtx)
{
    Start=pInCtx->Start;
    InPtr=pInCtx->InPtr;
    Effective=pInCtx->Effective;
    Strlen=pInCtx->Strlen;
    CharUnits=pInCtx->CharUnits;
    Status=pInCtx->Status;
    Reverse=pInCtx->Reverse;
    CodePoint=pInCtx->CodePoint;
    Count=pInCtx->Count;
    EndianRequest=pInCtx->EndianRequest;
    LittleEndian=pInCtx->LittleEndian;
    ErrFrom=pInCtx->ErrFrom;
    ErrTo=pInCtx->ErrTo;
    Substit=pInCtx->Substit;
    Escaped=pInCtx->Escaped;
    Skipped=pInCtx->Skipped;
    StopOnConvErr=pInCtx->StopOnConvErr;
    BOM=pInCtx->BOM;
    Action=pInCtx->Action;
    InUnitOffset=pInCtx->InUnitOffset;
    TraceFrom=pInCtx->TraceFrom;
    TraceTo=pInCtx->TraceTo;
    if (ErrorQueue)
            {
            delete ErrorQueue;
            ErrorQueue = pInCtx->ErrorQueue->_clone();
            }
//    Cloned=true;   // Cloned must not be touched
    return this;
}//_clone

utfStrCtx* utfStrCtx::_clone(void)
{
    utfStrCtx* wNew= new utfStrCtx;

    wNew->StopOnConvErr=StopOnConvErr;
    wNew->Reverse=Reverse;
    wNew->TraceFrom=TraceFrom;
    wNew->TraceTo=TraceTo;
    wNew->Action=Action;

    wNew->Start=Start;
    wNew->InPtr=InPtr;
    wNew->Effective=Effective;
    wNew->Strlen=Strlen;
    wNew->CharUnits=CharUnits;
    wNew->Status=Status;

    wNew->CodePoint=CodePoint;
    wNew->Count=Count;
    wNew->EndianRequest=EndianRequest;
    wNew->LittleEndian=LittleEndian;
    wNew->ErrFrom=ErrFrom;
    wNew->ErrTo=ErrTo;
    wNew->Substit=Substit;
    wNew->Skipped=Skipped;
    wNew->Escaped=Escaped;

    wNew->BOM=BOM;

    wNew->InUnitOffset=InUnitOffset;
    wNew->InByteOffset=InByteOffset;

    if (ErrorQueue)
            wNew->ErrorQueue = ErrorQueue->_clone();
    wNew->Cloned=true;
    return wNew;
}//_clone

void utfStrCtx::dump(FILE* pOutput)
{
    char* wMessage=_dump();
    fprintf(pOutput,wMessage);
    free(wMessage);
    if (ErrorQueue)
         ErrorQueue->dump(pOutput);
    return;
}

char* utfStrCtx::_dump(void)
{
    char* wMessage=(char*)malloc(1001);
    char wEndian[101];
    if (!EndianRequest)
         strcpy(wEndian,"none");
    else
         snprintf(wEndian,100,
                 "%s",
                  LittleEndian?"little endian":"big endian");
    snprintf(wMessage,1000,
             "Current offset        <%ld>\n"
             "String size (units)   <%ld>\n"
             "Endian request made   <%s>\n"
             "Little endian request <%s>  current system endian <%s>\n"
             "BOM detected          <%s>\n"
             "Direction             <%s>\n"
             "Last status           <0x%X> <%s>\n\n"

             "utf32 value           <0x%X>\n"
             "Input units           <%ld>\n"
             "Output units          <%ld>\n\n"

             "Conversion faults\n"
             " from                 <%d>\n"
             " to                   <%d>\n"
             " substitutions made   <%d>\n"

             "Faults trace\n"
             " ToUnicode            <%s>\n"
             " FromUnicode          <%s>\n"
             ,
             getOffset(),// 4
             Strlen,
             EndianRequest?"yes":"use system default",
             wEndian,
             is_little_endian()?"little endian":"big endian",
             decode_ZBOM(BOM),
             Reverse?"Reverse":"Forward",
             Status,
             decode_UST( Status),

             CodePoint,
             CharUnits,
             Count,
             ErrFrom,
             ErrTo,
             Substit,
             TraceTo?"On":"Off",
             TraceFrom?"On":"Off");
    return wMessage;
}// _dump

//========= Codes conversion & display====================

const char*
decode_ZCNV(ZCNV_Action pAction)
{
    switch(pAction)
    {
    case ZCNV_Nothing:
            return "ZCNV_Nothing";
    case ZCNV_Stop:
            return "ZCNV_Stop";
    case ZCNV_Skip:
            return "ZCNV_Skip";
    case ZCNV_Substitute:
            return "ZCNV_Substitute";
    case ZCNV_Escape_ICU:
            return "ZCNV_Esc_ICU";
    case ZCNV_Escape_Java:
            return "ZCNV_Esc_Java";
    case ZCNV_Escape_C:
            return "ZCNV_Esc_C";
    case ZCNV_Escape_XML_Dec:
            return "ZCNV_Esc_XML_Dec";
    case ZCNV_Escape_XML_Hex:
            return "ZCNV_Esc_XML_Hex";
    case ZCNV_Escape_Unicode:
            return "ZCNV_Esc_XML_Uni";
    case ZCNV_Escape_CSS2:
            return "ZCNV_Esc_XML_CSS2";
    default:
            return "Unknown ZCNV_Action";
    }
}

ZStatus UST_to_ZStatus(UST_Status_type pUST)
{
    switch (pUST)
    {
    case UST_SUCCESS:
            return ZS_SUCCESS;
    case UST_NULLPTR:
            return ZS_NULLPTR;
    case UST_MEMERROR:
        return ZS_MEMERROR;
    case UST_EMPTY:
        return ZS_EMPTY;

    case UST_IVBOMUTF8:
    case UST_IVBOMUTF16LE:
    case UST_IVBOMUTF16BE:
    case UST_IVBOMUTF32LE:
    case UST_IVBOMUTF32BE:
        return ZS_IVBOM;

    case UST_ENDOFSTRING:
        return ZS_EOF;

    case UST_ENCODING:
        return ZS_INVCHARSET;

    case UST_UTF8CAPAOVFLW:
    case UST_UTF8IVCONTBYTES:
    case UST_MISSCONTBYTE:
    case UST_IVUTF16SUR_1:
    case UST_IVUTF16SUR_2:
    case UST_ILLEGAL:
    case UST_IRREGULAR:
    case UST_TRUNCATEDCHAR:
        return  ZS_ILLEGAL;

    case UST_IVCODEPOINT:
        return ZS_IVCODEPOINT;
    case UST_CONVERROR:
        return ZS_CONVERR;

    case UST_TRUNCATED:
        return ZS_STRTRUNCATED;

    default:
        return ZS_CONVERR;

    }


}
#endif // _UTFSTRCTX_CPP_
