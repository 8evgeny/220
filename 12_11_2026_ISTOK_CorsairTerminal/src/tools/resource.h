#ifndef MVRESOURCE_H
#define MVRESOURCE_H

#include <QString>
class QVariant;

class Resource
{
public:
    Resource(const QString & section);
    ~Resource();
    bool save(const QString & fileName);
    bool load(const QString & fileName);
    QVariant get(const QString & k, bool & ok);
    void set(const QString & k, const QVariant & v);
private:
    QMap<QString, QString> * map;
    const QString section;
};

#endif // MVRESOURCE_H
