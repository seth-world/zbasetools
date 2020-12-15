

#include <zconfig.h>
#include <ztoolset/zutfstrings.h>

#include <QCoreApplication>



int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);


  QByteArray wQB;

  wQB = "thosdfls";

  QList<std::string> wQL;

  wQL.push_back("toto");

  utf8String wS = "titi";

  std::list<std::string> wSList;

  wSList.push_back("tutu");
  std::string wStdStr = "gabu";

  zbs::ZArray<utf8String> wZA;
  wZA.push("taataa");
  wZA.push("tbbtbb");
  wZA.push("txxtxx");

  zbs::ZArray<utf8String*> wZB;
  wZB.push(new utf8String("tyytyy"));


  return a.exec();
}
