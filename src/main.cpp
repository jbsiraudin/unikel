#include "viewer.h"
#include <qapplication.h>
//#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QtWidgets>
#include <QToolBar>
#include <QWidget>
#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>

int main(int argc, char **argv) {
  // Read command lines arguments.
  QApplication application(argc, argv);

  uniViewer *viewer = new uniViewer;
  QMainWindow *mainWindow = new QMainWindow;
  QWidget *widget = new QWidget;
  QDockWidget *dock = new QDockWidget("Tools", mainWindow);
  dock->setWidget(widget);

  // [0] Def
  QGroupBox *kpParamGroup = new QGroupBox(QStringLiteral("Path Control"));

  std::string nbKp = std::to_string(viewer->getNbKeypoints());
  QLabel *kpParamLabel = new QLabel(QStringLiteral("Keypoints: ")+nbKp.c_str());

  QPushButton *addKeypointButton = new QPushButton;
  addKeypointButton->setText(QStringLiteral("+"));
  QObject::connect(addKeypointButton, &QPushButton::clicked, viewer, &uniViewer::addKeyPoint);

  QPushButton *removeKeypointButton = new QPushButton;
  removeKeypointButton->setText(QStringLiteral("-"));
  QObject::connect(removeKeypointButton, &QPushButton::clicked, viewer, &uniViewer::removeKeyPoint);

  QLabel *stepFactorLabel = new QLabel(QStringLiteral("Step factor: "));
  QSpinBox *stepFactorSP = new QSpinBox;
  stepFactorSP->setRange(1, 30);
  stepFactorSP->setValue(viewer->getStepFactor());
  QObject::connect(stepFactorSP, QOverload<int>::of(&QSpinBox::valueChanged), viewer, [=](int i) { viewer->setStepFactor(i); });

  QCheckBox *debugKelvinletsCheckbox = new QCheckBox;
  debugKelvinletsCheckbox->setText(QStringLiteral("DEBUG VIEW"));
  debugKelvinletsCheckbox->setChecked(viewer->getDebugView());
  QObject::connect(debugKelvinletsCheckbox, &QCheckBox::stateChanged, viewer, [=]() { viewer->setDebugView(debugKelvinletsCheckbox->checkState()); });

  QGridLayout *kpParamLayout = new QGridLayout;
  kpParamLayout->addWidget(kpParamLabel, 0, 0);
  kpParamLayout->addWidget(removeKeypointButton, 0, 1);
  kpParamLayout->addWidget(addKeypointButton, 0, 2);
  kpParamLayout->addWidget(addKeypointButton, 0, 2);
  kpParamLayout->addWidget(stepFactorLabel, 1, 0);
  kpParamLayout->addWidget(stepFactorSP, 1, 1, 1, 2);
  kpParamLayout->addWidget(debugKelvinletsCheckbox, 2, 0, 1, 3);
  kpParamGroup->setLayout(kpParamLayout);
  // ! [0]

  //connect(action, &QAction::triggered, engine, [=]() { engine->processAction(action->text()); });

  // [1] Def
  QGroupBox *kpCtrlGroup = new QGroupBox(QStringLiteral("Keypoint in focus"));
  QComboBox *kpList = new QComboBox;
  for (int i = 0; i < viewer->getNbKeypoints(); i++) {
      kpList->addItem(QStringLiteral("Point ")+std::to_string(i+1).c_str());
  }


  QLabel *posLabel = new QLabel(QStringLiteral("POSITION"));
  QDoubleSpinBox *posKeypointX = new QDoubleSpinBox;
  QDoubleSpinBox *posKeypointY = new QDoubleSpinBox;
  QDoubleSpinBox *posKeypointZ = new QDoubleSpinBox;
  posKeypointX->setRange(-1000.0, 1000.0);
  posKeypointY->setRange(-1000.0, 1000.0);
  posKeypointZ->setRange(-1000.0, 1000.0);

  QLabel *rotLabel = new QLabel(QStringLiteral("ROTATION"));
  QSlider *rotationSlider = new QSlider(Qt::Horizontal);
  rotationSlider->setTickInterval(20);
  rotationSlider->setTickPosition(QSlider::TicksAbove);
  rotationSlider->setMinimum(0);
  rotationSlider->setValue(0);
  rotationSlider->setMaximum(360);

  QLabel *scaleLabel = new QLabel(QStringLiteral("SCALE"));
  QSlider *scaleSlider = new QSlider(Qt::Horizontal);
  rotationSlider->setTickInterval(10);
  rotationSlider->setTickPosition(QSlider::TicksAbove);
  rotationSlider->setMinimum(0);
  rotationSlider->setValue(100);
  rotationSlider->setMaximum(100);

  QLabel *epsilonLabel = new QLabel(QStringLiteral("EPSILON"));
  QDoubleSpinBox *epsilonSP = new QDoubleSpinBox();
  epsilonSP->setValue(10.0);
  epsilonSP->setRange(0.0, 30.0);

  QObject::connect(kpList, QOverload<int>::of(&QComboBox::currentIndexChanged), viewer, [=](int in){  });


  QGridLayout *kpCtrlLayout = new QGridLayout;
  kpCtrlLayout->addWidget(kpList, 0, 0, 1, 7);
  kpCtrlLayout->addWidget(posLabel, 1, 0);
  kpCtrlLayout->addWidget(new QLabel(QStringLiteral("x: ")), 1, 1);
  kpCtrlLayout->addWidget(posKeypointX, 1, 2);
  kpCtrlLayout->addWidget(new QLabel(QStringLiteral("y: ")), 1, 3);
  kpCtrlLayout->addWidget(posKeypointY, 1, 4);
  kpCtrlLayout->addWidget(new QLabel(QStringLiteral("z: ")), 1, 5);
  kpCtrlLayout->addWidget(posKeypointZ, 1, 6);
  kpCtrlLayout->addWidget(rotLabel, 2, 0, 1, 1);
  kpCtrlLayout->addWidget(rotationSlider, 2, 1, 1, 6);
  kpCtrlLayout->addWidget(scaleLabel, 3, 0, 1, 1);
  kpCtrlLayout->addWidget(scaleSlider, 3, 1, 1, 6);
  kpCtrlLayout->addWidget(epsilonLabel, 4, 0, 1, 1);
  kpCtrlLayout->addWidget(epsilonSP, 4, 1, 1, 6);
  kpCtrlGroup->setLayout(kpCtrlLayout);
  // ! [1]

  // [2]
  QGroupBox *indexGroup = new QGroupBox;

  QPushButton *openMeshBtn = new QPushButton;
  openMeshBtn->setText(QStringLiteral("Open mesh"));
  QObject::connect(openMeshBtn, &QPushButton::clicked, viewer, &uniViewer::openMesh);

  QPushButton *saveMeshBtn = new QPushButton;
  saveMeshBtn->setText(QStringLiteral("Save mesh"));
  QObject::connect(saveMeshBtn, &QPushButton::clicked, viewer, &uniViewer::saveMesh);

  QPushButton *snapshotBtn = new QPushButton;
  snapshotBtn->setText(QStringLiteral("Snapshot"));
  QObject::connect(snapshotBtn, &QPushButton::clicked, viewer, &uniViewer::saveSnapShot);

  QGridLayout *indexLayout = new QGridLayout;
  indexLayout->addWidget(openMeshBtn, 0, 0);
  indexLayout->addWidget(saveMeshBtn, 1, 0);
  indexLayout->addWidget(snapshotBtn, 2, 0);
  indexGroup->setLayout(indexLayout);
  // ! [2]

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(indexGroup, 0, 0);
  layout->addWidget(kpParamGroup, 1, 0);
  layout->addWidget(kpCtrlGroup, 2, 0);
  widget->setLayout(layout);

  mainWindow->setCentralWidget(viewer);
  mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
  mainWindow->setWindowTitle("Moving Kelvinlets Experiment");
//  mainWindow->setMinimumWidth(1200);
//  mainWindow->setMinimumHeight(900);
  mainWindow->setWindowIcon(QIcon("/home/siraudin/Dev/railedkelvinlets/Unikel/icons/icon.png"));
  mainWindow->show();

  return application.exec();
}
