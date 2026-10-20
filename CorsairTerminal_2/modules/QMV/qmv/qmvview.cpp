#include "qmvview.h"
#include "qmv/qmvmodel.h"
#include "qmv/widget/inoutstringwidget.h"
#include "qmv/property/valueproperty.h"

#ifdef DEBUG
#include <QDebug>
#endif

#include <QMessageBox>

#include <QFileDialog>

#include <QKeyEvent>

QMVView::QMVView(QWidget *parent) :
    QWidget(parent),
    model(nullptr)
{
    last_path = new InoutStringWidget(this);
    last_path->setObjectName("last_path");
    last_path->set_label_text("last_path");
    last_path->set_label_width(50);
    last_path->hide();
    version = new InoutStringWidget(this);
    version->setObjectName("version");
    version->set_label_text("version");
    version->set_label_width(50);
    version->hide();
}
void QMVView::on_save_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл настроек"), last_path->get_val().toString(), tr("XML (*.xml);;All files (*.*)"));
    if(fileName != "")
    {
        model->save(fileName);
    }
}
void QMVView::on_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл настроек"), last_path->get_val().toString(), tr("XML (*.xml);;All files (*.*)"));
    if(fileName != "")
    {
        model->load(fileName);
    }
}
void QMVView::warning(const QString & t, const QString & m)
{
    QMessageBox::warning(this, t, m);
}
void QMVView::information(const QString & t, const QString & m)
{
    QMessageBox::information(this, t, m);
}
void QMVView::critical(const QString & t, const QString & m)
{
    QMessageBox::critical(this, t, m);
}
void QMVView::set_model(QMVModel * model)
{
    if(!model)
    {
#ifdef DEBUG
        qCritical() << "QMVView::set_model: model can't be NULL.";
#endif
        return;
    }
    this->model = model;
    last_path->connect_property(model->value_property(last_path->objectName()));
    version->connect_property(model->value_property(version->objectName()));
    connect(model, SIGNAL(warning(QString, QString)), SLOT(warning(QString, QString)));
    connect(model, SIGNAL(information(QString, QString)), SLOT(information(QString, QString)));
    connect(model, SIGNAL(critical(QString, QString)), SLOT(critical(QString, QString)));
    connect_model();
    model->update_view();
}
void QMVView::keyReleaseEvent(QKeyEvent * key_event)
{
    if(key_event->isAutoRepeat()) return;
    switch(key_event->key())
    {
    case Qt::Key_Escape:
        hide();
    }
}
