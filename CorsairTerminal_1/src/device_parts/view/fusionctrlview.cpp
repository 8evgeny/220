#include "fusionctrlview.h"
#include "ui_fusionctrlview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

FusionCtrlView::FusionCtrlView(QWidget *parent) :
    QMVView(parent),
    ui(new Ui::FusionCtrlView)
{
    ui->setupUi(this);

    ui->enable->set_label_text("Комплексировать");
    ui->enable->set_label_width(100);
    ui->enable->hide_get_btn();

    ui->method->set_label_text("Метод");
    ui->method->set_label_width(100);
    ui->method->hide_get_btn();

    ui->offset_x->set_label_text("Смещение X");
    ui->offset_x->set_label_width(100);
    ui->offset_x->set_slider(-50, 50, 1, 10);
    ui->offset_x->set_val(0);
    ui->offset_x->hide_get_btn();

    ui->offset_y->set_label_text("Смещение Y");
    ui->offset_y->set_label_width(100);
    ui->offset_y->set_slider(-50, 50, 1, 10);
    ui->offset_y->set_val(0);
    ui->offset_y->hide_get_btn();

    ui->method_agc_en->set_label_text("Использовать АРУ");
    ui->method_agc_en->set_label_width(100);
    ui->method_agc_en->hide_get_btn();

    ui->method_c->set_label_text("Коэффициент");
    ui->method_c->set_label_width(100);
    ui->method_c->hide_get_btn();
}
void FusionCtrlView::connect_model()
{
    ui->enable->connect_property(model->value_property("enable"));
    ui->method->connect_property(model->value_property("method"));
    ui->offset_x->connect_property(model->value_property("offset_x"));
    ui->offset_y->connect_property(model->value_property("offset_y"));
    ui->method_agc_en->connect_property(model->value_property("method_agc_en"));
    ui->method_c->connect_property(model->value_property("method_c"));
}

FusionCtrlView::~FusionCtrlView()
{
    delete ui;
}

