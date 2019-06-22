#include "viewer.h"


using namespace qglviewer;
using namespace std;


///////////////////////   uniViewer  ///////////////////////

void uniViewer::adjustCamera(const point3d &bb, const point3d &BB)
{
    point3d const & center = ( bb + BB )/2.f;
    setSceneCenter( qglviewer::Vec( center[0] , center[1] , center[2] ) );
    setSceneRadius( 1.5 * ( BB - bb ).norm() );
    showEntireScene();
}

void uniViewer::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_H:
        help();
    default:
        QGLViewer::keyPressEvent(event);
    }
}

void uniViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
    QGLViewer::mouseDoubleClickEvent(e);
}

void uniViewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
}

void uniViewer::mouseMoveEvent(QMouseEvent* e  ){
    QGLViewer::mouseMoveEvent(e);
}

void uniViewer::mouseReleaseEvent(QMouseEvent* e  ) {
    QGLViewer::mouseReleaseEvent(e);
}

QString uniViewer::helpString() const {
    QString text("<h2>UniKel</h2>");
    text += "<h3>Kelvinlets experiment</h3>";
    text += "Create a path for the kelvinlets to follow. ";
    text += "Right click on a keypoint to drag it around.";
    return text;
}

void uniViewer::init()
{
    makeCurrent();
    //initializeOpenGLFunctions();

    setMouseTracking(true);// Needed for MouseGrabber.

    setBackgroundColor(QColor(255,255,255));

    // Lights:
    GLTools::initLights();
    GLTools::setSunsetLight();
    GLTools::setDefaultMaterial();

    //
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CW); // CCW ou CW

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CLIP_PLANE0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_COLOR_MATERIAL);

    setSceneCenter( qglviewer::Vec( 0 , 0 , 0 ) );
    setSceneRadius( 10.0 );
    showEntireScene();

    // An array of manipulated (key) frames.
    keyPoint_ = new ManipulatedFrame *[nbKeyPoints];

    // Create an initial path
    for (int i = 0; i < nbKeyPoints; i++) {
        keyPoint_[i] = new ManipulatedFrame();
        keyPoint_[i]->setPosition(-1.0 + 2.0 * i / (nbKeyPoints - 1), 0.0, 0.0);
        keyPoints_.push_back(keyPoint(keyPoint_[i]->position()));
    }

    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {
            for (int k = -3; k <= 3; k++) {
                debugSpheres_.push_back(Vec(i/3.0, j/3.0, k/3.0));
                debugSpheresInit_.push_back(Vec(i/3.0, j/3.0, k/3.0));
            }
        }
    }

    updatePath();

    currentKP_ = 0;
    setManipulatedFrame(keyPoint_[currentKP_]);

    // Enable direct frame manipulation when the mouse hovers.
    setMouseTracking(true);

    //help();
    for (int i = 0; i < nbKeyPoints ; i++) {
        connect(keyPoint_[i], SIGNAL(manipulated()), SLOT(update()));
        connect(keyPoint_[i], SIGNAL(manipulated()), SLOT(updatePath()));
        connect(keyPoint_[i], &ManipulatedFrame::manipulated, [=]() { uniViewer::updateKeyPointPosition(i); });
    }
}

void uniViewer::draw()
{
    //Draw mesh
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_LIGHTING );

    if (!debugView) {
        glColor3f(0.5f,0.5f,0.8f);
        glBegin(GL_TRIANGLES);
        for( unsigned int t = 0 ; t < mesh.triangles.size() ; ++t ) {
            point3d const & p0 = mesh.vertices[mesh.triangles[t][0]].p + vertexDisplacements[mesh.triangles[t][0]];
            point3d const & p1 = mesh.vertices[mesh.triangles[t][1]].p + vertexDisplacements[mesh.triangles[t][1]];
            point3d const & p2 = mesh.vertices[mesh.triangles[t][2]].p + vertexDisplacements[mesh.triangles[t][2]];
            point3d const & n = point3d::cross( p1-p0 , p2-p0 ).direction();
            n.glNormal();
            p0.glVertex();
            p1.glVertex();
            p2.glVertex();
        }
        glEnd();
    } else {
        //cout << debugSpheres_[1] << endl;
        glColor3f(36.0f/255.0f,153.0f/255.0f,252.0f/255.0f);
        for (int i = 0; i < debugSpheresInit_.size(); i++) {
            BasicGL::drawSphere(debugSpheres_[i].x(), debugSpheres_[i].y(), debugSpheres_[i].z(), 0.05, 6, 6);
        }
    }

    for (int i = 0; i < nbKeyPoints; ++i) {
      glPushMatrix();
      glMultMatrixd(keyPoint_[i]->matrix());

      if ((i == currentKP_) || (keyPoint_[i]->grabsMouse()))
        drawAxis(0.4*10/stepFactor);
      else
        drawAxis(0.2*10/stepFactor);

      glPopMatrix();
    }

    drawPath();
    emit timed(time_);
}

void uniViewer::updatePath() {
    unikelvinlets_.clear();

    for (int i = 0; i < nbKeyPoints; ++i) {
      keyPoints_[i].position = keyPoint_[i]->position();
    }

    static uniKelvinLet tmp;
    keyPoint *kp_[4];
    kp_[0] = &keyPoints_[0];
    kp_[1] = kp_[0];
    int index = 1;
    kp_[2] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;
    index++;
    kp_[3] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;

    while (kp_[2] != nullptr) {
        Vec diff = kp_[2]->position - kp_[1]->position;
        Vec tgP_1 = 0.5 * (kp_[2]->position - kp_[0]->position);
        Vec tgP_2 = kp_[3] ? 0.5 * (kp_[3]->position - kp_[1]->position) : kp_[2]->position;

        Vec v1 = 3.0 * diff - 2.0 * tgP_1 - tgP_2;
        Vec v2 = -2.0 * diff + tgP_1 + tgP_2;

        double scale_diff = kp_[2]->scale - kp_[1]->scale;
        double twist_diff = kp_[2]->twist - kp_[1]->twist;
        double epsilon_diff = kp_[2]->epsilon - kp_[1]->epsilon;

        int nbSteps = floor(stepFactor * diff.norm());

        for (int step = 0; step < nbSteps; step++) {
            qreal alpha = step / static_cast<qreal>(nbSteps);
            qreal alpha_plus = (step+1) / static_cast<qreal>(nbSteps);
            Vec posStart = kp_[1]->position + alpha * (tgP_1 + alpha * (v1 + alpha * v2));
            Vec posEnd = kp_[1]->position + alpha_plus * (tgP_1 + alpha_plus * (v1 + alpha_plus * v2));

            tmp.setCenterEnd(point3d(posStart.x, posStart.y, posStart.z), point3d(posEnd.x, posEnd.y, posEnd.z));
            tmp.scaling_s = kp_[1]->scale + alpha * scale_diff;
            tmp.twisting_q = kp_[1]->twist + alpha * twist_diff;
            tmp.epsilon = kp_[1]->epsilon + alpha * epsilon_diff;

            unikelvinlets_.push_back(tmp);
        }

        // Shift
        kp_[0] = kp_[1];
        kp_[1] = kp_[2];
        kp_[2] = kp_[3];
        index++;
        kp_[3] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;
    }
    //updateKelvinlets();
}

void uniViewer::updateKelvinlets() {
    //FieldAdvector fieldAdvector;
    for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
        vertexDisplacements[v] = point3d(0, 0, 0);

//        Q_FOREACH(uniKelvinLet kel, unikelvinlets_) {
//            vertexDisplacements[v] += kel.computeNaiveVelocity(mesh.vertices[v].p+vertexDisplacements[v]);
//        }
    }

    Q_FOREACH(uniKelvinLet kel, unikelvinlets_) {
        for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
            vertexDisplacements[v] += kel.computeVelocity(mesh.vertices[v].p+vertexDisplacements[v]);
        }
    }
}

void uniViewer::animate() {
    if(time_++ < timeMax_) {
        Q_FOREACH(uniKelvinLet kel, unikelvinlets_) {
            //kel.printInfo();
            for (int i = 0; i < debugSpheres_.size(); i++) {
                debugSpheres_[i] += kel.computeVelocity(debugSpheres_[i]);
            }
        }
    } else {
        time_ = 0;
        for (int i = 0; i < debugSpheres_.size(); i++) {
            debugSpheres_[i] = debugSpheresInit_[i];
        }
    }
}

void uniViewer::drawPath() {
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    Q_FOREACH (uniKelvinLet kel, unikelvinlets_)
        kel.center.glVertex();
    glEnd();

    Q_FOREACH (uniKelvinLet kel, unikelvinlets_) {
        glColor3f(kel.epsilon/20.f, 1.f - kel.scaling_s, kel.twisting_q/360.l);
        drawArrow(Vec(kel.center), Vec(kel.end));
    }
}

void uniViewer::openMesh()
{
    bool success = false;
    QString fileName = QFileDialog::getOpenFileName(NULL,"","");
    if ( !fileName.isNull() ) { // got a file name
        if(fileName.endsWith(QString(".off")))
            success = OFFIO::openTriMesh(fileName.toStdString() , mesh.vertices , mesh.triangles );
        else if(fileName.endsWith(QString(".obj")))
            success = OBJIO::openTriMesh(fileName.toStdString() , mesh.vertices , mesh.triangles );
        if(success) {
            std::cout << fileName.toStdString() << " was opened successfully" << std::endl;
            mesh.updateBoundingBox();
            emit meshOpen(QStringLiteral("Mesh vertices : ")+std::to_string(mesh.vertices.size()).c_str());
            adjustCamera(mesh.boundingBox.bb,mesh.boundingBox.BB);
            for (int i = 0; i < keyPoints_.size(); i++) {
                keyPoints_[i].epsilon = 0.6 * mesh.boundingBox.diagonal();
                cout << keyPoints_[i].epsilon << endl;
                //emit updatedKpInFocus(i);
                //emit updatedKpEpsilon(keyPoints_[i].epsilon);
            }
            Q_FOREACH (uniKelvinLet kel, unikelvinlets_) {
                kel.setElasticityParameters(0.1 / mesh.boundingBox.squareDiagonal() , 0.3);
            }
            emit updatedShear(0.1 / mesh.boundingBox.squareDiagonal());
            emit updatedPoisson(0.3);
            vertexDisplacements.clear();
            vertexDisplacements.resize( mesh.vertices.size() , point3d(0,0,0) );
            update();
        }
        else
            std::cout << fileName.toStdString() << " could not be opened" << std::endl;
    }

    updateKelvinlets();
}

void uniViewer::saveMesh()
{
    bool success = false;
    QString fileName = QFileDialog::getOpenFileName(nullptr,"","");
    if ( !fileName.isNull() ) { // got a file name
        if(fileName.endsWith(QString(".off")))
            success = OFFIO::save(fileName.toStdString() , mesh.vertices , mesh.triangles );
        else if(fileName.endsWith(QString(".obj")))
            success = OBJIO::save(fileName.toStdString() , mesh.vertices , mesh.triangles );
        if(success)
            std::cout << fileName.toStdString() << " was saved" << std::endl;
        else
            std::cout << fileName.toStdString() << " could not be saved" << std::endl;
    }
}

void uniViewer::saveSnapShot()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr,"*.png","");
    if ( !fileName.isNull() ) {
        setSnapshotFormat("PNG");
        setSnapshotQuality(100);
        saveSnapshot( fileName );
    }
}

void uniViewer::updateKeyPointPosition(int i) {
    Vec pos = keyPoint_[i]->position();
    emit updatedKpInFocus(i);
    emit updatedKpPosX(pos.x);
    emit updatedKpPosY(pos.y);
    emit updatedKpPosZ(pos.z);
}

void uniViewer::addKeyPoint() {
    cout << "add to " << nbKeyPoints << endl;
    nbKeyPoints++;
    emit addedKeyPoint(nbKeyPoints);
    cout << "result " << nbKeyPoints << endl;
    keyPoint_[nbKeyPoints - 1] = new ManipulatedFrame();
    keyPoint_[nbKeyPoints - 1]->setPosition(nbKeyPoints, 0, 0);
    keyPoints_.push_back(keyPoint_[nbKeyPoints - 1]->position());
    cout << keyPoint_[nbKeyPoints - 1]->position() << endl;

    connect(keyPoint_[nbKeyPoints - 1], SIGNAL(manipulated()), SLOT(update()));
    connect(keyPoint_[nbKeyPoints - 1], SIGNAL(manipulated()), SLOT(updatePath()));
    connect(keyPoint_[nbKeyPoints - 1], &ManipulatedFrame::manipulated, [=]() { uniViewer::updateKeyPointPosition(nbKeyPoints - 1); });
    updatePath();
    update();
}

void uniViewer::removeKeyPoint() {
    if (nbKeyPoints > 2) {
        nbKeyPoints--;
        emit rmvedKeyPoint(nbKeyPoints);
        keyPoints_.pop_back();
        keyPoint_[nbKeyPoints] = nullptr;
        updatePath();
        update();
    }
}

void uniViewer::setKpPosX(int i, double x) {
    Vec initPos = keyPoint_[i]->position();
    keyPoint_[i]->setPosition(x, initPos.y, initPos.z);
    updatePath();
    update();
}

void uniViewer::setKpPosY(int i, double y) {
    Vec initPos = keyPoint_[i]->position();
    keyPoint_[i]->setPosition(initPos.x, y, initPos.z);
    updatePath();
    update();
}

void uniViewer::setKpPosZ(int i, double z) {
    Vec initPos = keyPoint_[i]->position();
    keyPoint_[i]->setPosition(initPos.x, initPos.y, z);
    update();
    updatePath();
}

void uniViewer::setShearModulus(double a) {
    shearModulus = a;
    Q_FOREACH (uniKelvinLet kel, unikelvinlets_) {
        kel.setElasticityParameters(shearModulus , poissonModulus);
    }
    updateKelvinlets();
    update();
}

void uniViewer::setPoissonModulus(double b) {
    poissonModulus = b;
    Q_FOREACH (uniKelvinLet kel, unikelvinlets_) {
        kel.setElasticityParameters(shearModulus , poissonModulus);
    }
    updateKelvinlets();
    update();
}
