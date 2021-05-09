#include "rule.h"
#include "ui_rule.h"

Rule::Rule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rule)
{
    ui->setupUi(this);
    setWindowTitle("添加规则");
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

Rule::~Rule()
{
    delete ui;
}
