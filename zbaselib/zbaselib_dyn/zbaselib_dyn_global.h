#ifndef ZBASELIB_DYN_GLOBAL_H
#define ZBASELIB_DYN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZBASELIB_DYN_LIBRARY)
#  define ZBASELIB_DYNSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ZBASELIB_DYNSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ZBASELIB_DYN_GLOBAL_H
