#ifndef CUSTOM_H
#define CUSTOM_H

#include <QWidget>
#include "rule.h"
#include "dtw.h"

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

private:
    Ui::custom *ui;
};

#endif // CUSTOM_H
