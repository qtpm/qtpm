#ifndef [[.TargetLarge]]_GLOBAL_H
#define [[.TargetLarge]]_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined([[.TargetLarge]]_LIBRARY)
#  define [[.TargetLarge]]SHARED_EXPORT Q_DECL_EXPORT
#else
#  define [[.TargetLarge]]SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // [[.TargetLarge]]_GLOBAL_H

