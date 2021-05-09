#include "custom.h"
#include "ui_custom.h"

Custom::Custom(DTW *dtw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::custom)
{
    ui->setupUi(this);
    setWindowTitle("自定义技术类别");
    // 只能选中某一行
    ui->rule_list->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->rule_list->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 禁止修改
    ui->rule_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 列宽自适应
    ui->rule_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->bars->setText(QString::number(dtw->bars));

    // 默认添加左右手打分
    ui->rule_list->insertRow(0);
    ui->rule_list->setItem(0, 0, new QTableWidgetItem("左手得分"));
    ui->rule_list->setItem(0, 1, new QTableWidgetItem("1"));
    ui->rule_list->setItem(0, 2, new QTableWidgetItem(QString::number(dtw->bars)));
    ui->rule_list->setItem(0, 3, new QTableWidgetItem("5"));
    ui->rule_list->setItem(0, 4, new QTableWidgetItem("左手"));

    ui->rule_list->insertRow(1);
    ui->rule_list->setItem(1, 0, new QTableWidgetItem("右手得分"));
    ui->rule_list->setItem(1, 1, new QTableWidgetItem("1"));
    ui->rule_list->setItem(1, 2, new QTableWidgetItem(QString::number(dtw->bars)));
    ui->rule_list->setItem(1, 3, new QTableWidgetItem("5"));
    ui->rule_list->setItem(1, 4, new QTableWidgetItem("右手"));

}

Custom::~Custom()
{
    delete ui;
}

void Custom::on_add_released()
{
    Rule rule;
    rule.exec();
}
