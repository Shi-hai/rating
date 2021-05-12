#ifndef RULE_H
#define RULE_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include "dtw.h"

namespace Ui {
class Rule;
}

class Rule : public QDialog
{
    Q_OBJECT

public:
    explicit Rule(DTW *dtw, QWidget *parent = nullptr);
    ~Rule();
    int getIndex(QString ruleName);

private slots:
    void on_add_released();

signals:
    void load_rules_signal();

private:
    Ui::Rule *ui;
    DTW *dtw;
};

#endif // RULE_H
