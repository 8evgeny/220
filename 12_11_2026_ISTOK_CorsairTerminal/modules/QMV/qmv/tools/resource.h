#ifndef SUBJECT_PROJECT

#ifndef MVRESOURCE_H
#define MVRESOURCE_H

#include "qmv/qmv_global.h"
#include <QString>
class QVariant;

class QMVSHARED_EXPORT Resource
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

#endif
