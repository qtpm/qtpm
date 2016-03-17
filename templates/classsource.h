#ifndef [[.ClassNameLarge]]_H
#define [[.ClassNameLarge]]_H

[[if .DoExport]]#include "[[.TargetSmall]]_global.h"
[[end]]#include <[[.Parent]]>

class [[if .DoExport]][[.TargetLarge]]SHARED_EXPORT [[end]][[.ClassName]] : public [[.Parent]] {
    Q_OBJECT
public:
    explicit [[.ClassName]]([[.Parent]] *parent = 0);
    virtual ~[[.ClassName]]() = default;
signals:
public slots:
};

#endif // [[.ClassNameLarge]]_H

