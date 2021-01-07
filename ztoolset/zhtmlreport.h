#ifndef ZHTMLREPORT_H
#define ZHTMLREPORT_H

#include <ztoolset/zutfstrings.h>


//const char* HtmlHeader="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"";


/*
 *
 */




class ZHtmlCell
{
public:
  ZHtmlCell(){}
  ZHtmlCell(const char*pContent,const char*pStyle=nullptr);
  ZHtmlCell(const ZHtmlCell & pIn);
  ZHtmlCell(const ZHtmlCell && pIn);

  ZHtmlCell& operator =(ZHtmlCell &pIn)
  {
    Content=pIn.Content;
    Style=pIn.Style;
  }
  ZHtmlCell& operator =(ZHtmlCell &&pIn)
  {
    Content=pIn.Content;
    Style=pIn.Style;
  }
  utf8String get();


  utf8String Content;
  utf8String Style;
};//ZHtmlCell

enum HtmlRowType
{
  HRTP_Nothing = 0,
  HRTP_Header = 1,
  HRTP_Body = 2
};

class ZHtmlTableRow
{
public:
  ZHtmlTableRow(int pColumnCount,bool pIsHeader=false)     {IsHeader=pIsHeader; ColumnCount=pColumnCount;}
  ~ZHtmlTableRow();
  ZHtmlTableRow(ZHtmlTableRow& pIn);


  void addCell(const char* pText,const char*pStyle=nullptr);
  void addCellAt(int pColumn,const ZHtmlCell& pCell);
  void addCellAt(int pColumn,const char* pText,const char*pStyle=nullptr);
  void clear();

  ZHtmlTableRow& operator << (const char* pText) {addCell(pText); return *this;}

  zbs::ZArray<ZHtmlCell*>  Cells;
  bool IsHeader=false;
  int ColumnCount=0;
};//ZHtmlTableRow

class ZHtmlTableHeaderRow : public ZHtmlTableRow
{
public:
  ZHtmlTableHeaderRow(int pColumnCount) : ZHtmlTableRow(pColumnCount,true) {}
  ZHtmlTableHeaderRow(ZHtmlTableHeaderRow& pIn): ZHtmlTableRow(pIn) {}
  ZHtmlTableHeaderRow(ZHtmlTableHeaderRow&& pIn): ZHtmlTableRow(pIn) {}

  using ZHtmlTableRow::operator <<;

  utf8String get(int pColumnCount);
};//ZHtmlTableHeaderRow

class ZHtmlTableCommonRow : public ZHtmlTableRow
{
public:
  ZHtmlTableCommonRow(int pColumnCount) : ZHtmlTableRow(pColumnCount,false) {}
  ZHtmlTableCommonRow(ZHtmlTableCommonRow& pIn): ZHtmlTableRow(pIn) {}
  ZHtmlTableCommonRow(ZHtmlTableCommonRow&& pIn): ZHtmlTableRow(pIn) {}

  using ZHtmlTableRow::operator <<;

  utf8String get(int pColumnCount,bool pGrey=false);

};//ZHtmlTableCommonRow

class ZHtmlTable
{
public:
  ZHtmlTable(int pColumnCount=0) {ColumnCount=pColumnCount; }
  ~ZHtmlTable();

  void addCaption(const utf8String& pCaption) {Caption=new utf8String(pCaption);}

  void setHeaderBackColor(const utf8String&pColor) {HeaderBackColor=new utf8String(pColor);}

  void addHeaderRow(ZHtmlTableHeaderRow* pRow);
  void addCommonRow(ZHtmlTableCommonRow* pRow);

  void setAlternateRow(bool pOnOff) {AlternateRows=pOnOff;}

  utf8String get();

  ZHtmlTable& operator << (ZHtmlTableCommonRow& pRow) {addCommonRow(new ZHtmlTableCommonRow(pRow)); return *this;}
  ZHtmlTable& operator << (ZHtmlTableHeaderRow& pRow) {addHeaderRow(new ZHtmlTableHeaderRow(pRow)); return *this;}

  bool        AlternateRows=false;
  utf8String* Caption=nullptr;
  utf8String* HeaderBackColor=nullptr;
  zbs::ZArray<ZHtmlTableHeaderRow*> Headers;
  zbs::ZArray<ZHtmlTableCommonRow*>  Rows;
  int ColumnCount = 0;
}; //ZHtmlTable



class ZHtmlReport
{
public:
  ZHtmlReport()=default;
  ~ZHtmlReport();

  void addTitle(const utf8String& pTitle);
  void addParagraphBefore(const utf8String& pParagraph);
  void addParagraphAfter(const utf8String& pParagraph);
  void addTable(int pColumns);
  void setFont (const utf8String& pFont);

  utf8String get();

  ZHtmlReport& operator << (ZHtmlTableHeaderRow& pRow)
  {
    Table->addHeaderRow(new ZHtmlTableHeaderRow(pRow));
    return *this;
  }
  ZHtmlReport& operator << (ZHtmlTableCommonRow& pRow)
  {
    Table->addCommonRow(new ZHtmlTableCommonRow(pRow));
    return *this;
  }




  utf8String* Font=nullptr;
  utf8String* Title=nullptr;
  utf8String* ParBefore=nullptr;
  utf8String* ParAfter=nullptr;
  ZHtmlTable* Table=nullptr;
}; //ZHtmlReport


utf8String setupHtmlString(utf8String& wContent);

#endif // ZHTMLREPORT_H
