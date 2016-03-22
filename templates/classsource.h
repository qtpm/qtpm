#ifndef [[.ClassNameLarge]]_H
#define [[.ClassNameLarge]]_H

#include <[[.Parent]]>

class [[.ClassName]] : public [[.Parent]] {
    Q_OBJECT
public:
    explicit [[.ClassName]]([[.Parent]] *parent = 0);
    virtual ~[[.ClassName]]() = default;
signals:
public slots:
};

#endif // [[.ClassNameLarge]]_H

