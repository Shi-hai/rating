#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("钢琴评分系统");
    QStringList usrList = loadJson(QDir("C:/Users/msi-/Desktop/piano/rating/usr"));
    QStringList stdList = loadJson(QDir("C:/Users/msi-/Desktop/piano/rating/std"));
    int pathLen = QString("C:/Users/msi-/Desktop/piano/rating/usr/").size();
    foreach(QString file, usrList)
        ui->usr->addItem(file.mid(pathLen));
    foreach(QString file, stdList)
        ui->std->addItem(file.mid(pathLen));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadComboBox()
{
    QString stdFileName = "C:/Users/msi-/Desktop/piano/rating/std/"+ui->std->currentText();
    QString usrFileName = "C:/Users/msi-/Desktop/piano/rating/usr/"+ui->usr->currentText();
    QFile stdFile(stdFileName);
    QFile usrFile(usrFileName);
    openJson(stdFile, std, true);
    openJson(usrFile, usr, false);
    dtw = new DTW(std, usr);
}

// 加载Json文件名到待选列表
QStringList MainWindow::loadJson(QDir dir)
{
    QStringList fileList;
    if(!dir.exists()) {
        qDebug() << "not exist";
        return fileList;
    }
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    qDebug() << "filelist.size = " << list.size();

    for(int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.suffix() == "json") {
            QString absolute_file_path = fileInfo.absoluteFilePath();
            fileList.append(absolute_file_path);
        }
   }
    return fileList;
}

// 打开选定的Json文件，并保存至Sheet中
void MainWindow::openJson(QFile &file, Sheet &sheet, bool isStd)
{
    if(!file.open(QFile::ReadWrite)) {
        qDebug() << "open error";
        return;
    }

    QByteArray byteJson = file.readAll();
    QString strJson = QString(byteJson);
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteJson, &err);
    QJsonObject json = doc.object();

    if(err.error != QJsonParseError::NoError) {
        qDebug() << "format error";
        return;
    }

    QJsonArray vals = json.value("notes").toArray();
    for(int i = 0; i < vals.size(); ++i) {
        QJsonObject obj = vals.at(i).toObject();
        // 保留置信度高于50%的音符
        if(obj.value("confidence").toDouble() > 0.5) {
            sheet.start.push_back(obj.value("start_time").toDouble());
            sheet.end.push_back(obj.value("end_time").toDouble());
            sheet.pitch.push_back(obj.value("pitch").toInt());
            // 若为标准谱，还需载入小节、左右手信息
            if(isStd) {
                sheet.bar.push_back(obj.value("bar_no").toInt());
                sheet.hand.push_back(obj.value("hand").toInt());
            }
        }
    }
//    for(int i = 0; i < sheet.start.size(); ++i) {
//        qDebug() << sheet.start[i] << ' ' << sheet.end[i]<< ' ' << sheet.pitch[i];
//    }
}


void MainWindow::on_more_released()
{
    detail = new Detail(dtw);
    detail->setWindowModality(Qt::ApplicationModal);
    detail->show();
}

void MainWindow::on_confirm_released()
{
    if(!isLoad) {
        loadComboBox();
        isLoad = true;
    }
    ui->accuracy->setText(QString::number(qRound(dtw->getOverall(accuracy))));
    ui->fluency->setText(QString::number(qRound(dtw->getOverall(fluency))));
    ui->rhythm->setText(QString::number(qRound(dtw->getOverall(rhythm))));
    setMinimumHeight(670);
    setMaximumHeight(670);
}

void MainWindow::on_custom_released()
{
    if(!isLoad) {
        loadComboBox();
        isLoad = true;
    }
    custom = new Custom(dtw);
    custom->setWindowModality(Qt::ApplicationModal);
    custom->show();
}

void MainWindow::on_reset_released()
{
    delete dtw;
    isLoad = false;
    setMinimumHeight(280);
    setMaximumHeight(280);
}
