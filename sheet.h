#ifndef SHEET_H
#define SHEET_H
#include <QVector>

class Sheet
{
public:
    Sheet();
    QVector<double> start; // 开始时间
    QVector<double> end;   // 结束时间
    QVector<int>    pitch; // 音符音高
    QVector<int>    bar;   // 所属小节
    QVector<int>    hand;  // 弹奏用手
};

#endif // SHEET_H
