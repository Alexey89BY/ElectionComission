#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_btnCopyToClipboard_clicked();

  void on_btnGenerateReport_clicked();

  void on_btnNextLot_clicked();

  void on_btnPreFormat_clicked();

private:
  Ui::MainWindow *ui;

  void preFormatSplitted();
  void preFormatJoined();
};
#endif // MAINWINDOW_H
