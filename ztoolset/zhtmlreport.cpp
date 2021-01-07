#include "zhtmlreport.h"

const char* HtmlHeader="<!DOCTYPE html>\n"
                         "<html lang=\"en\">\n"
                         "<head> \n";

const char* HtmlStyleTable=
    "<style>\n"
    "table { border-collapse:collapse; }\n"
//    "table thead th { border-top: 1px solid #000;border-bottom: 1px solid #000;background-color:%s;text-align:left; width:2.2313in;}\n"
    "table thead th { border-top: 1px solid #000;border-bottom: 1px solid #000;background-color:%s;text-align:left; width:%s;}\n"
//    "table tr { text-align:left; width:2.2313in;}\n"
    "table tr { text-align:left; width:%s;}\n";

//    "th style=\"text-align:center; width:2.2313in; \" "
//    "td style=\"text-align:right; width:2.2313in; \" "
const char* HtmlStyleGrid=
    ".Gridtransparent { background-color:transparent; font-family:%s;}\n"
    ".Gridtransparentlast { background-color:transparent;border-bottom: 1px solid #000; font-family:%s;}\n"
    ".Gridgrey { background-color:LightYellow; font-family:%s;}\n"
    ".Gridgreylast { background-color:LightYellow;border-bottom: 1px solid #000; font-family:%s;}\n"
    "</style>\n";

const char* HtmlEndHeader=
    "</head>\n";
const char* HtmlBodyHeader=
    "<!--Message body-->\n"
    "<body style=\"font-family:%s\">\n";

//    "<p>%s</p>";

const char* HtmlTableHeader=
    "<table>\n"
    "<caption>%s</caption>\n"
    "<thead>";


const char* HtmlTableHeaderTrailer=
    "</thead>\n";

const char* HtmlTableTrailer= "</table>\n";
const char* HtmlTrailer=
    "</body>\n"
    "</html>";

const char* tablerow = "<tr>\n%s\n</tr>";







ZHtmlCell::ZHtmlCell(const char*pContent,const char*pStyle)
{
  Content=pContent;
  if (pStyle)
    Style=pStyle;
}
ZHtmlCell::ZHtmlCell(long pContent,const char*pStyle)
{
  Content.sprintf("%ld",pContent);
  if (pStyle)
    Style=pStyle;
}
ZHtmlCell::ZHtmlCell(int pContent,const char*pStyle)
{
  Content.sprintf("%d",pContent);
  if (pStyle)
    Style=pStyle;
}
ZHtmlCell::ZHtmlCell(const ZHtmlCell & pIn)
{
  Content=pIn.Content;
  Style=pIn.Style;
}
ZHtmlCell::ZHtmlCell(const ZHtmlCell && pIn)
{
  Content=pIn.Content;
  Style=pIn.Style;
}



utf8String ZHtmlCell::get()
{

  if (Content.isEmpty())
    return (utf8String(""));
  return setupHtmlString(Content);
}






ZHtmlTableRow::~ZHtmlTableRow()
{
  while (Cells.count())
    delete Cells.popR();
}
ZHtmlTableRow::ZHtmlTableRow(ZHtmlTableRow& pIn)
{
  ColumnCount=pIn.ColumnCount;
  IsHeader=pIn.IsHeader;
  for (long wi=0;wi<pIn.Cells.count();wi++)
    Cells.push(new ZHtmlCell(*pIn.Cells[wi]));
}


void ZHtmlTableRow::addCell(const char* pText,const char*pStyle)
{

  Cells.push(new ZHtmlCell(setupHtmlString(pText).toCChar(),pStyle));
}
void ZHtmlTableRow::addCell(long pNum,const char*pStyle)
{
  Cells.push(new ZHtmlCell(pNum,pStyle));
}
void ZHtmlTableRow::addCellAt(int pColumn,const ZHtmlCell& pCell)
{
  while(pColumn >= Cells.count())
    Cells.push(new ZHtmlCell(""));
  if (Cells[pColumn])
    delete Cells[pColumn];
  Cells[pColumn]= new ZHtmlCell(pCell);
}
void ZHtmlTableRow::addCellAt(int pColumn,const char* pText,const char*pStyle)
{
  addCellAt(pColumn,ZHtmlCell(pText,pStyle));
}
void ZHtmlTableRow::addCellAt(int pColumn,long pNum,const char*pStyle)
{
  addCellAt(pColumn,ZHtmlCell(pNum,pStyle));
}
void ZHtmlTableRow::addCellAt(int pColumn,int pNum,const char*pStyle)
{
  addCellAt(pColumn,ZHtmlCell(pNum,pStyle));
}

void ZHtmlTableRow::clear()
{
  while(Cells.count())
    delete Cells.popR();
}






utf8String ZHtmlTableHeaderRow::get(int pColumnCount)
{
  //    if (Cells.count()0)
  //      return utf8String("");
  utf8String wReturn;
  wReturn = "<tr>\n";
  long wi=0;
  for (;(wi < pColumnCount)&&(wi<Cells.count());wi++)
  {
    if (!Cells[wi]->Style.isEmpty())
      wReturn.addsprintf("<th style=%s>%s</th>\n",Cells[wi]->Style.toCChar(),Cells[wi]->get().toCChar());
    else
      wReturn.addsprintf("<th>%s</th>\n",Cells[wi]->get().toCChar());
  }
  while(wi++<pColumnCount)
    wReturn+="<th></th>\n";

  wReturn += "</tr>\n";
  return wReturn;
}// ZHtmlTableHeaderRow::get()


utf8String ZHtmlTableCommonRow::get(int pColumnCount,bool pGrey,bool pLast)
{
  //    if (Cells.count()==0)
  //      return utf8String("");
  utf8String wReturn;
  if (pGrey)
    {
    if (pLast)
      wReturn = "<tr class=\"Gridgreylast\">";
      else
      wReturn = "<tr class=\"Gridgrey\">";
    }
  else
    {
      if (pLast)
        wReturn = "<tr class=\"Gridtransparentlast\">";
      else
        wReturn = "<tr class=\"Gridtransparent\">";
    }

  long wi=0;
  for (;(wi < pColumnCount)&&(wi<Cells.count());wi++)
  {
    if (!Cells[wi]->Style.isEmpty())
      wReturn.addsprintf("<td style=%s>%s</td>\n",Cells[wi]->Style.toCChar(),Cells[wi]->get().toCChar());
    else
      wReturn.addsprintf("<td>%s</td>\n",Cells[wi]->get().toCChar());
  }
  while(wi++<pColumnCount)
    wReturn+="<td></td>\n";

  wReturn += "</tr>\n";
  return wReturn;
}// ZHtmlTableCommonRow::get()




ZHtmlTable::~ZHtmlTable()
{
  while (Rows.count())
    delete Rows.popR();
  while (Headers.count())
    delete Headers.popR();
  if (Caption)
    delete Caption;
  if (HeaderBackColor)
    delete HeaderBackColor;
}



void ZHtmlTable::addHeaderRow(ZHtmlTableHeaderRow* pRow)
{
  Headers.push(pRow);
}

void ZHtmlTable::addCommonRow(ZHtmlTableCommonRow* pRow)
{
  Rows.push(pRow);
}

utf8String ZHtmlTable::get()
{
  utf8String wReturn;

  int Maxcol=0;

  /* get max number of columns */
  for (long wi=0;wi<Rows.count();wi++)
  {
    if(Rows[wi]->Cells.count()>Maxcol)
      Maxcol=Rows[wi]->Cells.count();
  }
  if (Maxcol > ColumnCount)
    ColumnCount = Maxcol;


  wReturn = "<table>\n";

  if (Caption)
    if (!Caption->isEmpty())
      wReturn.addsprintf( "<caption>%s</caption>\n",Caption->toCChar());
  wReturn += "<thead>\n";

  for (long wi=0;wi < Headers.count();wi++)
  {
    wReturn += Headers[wi]->get(ColumnCount);
  }
  wReturn += "</thead>\n";

  bool wGrey=false;
  for (long wi=0;wi < Rows.count();wi++)
    {
    wReturn += Rows[wi]->get(ColumnCount,wGrey,(wi==Rows.count()-1));
    if (AlternateRows)
              wGrey = !wGrey;
    }


  wReturn += "</table>\n";
  return wReturn;
}//ZHtmlTable::get()


ZHtmlReport::~ZHtmlReport()
{
  if (Table)
    delete Table;
  if (Title)
    delete Title;
  if (ParBefore)
    delete ParBefore;
  if (ParAfter)
    delete ParAfter;
  if (Font)
    delete Font;
}

void ZHtmlReport::setTitle(const utf8String& pTitle)
{
  if (Title)
    delete Title;
  Title=new utf8String;
  Title->add(setupHtmlString(pTitle));
}

void ZHtmlReport::setIntroduction(const utf8String& pIntro)
{
  if (Introduction)
    delete Introduction;
  Introduction=new utf8String;
  Introduction->add(setupHtmlString(pIntro));
}
void ZHtmlReport::setSignature(const utf8String& pSignature)
{
  if (Signature)
    delete Signature;
  Signature=new utf8String;
  Signature->add(setupHtmlString(pSignature));
}

void ZHtmlReport::addParagraphBefore(const utf8String& pParagraph)
{
/*  if (ParBefore)
    delete ParBefore;*/
  if (!ParBefore)
      ParBefore=new utf8String;
  ParBefore->addsprintf("<p>%s</p>",setupHtmlString(pParagraph).toCChar());
}
void ZHtmlReport::addParagraphAfter(const utf8String& pParagraph)
{
/*  if (ParAfter)
    delete ParAfter;
  ParAfter=new utf8String(pParagraph);*/
  if (!ParAfter)
    ParAfter=new utf8String;

  ParAfter->addsprintf("<p>%s</p>",setupHtmlString(pParagraph).toCChar());
}

void ZHtmlReport::addTable(int pColumns)
{
  Table=new ZHtmlTable(pColumns);
}
void ZHtmlReport::setFont (const utf8String& pFont)
{
  if (Font)
    delete Font;
  Font=new utf8String(pFont);
}

utf8String ZHtmlReport::get()
{
  utf8String wReturn;
  wReturn = HtmlHeader;

  utf8String wHeaderBackColor="lightskyblue";
  utf8String wFont="verdana";
  if (Font)
    wFont=*Font;
  if (Table)
    {
    if (Table->HeaderBackColor)
      wHeaderBackColor=*Table->HeaderBackColor;

    wReturn.addsprintf(HtmlStyleTable,
        wHeaderBackColor.toCChar(),
        Table->getCellWidth().toCChar(),
        Table->getCellWidth().toCChar());
    wReturn.addsprintf(HtmlStyleGrid,
        wFont.toCChar(),
        wFont.toCChar(),
        wFont.toCChar(),
        wFont.toCChar());
    }

  if (Title)
    {
    wReturn.addsprintf("<title>\n%s</title>",setupHtmlString(*Title).toCChar());
    }

  wReturn+="</head>\n";

  if (Font)
    wReturn.addsprintf(HtmlBodyHeader,Font->toCChar());
  else
    wReturn += "<!--Message body-->\n<body>\n";

  if (Introduction)
  {
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*Introduction).toCChar());
//    wReturn.addsprintf("<p>%s</p>\n",Introduction->toCChar());
 //   wReturn += *Introduction;
  }

  if (ParBefore)
  {
//    wReturn.addsprintf("<p>%s</p>\n",ParBefore->toCChar());
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*ParBefore).toCChar());
//    wReturn += *ParBefore;
  }

  if (Table)
  {
    wReturn += Table->get();
  }
  if (ParAfter)
  {
//    wReturn.addsprintf("<p>%s</p>\n",ParAfter->toCChar());
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*ParAfter).toCChar());
//    wReturn += *ParAfter;
  }
  if (Signature)
  {
//    wReturn.addsprintf("<p>%s</p>\n",Signature->toCChar());
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*Signature).toCChar());
//    wReturn += *Signature;
  }
  wReturn +=   HtmlTrailer;

  return wReturn;
}//ZHtmlReport::get()
























utf8String FmtHtmlColHeadCell(const char* pText,const char*pStyle=nullptr)
{
  utf8String wReturn;
  if (pStyle)
    wReturn.sprintf("<th style=%s>%s</th>\n",pStyle,pText);
  else
    wReturn.sprintf("<th>%s</th>\n",pText);
  return wReturn;
}

utf8String
FmtHtmlColRowCell(const char* pText,const char*pStyle=nullptr)
{
  utf8String wReturn;
  const char* wDefaultheaderstyle="\"text-align:left;\"";
  if (pStyle)
    wReturn.sprintf("<td style=%s>%s</td>\n",pStyle,pText);
  else
    wReturn.sprintf("<td style=%s>%s</td>\n",wDefaultheaderstyle,pText);
  return wReturn;
}
utf8String FmtHtmlColRowCell1(const char* pText,const char*pStyle=nullptr)
{
  utf8String wReturn;
  const char* wDefaultheaderstyle="\"background-color: Light Sky Blue;text-align:left;\"";
  if (pStyle)
    wReturn.sprintf("<td style=%s>%s</td>\n",pStyle,pText);
  else
    wReturn.sprintf("<td style=%s>%s</td>\n",wDefaultheaderstyle,pText);
  return wReturn;
}


utf8String FmtHtmlHeaderRow(const char* pHead1,const char*pHead2,const char*pHead3)
{
  utf8String wReturn;
  wReturn = "<tr>";
  wReturn = FmtHtmlColHeadCell(pHead1);

  wReturn +=  FmtHtmlColHeadCell(pHead2);

  wReturn +=  FmtHtmlColHeadCell(pHead3);

  wReturn += "</tr>";
  return wReturn;
}

utf8String FmtHtmlRow(const char* pHead1,const char*pHead2,const char*pHead3)
{
  utf8String wReturn;
  wReturn = "<tr class=\"Gridtransparent\">";

  wReturn +=  FmtHtmlColRowCell(pHead1);

  wReturn +=  FmtHtmlColRowCell(pHead2);

  wReturn +=  FmtHtmlColRowCell(pHead3);

  wReturn += "</tr>";
  return wReturn;
}
utf8String FmtHtmlRowGrey(const char* pHead1,const char*pHead2,const char*pHead3)
{
  utf8String wReturn;
  wReturn = "<tr class=\"Gridgrey\">";
  wReturn +=  FmtHtmlColRowCell1(pHead1);

  wReturn +=  FmtHtmlColRowCell1(pHead2);

  wReturn +=  FmtHtmlColRowCell1(pHead3);

  wReturn += "</tr>";
  return wReturn;
}

utf8String
FmtHtmlMail()
{
  utf8String wMailBody ="Dear User,\n\n"
                         "You changed some key parameters in your profile.\n"
                         "As your parameter file is encrypted, this is utmost important that you keep this email in a secure place.\n"
                         "This will be the unique mode of recovery for your data in case you loose your password or your encryption Key.";

  wMailBody.replace("\n","<br/>");

  return wMailBody;

}

utf8String setupHtmlString(const utf8String& pContent)
{
  utf8String wContent=pContent;
  wContent.replace("\"","&quot");
  wContent.replace("'","&apos");
//  wContent.replace("<","&lt;");
//  wContent.replace(">","&gt;");
  wContent.replace("\n","<br/>");
  return wContent;
}


