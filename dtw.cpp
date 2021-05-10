#include "dtw.h"
#include <QDebug>
#include <cstdio>

DTW::DTW(Sheet &_std, Sheet &_usr)
{
    std = &_std;
    usr = &_usr;
    n = std->pitch.size();
    m = usr->pitch.size();
    bars = std->bar[n-1];
    // 用小节总数初始化各项得分的vector容量
    for(int i = 0; i < SCORE_TYPES; ++i) {
        barScore[i].resize(bars+1);
        // 左右手分数只初始化到1，便于后续push_back
        barScoreLeft[i].resize(1);
        barScoreRight[i].resize(1);
    }

    // 加载默认规则，按左右手评分
    rules.push_back(CustomedRule(QString("左手"), 1 ,bars, left_hand,  DEFAULT_WEIGHT));
    rules.push_back(CustomedRule(QString("右手"), 1, bars, right_hand, DEFAULT_WEIGHT));
    nameMap["左手"] = 0;
    nameMap["右手"] = 1;

    // 用小节总数初始化详细信息的vector容量
    barNotes.resize(bars+1);
    errNotes.resize(bars+1);

    stdLenth.resize(bars+1);
    usrLenth.resize(bars+1);

    durationScore.resize(bars+1);
    positionScore.resize(bars+1);

    barWeight.resize(bars+1);

    getDTW();
    // 先求整体的匹配情况，求出后再用于求小节的匹配情况
    getMatches();
    getMatchesByBar();
    // 计算各项得分
    calScore();
}

void DTW::getDTW()
{
    // 初始化
    for(int i = 0; i <= n; ++i) {
        dp.push_back(QVector<double>(m+1) );
        dir.push_back(QVector<int>(m+1) );
    }
    // 路径限制下的DTW算法
    dp[1][1] = getDist(1, 1);
    for(int i = 2; i <= n; ++i)
        dp[i][1] = dp[i-1][1] + getDist(i, 1), dir[i][1] = 2;
    for(int j = 2; j <= m; ++j)
        dp[1][j] = dp[1][j-1] + getDist(1, j), dir[1][j] = 3;
    for(int i = 2; i <= n; ++i) {
        for(int j = 2; j <= m; ++j) {
            double Min = qMin(dp[i-1][j-1], qMin(dp[i-1][j], dp[i][j-1]));
            double add = getDist(i, j);
            if(dp[i-1][j-1] == Min)
                dp[i][j] = add+dp[i-1][j-1], dir[i][j] = 1;
            else if(dp[i-1][j] == Min)
                dp[i][j] = add+dp[i-1][j], dir[i][j] = 2;
            else
                dp[i][j] = add+dp[i][j-1], dir[i][j] = 3;
        }
    }
}

void DTW::calScore()
{
    // 准确分
    overall[accuracy] = calAccuracy(matches);
    for(int i = 1; i <= bars; ++i) {
        barScore[accuracy][i] = calAccuracy(barMatches[i], barNotes[i], errNotes[i]);
        qDebug() << "bar:" << i << "  acc:" << barScore[accuracy][i];
    }

    // 流畅分
    double fluencySum = 0;
    for(int i = 1; i <= bars; ++i) {
        if(i < bars)
            barScore[fluency][i] = calFluency(barMatches[i], barMatches[i+1], stdLenth[i], usrLenth[i]);
        else
            barScore[fluency][i] = calFluency(barMatches[i], stdLenth[i], usrLenth[i]);
        fluencySum += barScore[fluency][i];
        qDebug() << "bar:" << i << "  flu:" << barScore[fluency][i];
    }
    overall[fluency] = fluencySum / bars;

    // 节奏分
    double rhythmSum = 0;
    for(int i = 1; i <= bars; ++i) {
        barScore[rhythm][i] = calRhythm(barMatches[i], durationScore[i], positionScore[i]);
        rhythmSum += barScore[rhythm][i];
        qDebug() << "bar:" << i << "  rhy:" << barScore[rhythm][i];
    }
    overall[rhythm] = rhythmSum / bars;

    // show the score of left and right hand
//    for(int i = 1; i <= bars; ++i) {
//        qDebug() << "bar=" << i;
//        qDebug() << "left";
//        qDebug() << " acc=" << barScoreLeft[accuracy][i]
//                 << " flu=" << barScoreLeft[fluency] [i]
//                 << " rhy=" << barScoreLeft[rhythm]  [i];
//        qDebug() << "right";
//        qDebug() << " acc=" << barScoreRight[accuracy][i]
//                 << " flu=" << barScoreRight[fluency] [i]
//                 << " rhy=" << barScoreRight[rhythm]  [i];
//    }

}


// 由dir标记的转移方向，得到每个标准音符的候选音符
void DTW::getMatches()
{
    int x = n;
    int y = m;
    while(true) {
        addMatch(x, y);
        if(dir[x][y] == 1) --x, --y;
        else if(dir[x][y] == 2) --x;
        else if(dir[x][y] == 3) --y;
        else break;
    }
}

void DTW::getMatchesByBar()
{
    barMatches.resize(bars+1);
    for(int i = 0; i < n; ++i) {
         int bar = std->bar[i];
         if(!barMatches[bar].count(i+1))
             barMatches[bar][i+1] = QVector<int>();
         barMatches[bar][i+1] = (matches[i+1]);
    }
    // debug
//    for(int i = 0; i < barMatches.size(); ++i) {
//        qDebug() << "i=" << i;
//        for(auto it = barMatches[i].begin(); it != barMatches[i].end(); ++it) {
//            qDebug() << "stdNote: " << it.key();
//            for(int j = 0; j < it.value().size(); ++j)
//                qDebug() << it.value()[j];
//        }
//        qDebug() << "";
//    }
}

void DTW::addMatch(int stdNote, int usrNote)
{
    if(!matches.count(stdNote) )
        matches[stdNote] = QVector<int>();
    matches[stdNote].push_back(usrNote);
}

double DTW::getDist(int i, int j)
{
    // 如果音高不同，则设它们的距离极大
//    if(std->pitch[i-1] != usr->pitch[j-1])
//        return 1e8;
    // 否则计算欧氏距离
    return (std->start[i-1]-usr->start[j-1])*(std->start[i-1]-usr->start[j-1]);
}

double DTW::getDist(int i, int j, double stdOffset, double usrOffset)
{
    double stdRegular = std->start[i-1] - stdOffset;
    double usrRegular = usr->start[j-1] - usrOffset;
    // 计算曼哈顿距离
    return qAbs(stdRegular-usrRegular);
}

// 计算整体，不加载详细信息
double DTW::calAccuracy(Match &matches)
{
    // 两曲谱的音符数目差值
    int bar_notes = matches.size();
    int err_notes = 0;

    for(auto it = matches.begin(); it != matches.end(); ++it) {
        int stdNote = it.key();
        bool isMatch = false;
        for(int i = 0; i < it.value().size(); ++i) {
            int usrNote = it.value()[i];
            // 除了第一个音高相同的音符，都按错误音符处理
            if(!isMatch && std->pitch[stdNote-1] == usr->pitch[usrNote-1])
                isMatch = true;
            else ++err_notes;
        }
    }
    double accuracy_score = 100.0*(qMax(0, bar_notes-err_notes)) / bar_notes;
    qDebug() << "bar_notes = " << bar_notes << "  err_notes = " << err_notes;
    return accuracy_score;
}

// 计算小节，加载详细信息，并分别计算左右手
double DTW::calAccuracy(Match &matches, int &barNotes, int &errNotes)
{
    // 两曲谱的音符数目差值
    int bar_notes = matches.size();
    int err_notes = 0;

    int bar_left = 0, bar_right = 0;
    int err_left = 0, err_right = 0;

    for(auto it = matches.begin(); it != matches.end(); ++it) {
        int stdNote = it.key();
        std->hand[stdNote-1] == 1? ++bar_left: ++bar_right;
        bool isMatch = false;
        for(int i = 0; i < it.value().size(); ++i) {
            int usrNote = it.value()[i];
            // 除了第一个音高相同的音符，都按错误音符处理
            if(!isMatch && std->pitch[stdNote-1] == usr->pitch[usrNote-1])
                isMatch = true;
            else {
                ++err_notes;
                std->hand[stdNote-1] == 1? ++err_left: ++err_right;
            }
        }
    }
    double accuracy_score = 100.0*(qMax(0, bar_notes-err_notes)) / bar_notes;
    qDebug() << "bar_notes = " << bar_notes << "  err_notes = " << err_notes;
    barNotes = bar_notes;
    errNotes = err_notes;

    barScoreLeft[accuracy].push_back(100.0*(qMax(0, bar_left-err_left)) / bar_left);
    barScoreRight[accuracy].push_back(100.0*(qMax(0, bar_right-err_right)) / bar_right);

    return accuracy_score;
}

// 每小节还需对比下一小节的第一个音符
double DTW::calFluency(Match &matches1, Match &matches2, double &stdLenth, double &usrLenth)
{
    // std序号有序，直接取两小节的起始序号
    int stdStartNote = matches1.firstKey();
    int stdEndNote = matches2.firstKey();
    stdLenth = std->start[stdEndNote-1] - std->start[stdStartNote-1];
    double usrStart = INFINITY, usrEnd = -INFINITY;
    for(auto it = matches1.begin(); it != matches1.end(); ++it) {
        int stdNote = it.key();
        for(int i = 0; i < it.value().size(); ++i) {
            // 以音符起始时刻为标准，维护该小节的最早开始时间、最晚结束时间
            int usrNote = it.value()[i];
            if(std->pitch[stdNote-1] == usr->pitch[usrNote-1]) {
                usrStart = qMin(usrStart, usr->start[usrNote-1]);
                usrEnd   = qMax(usrEnd,   usr->start[usrNote-1]);
            }

        }
    }
    // 额外处理相邻下一小节的第一个音符
    int stdNote = matches2.begin().key();
    for(int i = 0; i < matches2.begin().value().size(); ++i) {
        // 以音符起始时刻为标准，维护该小节的最早开始时间、最晚结束时间
        int usrNote = matches2.begin().value()[i];
        if(std->pitch[stdNote-1] == usr->pitch[usrNote-1]) {
            usrStart = qMin(usrStart, usr->start[usrNote-1]);
            usrEnd   = qMax(usrEnd,   usr->start[usrNote-1]);
        }
    }
    double res = 0;
    // 若仍为初始值，则没有匹配上的音符，打0分
    if(usrStart == INFINITY)
        res = 0;
    else {
        // 计算用户时长相对标准时长的差异，超过一倍时长打0分
        usrLenth = usrEnd - usrStart;
        if(usrLenth >= stdLenth) {
            double diff = usrLenth - stdLenth;
            res = qMax(0.0, 100 * (1 - diff/stdLenth));
        }
        // 时长不足，按耗时百分比计算得分
        else res = 100 * usrLenth / stdLenth;
    }
    // 流畅性上，左右手得分沿用整体分数即可
    barScoreLeft[fluency].push_back(res);
    barScoreRight[fluency].push_back(res);
    return res;
}

// 对于最后一小节，只比较本小节的音符，其余与上述一致
double DTW::calFluency(Match &matches, double &stdLenth, double &usrLenth)
{
    int stdStartNote = matches.firstKey();
    int stdEndNote = matches.lastKey();
    stdLenth = std->start[stdEndNote-1] - std->start[stdStartNote-1];
    double usrStart = INFINITY, usrEnd = -INFINITY;
    for(auto it = matches.begin(); it != matches.end(); ++it) {
        int stdNote = it.key();
        for(int i = 0; i < it.value().size(); ++i) {
            int usrNote = it.value()[i];
            if(std->pitch[stdNote-1] == usr->pitch[usrNote-1]) {
                usrStart = qMin(usrStart, usr->start[usrNote-1]);
                usrEnd   = qMax(usrEnd,   usr->start[usrNote-1]);
            }

        }
    }
    double res = 0;
    if(usrStart == INFINITY)
        res = 0;
    else {
        usrLenth = usrEnd - usrStart;
        if(usrLenth >= stdLenth) {
            double diff = usrLenth - stdLenth;
            res = qMax(0.0, 100 * (1 - diff/stdLenth));
        }
        else res = 100 * usrLenth / stdLenth;
    }
    barScoreLeft[fluency].push_back(res);
    barScoreRight[fluency].push_back(res);
    return res;
}

double DTW::calRhythm(Match &matches, QVector<double> &duration, QVector<double> &position)
{
    int barNotes = matches.size();
    int startNote = matches.firstKey();
    int endNote = matches.lastKey();
    double barLen = std->end[endNote-1] - std->start[startNote-1];

    duration.resize(barNotes+1);
    position.resize(barNotes+1);

    int bar_left = 0, bar_right = 0;

    // 将本小节第一对匹配的音符的开始时间，作为规整的偏移量
    double stdOffset = std->start[startNote-1];
    double usrOffset = usr->start[matches.begin().value()[0]-1];
    double sum = 0, sumLeft = 0, sumRight = 0;
    int num = 0;
    for(auto it = matches.begin(); it != matches.end(); ++it) {
        int stdNote = it.key();
        std->hand[stdNote-1] == 1? ++bar_left: ++bar_right;
        // 维护最佳匹配的信息
        double minDist = INFINITY;
        int bestNote = -1;
        for(int i = 0; i < it.value().size(); ++i) {
            int usrNote = it.value()[i];
            double dist = getDist(stdNote, usrNote, stdOffset, usrOffset);
            if(dist < minDist) {
                minDist = dist;
                bestNote = usrNote;
            }
        }
        double stdLen = std->end[stdNote-1] - std->start[stdNote-1];
        double usrLen = usr->end[bestNote-1] - usr->start[bestNote-1];
        // 按音符时值、起始位置1:3的加权比例计算
        // 音符时值0.5倍加权（低敏感）；起始位置2倍加权（高敏感）
        double durationScore = qMax(1-0.5*qAbs(stdLen-usrLen)/stdLen, 0.0);
        double positionScore = qMax(1-2*minDist/barLen, 0.0);
        double weightedScore = 0.25 * durationScore + 0.75 * positionScore;
        sum += weightedScore;
        std->hand[stdNote-1] == 1? sumLeft += weightedScore: sumRight += weightedScore;
        ++num;
        duration[num] = durationScore;
        position[num] = positionScore;

//        qDebug() << "num=" << num;
//        qDebug() << "stdNote=" << stdNote << " bestNote=" << bestNote;
//        qDebug() << "stdLen=" << stdLen << " usrLen=" << usrLen;
//        qDebug() << "barLen=" << barLen << " minDist=" << minDist;
//        qDebug() << "d=" <<durationScore << " p=" << positionScore << " w=" << weightedScore;
    }
//    qDebug() << "sum=" << sum << " barNotes=" << barNotes;

    barScoreLeft[rhythm].push_back(100*sumLeft/bar_left);
    barScoreRight[rhythm].push_back(100*sumRight/bar_right);

    return 100*sum/barNotes;
}

double DTW::calTechnic()
{
    for(auto rule: rules) {
        int id = nameMap[rule.ruleName];
        for(int i = 0; i < rule.timeRange.size(); ++i) {
            int startBar = rule.timeRange[i].first;
            int endBar   = rule.timeRange[i].second;
            for(int j = startBar; j <= endBar; ++j) {
                barWeight[j] += rule.weight;

            }
        }
    }
}

double DTW::getOverall(int scoreType)
{
    return overall[scoreType];
}

CustomedRule::CustomedRule(QString r, int s, int e, int h, int w)
{
    ruleName = r;
    timeRange.push_back(Range(s, e));
    handType = h;
    weight = w;
}
