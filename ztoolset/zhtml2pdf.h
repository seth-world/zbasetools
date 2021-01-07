#ifndef ZHTML2PDF_H
#define ZHTML2PDF_H

#include <QObject>
#include <QtWebEngineWidgets/QWebEnginePage>

#include <ztoolset/zstatus.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/uristring.h>

class ZHtml2Pdf : public QWebEnginePage
{
  Q_OBJECT
public:
  explicit ZHtml2Pdf(QObject *parent = nullptr);


  void setLocalFile(bool pOnOff=true) {isLocalFile=pOnOff;}
  void setQuiet(bool pOnOff=true) {isQuiet=pOnOff;}

  void setup (const uriString &phtmlInput, const uriString &pPdfOutput);

  void run();

  ZStatus loadLocal(const uriString &pPath, utf8String& pContent);

signals:

public slots:
  void pdfPrinted(const QString &pFilePath, bool pSuccess);
  void htmlLoadFinished( bool pSuccess);
  void htmlLoadStarted( );

private:
  int pdfProgress=0;
  bool isLocalFile=false;
  bool isQuiet=false;
  uriString Input;
  uriString Output;
};

#endif // ZHTML2PDF_H
