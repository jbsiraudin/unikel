
#include "mesh.h"
#include "kelvinlet.h"
#include "fieldadvector.h"

// Parsing
#include "basicIO.h"

// opengl and utilities
#include "gl/openglincludeQtComp.h"
#include <GL/glext.h>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFunctions>
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/frame.h>
#include <QGLViewer/manipulatedFrame.h>
#include <QGLViewer/keyFrameInterpolator.h>

#include "gl/GLUtilityMethods.h"

//QT stuff
#include <QFormLayout>
#include <QToolBar>
#include <QColorDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <QInputDialog>
#include <QLineEdit>
#include "../qt/QBrushDisk.h"
#include "../qt/TextureHandler.h"
#include "../qt/QSmartAction.h"

enum VIEWERMODE {
    SOLO_BRUSH,
    MOVING_BRUSH
};

struct QSmartTextOverlay{
    QString text;
    double transparency;
    double timetext;
    QTime timer;

    QSmartTextOverlay(){
        timetext = 1500.0; // 1.5 second
        transparency = 0.0;
    }

    void updateOverlayedText( QString const & t ) {
        text = t;
        transparency = 1.0;
        timer.restart();
    }

    bool updateTransparency(){
        if( transparency >= 0.0 ){
            transparency = 1.0 - (double)(timer.elapsed()) / timetext;
        }
        if( transparency <= 0.0 ) {
            transparency = 0.0;
            return false;
        }
        // else:
        return true;
    }
};

class uniViewer : public QGLViewer
{
    Q_OBJECT

    Mesh mesh;
    KelvinLet kelvinlet;
    std::vector<point3d> vertexDisplacements;
    std::vector<KelvinLet> kelvinlets;

    QWidget * controls;
    QSmartTextOverlay textOverlay;

public:
    uniViewer(QGLWidget *parent = nullptr) : QGLViewer(parent) {}

    void addActionsToToolbar(QToolBar *toolBar);
    void adjustCamera( point3d const & bb , point3d const & BB );

    void drawPath();
    void updateOverlayedText( QString const & t );

    void addToStepFactor(int t);

    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    //void wheelEvent( QWheelEvent * e );

    void draw();
    void postDraw();
    void init();
    QString helpString() const;

public slots:
    void updatePath();
    void updateKelvinlet();
    void updateKelvinlets();
    void openMesh();
    void saveMesh();
    void showControls();
    void saveSnapShotPlusPlus();

private:
    qglviewer::ManipulatedFrame **keyFrame_;
    qglviewer::KeyFrameInterpolator kfi_;
    QList<qglviewer::Frame> frameList_;
    QList<qglviewer::Frame> path_;
    int stepFactor = 10;
    int nbKeyFrames = 2;
    int currentKF_;
};
