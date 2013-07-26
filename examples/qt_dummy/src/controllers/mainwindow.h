#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/qt_push_headers.h"
#include "src/qt_pop_headers.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

 private:
  void ButtonClicked();
  void InitialiseSignals();

  Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
