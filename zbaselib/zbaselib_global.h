#ifndef ZBASELIB_GLOBAL_H
#define ZBASELIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZBASELIB_LIBRARY)
#  define ZBASELIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZBASELIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZBASELIB_GLOBAL_H
