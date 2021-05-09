#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QVector>

#include "detail.h"
#include "custom.h"
#include "sheet.h"
#include "dtw.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadComboBox();
    QStringList loadJson(QDir dir);
    void openJson(QFile &file, Sheet &sheet, bool isStd);

private slots:
    void on_more_released();

    void on_confirm_released();

    void on_custom_released();

    void on_reset_released();

private:
    Ui::MainWindow *ui;
    Detail *detail;
    Custom *custom;
    Sheet std, usr;
    DTW *dtw;

    bool isLoad = false;
};
#endif // MAINWINDOW_H
