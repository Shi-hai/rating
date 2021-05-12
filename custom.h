#ifndef CUSTOM_H
#define CUSTOM_H

#include <QWidget>
#include "dtw.h"
#include "rule.h"

namespace Ui {
class custom;
}

class Custom : public QWidget
{
    Q_OBJECT

public:
    explicit Custom(DTW *dtw, QWidget *parent = nullptr);
    ~Custom();



private slots:
    void on_add_released();
    void loadRules();

    void on_del_released();

signals:
    void load_rules_signal();

private:
    Ui::custom *ui;
    DTW *dtw;
    Rule *rule;
};

#endif // CUSTOM_H
