#ifndef ZHTMLREPORT_H
#define ZHTMLREPORT_H
/** @file zhtmlreport.h  html report creation using html tables
 *    see  examples/examplehtml.cpp
*/
#include "utfvaryingstring.h"



class ZHtmlCell
{
public:
  ZHtmlCell(){}
  ZHtmlCell(const ZHtmlCell & pIn);
  ZHtmlCell(const ZHtmlCell && pIn);
  ZHtmlCell(const char*pContent,const char*pStyle=nullptr);
  ZHtmlCell(long pContent,const char*pStyle=nullptr);
  ZHtmlCell(int pContent,const char*pStyle=nullptr);

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
  utf8VaryingString get();


  utf8VaryingString Content;
  utf8VaryingString Style;
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
  void addCell(long pNum,const char*pStyle=nullptr);
  void addCellAt(int pColumn,const ZHtmlCell& pCell);
  void addCellAt(int pColumn,const char* pText,const char*pStyle=nullptr);
  void addCellAt(int pColumn,long pNum,const char*pStyle=nullptr);
  void addCellAt(int pColumn,int pNum,const char*pStyle=nullptr);

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

  utf8VaryingString get(int pColumnCount);
};//ZHtmlTableHeaderRow

class ZHtmlTableCommonRow : public ZHtmlTableRow
{
public:
  ZHtmlTableCommonRow(int pColumnCount) : ZHtmlTableRow(pColumnCount,false) {}
  ZHtmlTableCommonRow(ZHtmlTableCommonRow& pIn): ZHtmlTableRow(pIn) {}
  ZHtmlTableCommonRow(ZHtmlTableCommonRow&& pIn): ZHtmlTableRow(pIn) {}

  using ZHtmlTableRow::operator <<;

  utf8VaryingString get(int pColumnCount, bool pGrey=false, bool pLast=false);

};//ZHtmlTableCommonRow
/**
 * @brief The ZHtmlTable class generated by ZHtmlReport for presenting an html table within html report
 */
class ZHtmlTable
{
public:
  ZHtmlTable(int pColumnCount=0) {ColumnCount=pColumnCount; }
  ~ZHtmlTable();

  void addCaption(const utf8VaryingString& pCaption) {Caption=new utf8VaryingString(pCaption);}

  void setHeaderBackColor(const utf8VaryingString&pColor) {HeaderBackColor=new utf8VaryingString(pColor);}

  void addHeaderRow(ZHtmlTableHeaderRow* pRow);
  void addCommonRow(ZHtmlTableCommonRow* pRow);

  void setAlternateRow(bool pOnOff) {AlternateRows=pOnOff;}

  void setCellWidth(double pWidth,const char* pUnit="in")  {CellWidth=pWidth;CellWidthUnit=pUnit;}

  utf8VaryingString getCellWidth()
  {
    utf8VaryingString wWidth;
    wWidth.sprintf("%g%s",CellWidth,CellWidthUnit);
    wWidth.replace(",",".");
    return wWidth;
  }

  utf8VaryingString get();

  ZHtmlTable& operator << (ZHtmlTableCommonRow& pRow) {addCommonRow(new ZHtmlTableCommonRow(pRow)); return *this;}
  ZHtmlTable& operator << (ZHtmlTableHeaderRow& pRow) {addHeaderRow(new ZHtmlTableHeaderRow(pRow)); return *this;}

  bool        AlternateRows=false;
  double      CellWidth=2.2313 ;
  const char* CellWidthUnit="in";
  utf8VaryingString* Caption=nullptr;
  utf8VaryingString* HeaderBackColor=nullptr;
  zbs::ZArray<ZHtmlTableHeaderRow*> Headers;
  zbs::ZArray<ZHtmlTableCommonRow*>  Rows;
  int ColumnCount = 0;
}; //ZHtmlTable


/**
 * @brief The ZHtmlReport class manage the creation of an html report including html table
 */
class ZHtmlReport
{
public:
  ZHtmlReport()=default;
  ~ZHtmlReport();

  void setTitle(const utf8VaryingString& pTitle);
  void setIntroduction(const utf8VaryingString& pIntro);
  void setSignature(const utf8VaryingString& pSignature);
  void addParagraphBefore(const utf8VaryingString& pParagraph);
  void addParagraphAfter(const utf8VaryingString& pParagraph);
  void addTable(int pColumns);
  void setFont (const utf8VaryingString& pFont);

  utf8VaryingString get();

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




  utf8VaryingString* Font=nullptr;
  utf8VaryingString* Title=nullptr;
  utf8VaryingString* Introduction=nullptr;
  utf8VaryingString* ParBefore=nullptr;
  utf8VaryingString* ParAfter=nullptr;
  utf8VaryingString* Signature=nullptr;
  ZHtmlTable* Table=nullptr;
}; //ZHtmlReport


utf8VaryingString setupHtmlString(const utf8VaryingString &pContent);

#endif // ZHTMLREPORT_H
