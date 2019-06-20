
#include "mesh.h"
#include "kelvinlet.h"
#include "fieldadvector.h"
#include "unikelvinlet.h"

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

using namespace qglviewer;

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

struct keyPoint{
    Vec position;
    double rotation;
    double scale;
    double epsilon;

    keyPoint(){
        rotation = 0.0;
        scale = 1.0;
        epsilon = 10.0;
    }

    keyPoint(Vec in){
        position = in;
        rotation = 0.0;
        scale = 1.0;
        epsilon = 10.0;
    }
};

class uniViewer : public QGLViewer
{
    Q_OBJECT

    Mesh mesh;
    KelvinLet kelvinlet;
    std::vector<point3d> vertexDisplacements;
    std::vector<KelvinLet> kelvinlets;
    std::vector<uniKelvinLet> unikelvinlets;

    QWidget * controls;
    QSmartTextOverlay textOverlay;

public:
    uniViewer(QGLWidget *parent = nullptr) : QGLViewer(parent) {}

    void adjustCamera( point3d const & bb , point3d const & BB );

    void drawPath();
    void updateOverlayedText( QString const & t );

    inline int getNbKeypoints() { return nbKeyPoints; }
    inline int getStepFactor() { return stepFactor; }
    inline bool getDebugView() { return debugView; }
    inline keyPoint getKeyPoint(int idx) { return keyPoints_[idx]; }

    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void draw();
    void postDraw();
    void init();
    QString helpString() const;

public slots:
    void updatePath();
    void updateKelvinlets();
    //void updateUniKelvinLets();

    void updateKeyPointPosition(int i);
    void updateKeyPoint();
    void addKeyPoint();
    void removeKeyPoint();

    void setKpPosX(int i, double x);
    void setKpPosY(int i, double y);
    void setKpPosZ(int i, double z);
    inline void setKpRot(int i, double rot) { keyPoints_[i].rotation = rot; }
    inline void setKpScale(int i, double scale) { keyPoints_[i].scale = scale; }
    inline void setKpEps(int i, double epsilon) { keyPoints_[i].epsilon = epsilon; }

    inline void setStepFactor(int in) { stepFactor = in; update(); updatePath(); }
    inline void setDebugView(bool in) { debugView = in; update(); }

    void openMesh();
    void saveMesh();
    void showControls();
    void saveSnapShot();

signals:
    void pathUpdated();
    void addedKeyPoint(int i);
    void rmvedKeyPoint(int i);
    void updatedKpInFocus(int i);
    void updatedKpPosX(double x);
    void updatedKpPosY(double y);
    void updatedKpPosZ(double z);

private:
    qglviewer::ManipulatedFrame **keyPoint_;
    QList<keyPoint> keyPoints_;
    QList<qglviewer::Frame> path_;
    int stepFactor = 10;
    int nbKeyPoints = 2;
    int currentKP_;

    bool debugView = false;
};
