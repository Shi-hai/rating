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

    // 可选小节不能超过小节总数
    ui->start->setMaximum(dtw->bars);
    ui->end->setMaximum(dtw->bars);
}

Rule::~Rule()
{
    delete ui;
}

int Rule::getIndex(QString ruleName)
{
    for(int i = 0; i < dtw->rules.size(); ++i)
        if(dtw->rules[i].ruleName == ruleName)
            return i;
    return -1;
}

void Rule::on_add_released()
{
    QString ruleName = ui->name->text();
    int startBar = ui->start->text().toInt();
    int endBar   = ui->end->text().toInt();
    int weight   = ui->weight->text().toInt();
    int handType = ui->both->isChecked()? both_hand:
                   ui->left->isChecked()? left_hand: right_hand;

    if(ruleName == "") {
        QMessageBox::warning(this, "信息有误", "规则名称不能为空", "确定");
        return;
    }
    if(startBar > endBar) {
        QMessageBox::warning(this, "信息有误", "开始小节不能大于结束小节", "确定");
        return;
    }
    int index = getIndex(ruleName);
    qDebug() << "index=" << index;
    // 若规则不存在，直接添加即可
    if(index == -1) {
        dtw->rules.push_back(CustomedRule(ruleName, startBar, endBar, handType, weight));
    }
    else {
        // 同名规则，需保证除始末小节外的参数一致
        if(dtw->rules[index].weight != weight || dtw->rules[index].handType != handType) {
            QMessageBox::warning(this, "信息有误", "参数与原规则不一致", "确定");
            return;
        }
        dtw->rules[index].timeRange.push_back({startBar, endBar});

        // 插入后，合并小节区间，使其有序且不相交
        QVector<Range> &intervals = dtw->rules[index].timeRange;
        QVector<Range> res;
        std::sort(intervals.begin(), intervals.end());
        int s = intervals[0].first, t = intervals[0].second;
        for(int i = 1; i < intervals.size(); ++i)
            if(intervals[i].first <= t) t = qMax(t, intervals[i].second), s = qMin(s, intervals[i].first);
            else res.push_back({s, t}), s = intervals[i].first, t = intervals[i].second;
        res.push_back({s, t});
        intervals = res;

    }
    emit load_rules_signal();
    this->close();
}
