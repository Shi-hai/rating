#ifndef DETAIL_H
#define DETAIL_H

#include <QWidget>
#include <QDebug>
#include "dtw.h"

namespace Ui {
class Detail;
}

class Detail : public QWidget
{
    Q_OBJECT

public:
    explicit Detail(DTW *dtw, QWidget *parent = nullptr);
    ~Detail();
    DTW *dtw;

private:
    Ui::Detail *ui;
};

#endif // DETAIL_H
