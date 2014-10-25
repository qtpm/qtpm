#ifndef RESULTHASH_H
#define RESULTHASH_H

#include <QMap>
#include <QReadWriteLock>

template<typename Key, typename Value>
class ResultHash
{
public:
    explicit ResultHash() {}

    const QMap<Key, Value>& results() const {
        return this->_map;
    }

    void insertError(const QString& error) {
        QWriteLocker locker(&this->_lock);
        this->_errors.append(error);
    }

    void insert(const Key &key, const Value &value) {
        QWriteLocker locker(&this->_lock);
        this->_map.insert(key, value);
    }

protected:
    mutable QReadWriteLock _lock;
    QMap<Key, Value> _map;
    QList<QString> _errors;
};

#endif // RESULTHASH_H
