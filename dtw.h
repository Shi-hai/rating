#ifndef DTW_H
#define DTW_H

#include <QVector>
#include <QMap>

#include "sheet.h"

const int SCORE_TYPES = 5;
// 每个小节的所有音符，每个音符：std note_id -> 候选usr note_id
typedef QMap<int, QVector<int>> Match;

enum {
    total,    // 总分：由以下四项加权得到
    accuracy, // 准确分：涉及音高与开始时间
    fluency,  // 流畅分：判断是否有明显停顿
    rhythm,   // 节奏分：规整到具体小节，涉及音高与时值
    technic   // 技术分：对技术点分类后，按类型处理
};

class DTW
{
public:
    DTW(Sheet &std, Sheet &usr);

    void getDTW();
    void getDTWByBar();
    void calScore();

    double getDist(int i, int j);
    double getDist(int i, int j, double stdOffset, double usrOffset);

    void getMatches();
    void getMatchesByBar();

    void addMatch(int stdNote, int usrNote);
    void addMatch(int barNo, int stdNote, int usrNote);

    double calTotal();
    double calAccuracy(Match &matches);
    double calAccuracy(Match &matches, int &barNotes, int &errNotes);
    double calFluency(Match &matches1, Match &matches2, double &stdLenth, double &usrLenth);
    double calFluency(Match &matches, double &stdLenth, double &usrLenth);
    double calRhythm(Match &matches, QVector<double> &duration, QVector<double> &position);
    double calTechnic(Match &matches);

    double getOverall(int scoreType);

    // 以下数据设为公有变量，便于其他类获取

    int bars; // 小节总数
    // 按小节分别打分
    QVector<double> barScore[SCORE_TYPES];
    // 按全曲整体打分
    double overall[SCORE_TYPES];

    // 准确分信息
    QVector<int> barNotes;
    QVector<int> errNotes;

    // 流畅分信息
    QVector<double> stdLenth;
    QVector<double> usrLenth;

    // 节奏分信息
    QVector<QVector<double>> durationScore;
    QVector<QVector<double>> positionScore;

    // TODO: 技术分信息

private:
    // 指向已读取的曲谱
    Sheet *std;
    Sheet *usr;

    // 用于DTW算法
    QVector<QVector<double> > dp;  // 距离信息
    QVector<QVector<int> >    dir; // 方向信息

    Match matches;             // 整体匹配情况
    QVector<Match> barMatches; // 各小节匹配情况

    int n, m; // std与usr的音符数目


};

#endif // DTW_H
