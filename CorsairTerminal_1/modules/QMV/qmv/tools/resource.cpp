#include "resource.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>
#include <QVariant>
#include <QMap>

Resource::Resource(const QString & section) :
    map(new QMap<QString, QString>()),
    section(section)
{
}
Resource::~Resource()
{
    delete map;
}
bool Resource::save(const QString & fileName)
{
    QString path = fileName.section('/', 0, -2).append('/');
    QDir dir;
    if(!dir.exists(path)) dir.mkpath(path);
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) return false;
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();

        xml.writeStartElement(section);

        for(auto iterator = map->begin(); iterator != map->end(); ++iterator)
        {
            xml.writeStartElement(iterator.key());
            xml.writeCharacters(iterator.value());
            xml.writeEndElement();
        }

        xml.writeEndElement();

    xml.writeEndDocument();

    file.close();

    return true;
}
bool Resource::load(const QString & fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) return false;
    QXmlStreamReader xml(&file);
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument || token == QXmlStreamReader::EndDocument || token == QXmlStreamReader::EndElement) continue;
        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name().toString() == section) break;
        }
    }
    if(xml.atEnd() || xml.hasError()) return false;
    while(!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument || token == QXmlStreamReader::EndDocument || token == QXmlStreamReader::EndElement) continue;
        if(token == QXmlStreamReader::StartElement)
        {
            QString k = xml.name().toString();
            xml.readNext();
            QString v = xml.text().toString();
            map->insert(k, v);
        }
    }
    return true;
}
QVariant Resource::get(const QString & k, bool & ok)
{
    if(map->contains(k))
    {
        ok = true;
        return map->value(k);
    }
    else
    {
        ok = false;
        return 0;
    }
}
void Resource::set(const QString & k, const QVariant & v)
{
    map->insert(k, v.toString());
}
