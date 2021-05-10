#include "rule.h"
#include "ui_rule.h"

Rule::Rule(DTW *dtw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rule)
{
    this->dtw = dtw;
    ui->setupUi(this);
    setWindowTitle("添加规则");
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

Rule::~Rule()
{
    delete ui;
}

void Rule::on_add_released()
{

}
