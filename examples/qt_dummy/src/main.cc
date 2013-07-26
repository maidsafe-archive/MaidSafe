#include "src/qt_push_headers.h"
#include "src/qt_pop_headers.h"

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"

#include "src/controllers/mainwindow.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  // Set Application Plugin Directory
  app.addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

  // Load Default Style's
  QFile default_style(":/styles/common.qss");
  default_style.open(QFile::ReadOnly);
  QString style_sheet(QLatin1String(default_style.readAll()));
  app.setStyleSheet(style_sheet);

  // Create MainWindow
  MainWindow mainWindow;
  mainWindow.show();

  // Start Event loop
  return app.exec();
}