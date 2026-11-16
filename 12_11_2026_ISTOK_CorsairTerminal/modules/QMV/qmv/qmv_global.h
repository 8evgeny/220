#ifndef QMV_GLOBAL_H
#define QMV_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QMV_LIBRARY)
#  define QMVSHARED_EXPORT Q_DECL_EXPORT
#elif !SUBJECT_PROJECT
#  define QMVSHARED_EXPORT Q_DECL_IMPORT
#else
#  define QMVSHARED_EXPORT
#endif

#endif // QMV_GLOBAL_H
