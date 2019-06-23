
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

struct vec2 {
    int pos1;
    int pos2;

    vec2(int un, int deux) {
        pos1 = un;
        pos2 = deux;
    }
};

struct keyPoint{
    Vec position;
    double twist;
    double scale;
    double epsilon;

    keyPoint(){
        twist = 0.0;
        scale = 0.0;
        epsilon = 1.0;
    }

    keyPoint(Vec in){
        position = in;
        twist = 0.0;
        scale = 0.0;
        epsilon = 1.0;
    }
};

class uniViewer : public QGLViewer
{
    Q_OBJECT

    std::vector<point3d> vertexDisplacements;

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

    void addToLines(point3d point);

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
    void updateCov();

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
    inline void setSpeedFactor(double in) { speedFactor = in; update(); }
    inline void setDebugView(bool in) { debugView = in; update(); }

    void setShearModulus(double a);
    void setPoissonModulus(double b);

    inline void setTimeMax(int time) { timeMax_ = time; }
    void initUniKelvinlet();
    void initAnimation();
    void nextUniKelvinlet();

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
    Mesh mesh_, meshInit_;
    Mesh debugSpheres_, debugSpheresInit_;
    QList<vec2> debugLines_;
    mat33d cov_;

    double shearModulus = 1.0;
    double poissonModulus = 0.5;
    int stepFactor = 10;
    double speedFactor = 0.1;
    int nbKeyPoints = 2;
    int currentKP_;

    QList<unsigned int> activeKL_;

    int time_ = 0;
    int timeMax_ = 75;

    bool debugView = true;
};
