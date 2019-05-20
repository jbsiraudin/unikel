#include "viewer.h"
#include <qapplication.h>
#include <QMainWindow>
#include <QToolBar>

int main(int argc, char **argv) {
  // Read command lines arguments.
  QApplication application(argc, argv);

  uniViewer *otherViewer = new uniViewer;
  QMainWindow *mainWindow = new QMainWindow;
  QToolBar *toolBar = new QToolBar;
  toolBar->setIconSize(QSize(35,35));
  toolBar->setAutoFillBackground(true);
  toolBar->setStyleSheet("QToolBar { background: white; }");
  otherViewer->addActionsToToolbar(toolBar);
  mainWindow->addToolBar(toolBar);
  mainWindow->setCentralWidget(otherViewer);

  mainWindow->setWindowIcon(QIcon("img/icons/icon.png"));
  mainWindow->show();

//  // Instantiate the viewer.
//  Viewer viewer;

//  viewer.setWindowTitle("simpleViewer");

//  // Make the viewer window visible on screen.
//  viewer.show();

  // Run main loop.
  return application.exec();
}
