#ifndef [[.TargetLarge]]_H
#define [[.TargetLarge]]_H

[[if .Library]]#include "[[.TargetSmall]]_global.h"
[[end]]#include <[[.Parent]]>

class [[if .Library]][[.TargetLarge]]SHARED_EXPORT [[end]][[.Target]] : public [[.Parent]] {
    Q_OBJECT
public:
    explicit [[.Target]]([[.Parent]] *parent = 0);
signals:
public slots:
};

#endif // [[.TargetLarge]]_H

