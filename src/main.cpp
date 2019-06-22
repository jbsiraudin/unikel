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
  QToolBar *timeline = new QToolBar(mainWindow);
  QDockWidget *dock = new QDockWidget("Tools", mainWindow);
  dock->setWidget(widget);

  // [0]
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
  indexLayout->addWidget(saveMeshBtn, 0, 1);
  indexLayout->addWidget(snapshotBtn, 0, 2);
  indexGroup->setLayout(indexLayout);
  // ! [0]

  // [1] Def
  QGroupBox *kpParamGroup = new QGroupBox(QStringLiteral("Path Control"));

  std::string nbKp = std::to_string(viewer->getNbKeypoints());
  QLabel *kpParamLabel = new QLabel(QStringLiteral("Keypoints: ")+nbKp.c_str());

  QPushButton *addKeypointButton = new QPushButton;
  addKeypointButton->setText(QStringLiteral("+"));
  QObject::connect(addKeypointButton, &QPushButton::clicked, viewer, &uniViewer::addKeyPoint);
  QObject::connect(addKeypointButton, &QPushButton::clicked,
                   [=]() { kpParamLabel->setText(QStringLiteral("Keypoints: ")+std::to_string(viewer->getNbKeypoints()).c_str()); });

  QPushButton *removeKeypointButton = new QPushButton;
  removeKeypointButton->setText(QStringLiteral("-"));
  QObject::connect(removeKeypointButton, &QPushButton::clicked, viewer, &uniViewer::removeKeyPoint);

  QLabel *stepFactorLabel = new QLabel(QStringLiteral("Step factor: "));
  QSpinBox *stepFactorSP = new QSpinBox;
  stepFactorSP->setRange(1, 30);
  stepFactorSP->setValue(viewer->getStepFactor());
  QObject::connect(stepFactorSP, QOverload<int>::of(&QSpinBox::valueChanged), viewer, [=](int i) { viewer->setStepFactor(i); });

  QGridLayout *kpParamLayout = new QGridLayout;
  kpParamLayout->addWidget(kpParamLabel, 0, 0);
  kpParamLayout->addWidget(removeKeypointButton, 0, 1);
  kpParamLayout->addWidget(addKeypointButton, 0, 2);
  kpParamLayout->addWidget(addKeypointButton, 0, 2);
  kpParamLayout->addWidget(stepFactorLabel, 1, 0);
  kpParamLayout->addWidget(stepFactorSP, 1, 1, 1, 2);
  kpParamGroup->setLayout(kpParamLayout);
  // ! [1]

  // [2] Def
  QGroupBox *kpCtrlGroup = new QGroupBox(QStringLiteral("Keypoint in focus"));
  QComboBox *kpList = new QComboBox;
  for (int i = 0; i < viewer->getNbKeypoints(); i++) {
      kpList->addItem(QStringLiteral("Point ")+std::to_string(i+1).c_str());
  }
  QObject::connect(viewer, &uniViewer::updatedKpInFocus, kpList, &QComboBox::setCurrentIndex);
  QObject::connect(viewer, &uniViewer::rmvedKeyPoint, kpList, &QComboBox::removeItem);
  QObject::connect(viewer, &uniViewer::addedKeyPoint, [=](int idx) {kpList->addItem(QStringLiteral("Point ")+std::to_string(idx).c_str());});

  QLabel *posLabel = new QLabel(QStringLiteral("POSITION"));
  QDoubleSpinBox *posKeypointX = new QDoubleSpinBox;
  QDoubleSpinBox *posKeypointY = new QDoubleSpinBox;
  QDoubleSpinBox *posKeypointZ = new QDoubleSpinBox;
  posKeypointX->setRange(-1000.0, 1000.0);
  posKeypointX->setValue(-1.0);
  posKeypointY->setRange(-1000.0, 1000.0);
  posKeypointY->setValue(0.0);
  posKeypointZ->setRange(-1000.0, 1000.0);
  posKeypointZ->setValue(0.0);
  QObject::connect(posKeypointX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                   [=](double in) { viewer->setKpPosX(kpList->currentIndex(), in); });
  QObject::connect(viewer, &uniViewer::updatedKpPosX, posKeypointX, &QDoubleSpinBox::setValue);
  QObject::connect(posKeypointY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                   [=](double in) { viewer->setKpPosY(kpList->currentIndex(), in); });
  QObject::connect(viewer, &uniViewer::updatedKpPosY, posKeypointY, &QDoubleSpinBox::setValue);
  QObject::connect(posKeypointZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                   [=](double in) { viewer->setKpPosZ(kpList->currentIndex(), in); });
  QObject::connect(viewer, &uniViewer::updatedKpPosZ, posKeypointZ, &QDoubleSpinBox::setValue);

  QLabel *rotLabel = new QLabel(QStringLiteral("TWIST"));
  QSlider *rotationSlider = new QSlider(Qt::Horizontal);
  rotationSlider->setTickInterval(20);
  rotationSlider->setTickPosition(QSlider::TicksAbove);
  rotationSlider->setRange(0, 360);
  rotationSlider->setValue(0);
  QObject::connect(rotationSlider, &QSlider::valueChanged, [=](int in) {viewer->setKpTwist(kpList->currentIndex(), in); });

  QLabel *scaleLabel = new QLabel(QStringLiteral("SCALE"));
  QSlider *scaleSlider = new QSlider(Qt::Horizontal);
  scaleSlider->setTickInterval(10);
  scaleSlider->setTickPosition(QSlider::TicksAbove);
  scaleSlider->setRange(0, 100);
  scaleSlider->setValue(100);
  QObject::connect(scaleSlider, &QSlider::valueChanged, [=](int in) {viewer->setKpScale(kpList->currentIndex(), in/100.0); });

  QLabel *epsilonLabel = new QLabel(QStringLiteral("EPSILON"));
  QDoubleSpinBox *epsilonSP = new QDoubleSpinBox;
  epsilonSP->setValue(0.1);
  epsilonSP->setRange(0.0, 30.0);
  QObject::connect(epsilonSP, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                   [=](double in) {viewer->setKpEps(kpList->currentIndex(), in); });
  QObject::connect(viewer, &uniViewer::updatedKpEpsilon, epsilonSP, &QDoubleSpinBox::setValue);

  QObject::connect(kpList, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [=](int in){
                        keyPoint kp = viewer->getKeyPoint(in);
                        posKeypointX->setValue(kp.position.x);
                        posKeypointY->setValue(kp.position.y);
                        posKeypointZ->setValue(kp.position.z);
                        rotationSlider->setValue(kp.twist);
                        scaleSlider->setValue(100*kp.scale);
                        epsilonSP->setValue(kp.epsilon);
  });


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
  // ! [2]

  // [3]
  QGroupBox *meshGroup = new QGroupBox(QStringLiteral("Mesh options"));
  QLabel *meshLabel = new QLabel(QStringLiteral("Mesh vertices : 0"));
  QObject::connect(viewer, &uniViewer::meshOpen, meshLabel, &QLabel::setText);

  QLabel *shearLabel = new QLabel(QStringLiteral("Shear modulus"));
  QLabel *poissonLabel = new QLabel(QStringLiteral("Poisson modulus"));
  QDoubleSpinBox *shearSP = new QDoubleSpinBox;
  QDoubleSpinBox *poissonSP = new QDoubleSpinBox;
  shearSP->setValue(1.0);
  poissonSP->setValue(0.5);
  QObject::connect(viewer, &uniViewer::updatedShear, shearSP, &QDoubleSpinBox::setValue);
  QObject::connect(viewer, &uniViewer::updatedPoisson, poissonSP, &QDoubleSpinBox::setValue);
  QObject::connect(shearSP, QOverload<double>::of(&QDoubleSpinBox::valueChanged), viewer, &uniViewer::setShearModulus);
  QObject::connect(poissonSP, QOverload<double>::of(&QDoubleSpinBox::valueChanged), viewer, &uniViewer::setPoissonModulus);

  QGridLayout *meshLayout = new QGridLayout;
  meshLayout->addWidget(meshLabel, 0, 0, 1, 5);
  meshLayout->addWidget(shearLabel, 1, 0, 1, 1);
  meshLayout->addWidget(shearSP, 1, 1, 1, 4);
  meshLayout->addWidget(poissonLabel, 2, 0, 1, 1);
  meshLayout->addWidget(poissonSP, 2, 1, 1, 4);

  meshGroup->setLayout(meshLayout);
  // ![3]

  // [4]
  QGroupBox *debugGroup = new QGroupBox(QStringLiteral("Debug view options"));

  QCheckBox *debugKelvinletsCheckbox = new QCheckBox;
  debugKelvinletsCheckbox->setText(QStringLiteral("Activate debug spheres"));
  debugKelvinletsCheckbox->setChecked(viewer->getDebugView());
  QObject::connect(debugKelvinletsCheckbox, &QCheckBox::stateChanged, viewer, [=]() { viewer->setDebugView(debugKelvinletsCheckbox->checkState()); });

  QLabel *progressLabel = new QLabel(QStringLiteral(""));
  QProgressBar *progressBar = new QProgressBar;
  progressBar->setTextVisible(true);
  progressBar->setFixedHeight(10);

  QGridLayout *debugLayout = new QGridLayout;
  debugLayout->addWidget(debugKelvinletsCheckbox, 0, 0);
  debugLayout->addWidget(progressLabel, 1, 0);
  debugLayout->addWidget(progressBar, 2, 0);

  debugGroup->setLayout(debugLayout);
  // ![4]

  // [5]
  QPushButton *previousFrame = new QPushButton(QStringLiteral("«"));
  QPushButton *playFrame = new QPushButton(QStringLiteral(">"));
  QPushButton *nextFrame = new QPushButton(QStringLiteral("»"));
  previousFrame->setFixedWidth(20);
  playFrame->setFixedWidth(20);
  nextFrame->setFixedWidth(20);
  QObject::connect(playFrame, &QPushButton::clicked,
                   [=]() {if(!viewer->animationIsStarted()) {
                                viewer->startAnimation();
                                playFrame->setText(QStringLiteral("II"));
                            } else {
                                  viewer->stopAnimation();
                                  playFrame->setText(QStringLiteral(">"));
                            }
                    });

  QSlider *timelineSlider = new QSlider(Qt::Horizontal);
  timelineSlider->setRange(0, viewer->getTimeMax());
  timelineSlider->setTickInterval(1);
  QObject::connect(viewer, &uniViewer::timed, timelineSlider, &QSlider::setValue);

  timeline->addWidget(previousFrame);
  timeline->addWidget(playFrame);
  timeline->addWidget(nextFrame);
  timeline->addWidget(timelineSlider);
  // ![5]

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(indexGroup, 0, 0);
  layout->addWidget(kpParamGroup, 1, 0);
  layout->addWidget(kpCtrlGroup, 2, 0);
  layout->addWidget(meshGroup, 3, 0);
  layout->addWidget(debugGroup, 4, 0);
  widget->setLayout(layout);

  mainWindow->setCentralWidget(viewer);
  mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
  mainWindow->addToolBar(Qt::BottomToolBarArea, timeline);
  mainWindow->setWindowTitle("Moving Kelvinlets Experiment");
  mainWindow->setWindowIcon(QIcon("/home/siraudin/Dev/railedkelvinlets/Unikel/icons/icon.png"));
  mainWindow->show();

  return application.exec();
}
