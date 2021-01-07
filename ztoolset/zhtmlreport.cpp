#include "zhtmlreport.h"

const char* HtmlHeader="<!DOCTYPE html>\n"
                         "<html lang=\"en\">\n"
                         "<head> \n";

const char* HtmlStyle=
    "<style>\n"
    "table { border-collapse:collapse; }\n"
    "table thead th { border-top: 1px solid #000;border-bottom: 1px solid #000;background-color:%s;text-align:left; }\n"
    ".Gridtransparent { background-color:transparent;width:2.2313in; font-family:%s;}\n"
    ".Gridgrey { background-color:LightYellow;width:2.2313in; font-family:%s;}\n"
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
  Cells.push(new ZHtmlCell(pText,pStyle));

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


utf8String ZHtmlTableCommonRow::get(int pColumnCount,bool pGrey)
{
  //    if (Cells.count()==0)
  //      return utf8String("");
  utf8String wReturn;
  if (pGrey)
    wReturn = "<tr class=\"Gridgrey\">";
  else
    wReturn = "<tr class=\"Gridtransparent\">";
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
    wReturn += Rows[wi]->get(ColumnCount,wGrey);
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

void ZHtmlReport::addTitle(const utf8String& pTitle)
{
  if (Title)
    delete Title;
  Title=new utf8String(pTitle);
}
void ZHtmlReport::addParagraphBefore(const utf8String& pParagraph)
{
  if (ParBefore)
    delete ParBefore;
  ParBefore=new utf8String(pParagraph);
}
void ZHtmlReport::addParagraphAfter(const utf8String& pParagraph)
{
  if (ParAfter)
    delete ParAfter;
  ParAfter=new utf8String(pParagraph);
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
  if (Table)
    {
    if (Table->HeaderBackColor)
      wHeaderBackColor=*Table->HeaderBackColor;
    }
  if (Font)
    wFont=*Font;

  wReturn.addsprintf(HtmlStyle,wHeaderBackColor.toCChar(),wFont.toCChar(),wFont.toCChar());

  if (Title)
    {
    wReturn.addsprintf("<title>\n%s</title>",setupHtmlString(*Title).toCChar());
    }

  wReturn+="</head>\n";

  if (Font)
    wReturn.addsprintf(HtmlBodyHeader,Font->toCChar());
  else
    wReturn += "<!--Message body-->\n<body>\n";

  if (ParBefore)
  {
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*ParBefore).toCChar());
  }

  if (Table)
  {
    wReturn += Table->get();
  }
  if (ParAfter)
  {
    wReturn.addsprintf("<p>%s</p>\n",setupHtmlString(*ParAfter).toCChar());
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

utf8String setupHtmlString(utf8String& wContent)
{
  wContent.replace("\"","&quot");
  wContent.replace("'","&apos");
//  wContent.replace("<","&lt;");
//  wContent.replace(">","&gt;");
  wContent.replace("\n","<br/>");
  return wContent;
}


