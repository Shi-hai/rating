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

    void loadRules();

private slots:
    void on_add_released();

private:
    Ui::custom *ui;
    DTW *dtw;
};

#endif // CUSTOM_H
