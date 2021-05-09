#include "detail.h"
#include "ui_detail.h"

Detail::Detail(DTW *dtw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Detail)
{
    this->dtw = dtw;
    ui->setupUi(this);
    setWindowTitle("各小节得分详情");

    // 加载表头
    ui->bar_list->setColumnCount(6);
    QStringList header;
    header << "小节序号" << "小节总分" << "准确分" << "流畅分" << "节奏分" << "技术分";
    ui->bar_list->setHorizontalHeaderLabels(header);
    // 隐藏行序号
    QHeaderView* headerView = ui->bar_list->verticalHeader();
    headerView->setHidden(true);

    // 只能选中某一行
    ui->bar_list->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->bar_list->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 禁止修改
    ui->bar_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 列宽自适应
    ui->bar_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "dtw.bars = " << dtw->bars;
    for(int i = 1; i <= dtw->bars; ++i) {
        int row = ui->bar_list->rowCount();
        // 加载各小节得分
        ui->bar_list->insertRow(row);
        ui->bar_list->setItem(row, 0, new QTableWidgetItem(QString::number(i)));
        for(int j = 0; j < SCORE_TYPES; ++j) {
            qDebug() << "i=" << i << "  j=" << j;
            ui->bar_list->setItem(row, j+1, new QTableWidgetItem(QString::number(qRound(dtw->barScore[j][i]))));
        }
    }

}

Detail::~Detail()
{
    delete ui;
}
