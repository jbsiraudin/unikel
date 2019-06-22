
#include "mesh.h"
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

struct keyPoint{
    Vec position;
    double twist;
    double scale;
    double epsilon;

    keyPoint(){
        twist = 0.0;
        scale = 1.0;
        epsilon = 0.1;
    }

    keyPoint(Vec in){
        position = in;
        twist = 0.0;
        scale = 1.0;
        epsilon = 0.1;
    }
};

class uniViewer : public QGLViewer
{
    Q_OBJECT

    Mesh mesh;
    std::vector<point3d> vertexDisplacements;

    QWidget * controls;

public:
    uniViewer(QGLWidget *parent = nullptr) : QGLViewer(parent) {}

    void adjustCamera( point3d const & bb , point3d const & BB );

    void drawPath();
    void updateOverlayedText( QString const & t );

    inline int getNbKeypoints() { return nbKeyPoints; }
    inline int getStepFactor() { return stepFactor; }
    inline bool getDebugView() { return debugView; }
    inline keyPoint getKeyPoint(int idx) { return keyPoints_[idx]; }
    inline int getTimeMax() { return timeMax_; }

    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void animate();
    void draw();
    void init();
    QString helpString() const;

public slots:
    void updatePath();
    void updateKelvinlets();

    void updateKeyPointPosition(int i);
    void addKeyPoint();
    void removeKeyPoint();

    void setKpPosX(int i, double x);
    void setKpPosY(int i, double y);
    void setKpPosZ(int i, double z);
    inline void setKpTwist(int i, double twist) { keyPoints_[i].twist = twist; update(); updatePath();}
    inline void setKpScale(int i, double scale) { keyPoints_[i].scale = scale; update(); updatePath();}
    inline void setKpEps(int i, double epsilon) { keyPoints_[i].epsilon = epsilon; update(); updatePath(); }

    inline void setStepFactor(int in) { stepFactor = in; update(); updatePath(); }
    inline void setDebugView(bool in) { debugView = in; update(); }

    void setShearModulus(double a);
    void setPoissonModulus(double b);

    void openMesh();
    void saveMesh();
    void saveSnapShot();

signals:
    void pathUpdated();
    void addedKeyPoint(int i);
    void rmvedKeyPoint(int i);
    void updatedKpInFocus(int i);
    void updatedKpPosX(double x);
    void updatedKpPosY(double y);
    void updatedKpPosZ(double z);
    void updatedKpEpsilon(double epsilon);
    void meshOpen(QString vertices);
    void updatedShear(double a);
    void updatedPoisson(double b);
    void timed(int time);

private:
    qglviewer::ManipulatedFrame **keyPoint_;
    QList<keyPoint> keyPoints_;
    QList<uniKelvinLet> unikelvinlets_;
    QList<Mesh> meshes_;
    QList<point3d> debugSpheres_, debugSpheresInit_;
    double shearModulus = 1.0;
    double poissonModulus = 0.5;
    int stepFactor = 10;
    int nbKeyPoints = 2;
    int currentKP_;
    int time_ = 0;
    int timeMax_ = 100;

    bool debugView = true;
};
