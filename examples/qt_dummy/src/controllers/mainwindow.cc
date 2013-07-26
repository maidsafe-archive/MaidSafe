#include "src/controllers/mainwindow.h"

#include "src/qt_push_headers.h"
#include "ui_mainwindow.h"
#include "src/qt_pop_headers.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow) {
  ui->setupUi(this);

  InitialiseSignals();
}

void MainWindow::ButtonClicked() {
  ui->label->setText(tr("You trust me too easily :-["));
}

void MainWindow::InitialiseSignals() {
  connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::ButtonClicked);
}

MainWindow::~MainWindow() {
  delete ui;
}
