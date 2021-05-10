#ifndef RULE_H
#define RULE_H

#include <QDialog>
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

private slots:
    void on_add_released();

private:
    Ui::Rule *ui;
    DTW *dtw;
};

#endif // RULE_H
