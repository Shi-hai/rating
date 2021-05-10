#include "custom.h"
#include "ui_custom.h"

Custom::Custom(DTW *dtw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::custom)
{
    this->dtw = dtw;
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

    loadRules();

}

Custom::~Custom()
{
    delete ui;
}

void Custom::loadRules()
{
    int num = 0;
    for(auto rule: dtw->rules) {
        for(int i = 0; i < rule.timeRange.size(); ++i, ++num) {
            ui->rule_list->insertRow(num);
            ui->rule_list->setItem(num, 0, new QTableWidgetItem(rule.ruleName));
            ui->rule_list->setItem(num, 1, new QTableWidgetItem(QString::number(rule.timeRange[i].first)));
            ui->rule_list->setItem(num, 2, new QTableWidgetItem(QString::number(rule.timeRange[i].second)));
            ui->rule_list->setItem(num, 3, new QTableWidgetItem(QString::number(rule.weight)));
            if(rule.handType == both_hand)
                ui->rule_list->setItem(num, 4, new QTableWidgetItem("双手"));
            else if(rule.handType == left_hand)
                ui->rule_list->setItem(num, 4, new QTableWidgetItem("左手"));
            else
                ui->rule_list->setItem(num, 4, new QTableWidgetItem("右手"));
        }
    }
}

void Custom::on_add_released()
{
    Rule rule(dtw);
    rule.exec();
}
