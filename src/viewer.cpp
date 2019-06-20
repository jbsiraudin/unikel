#include "viewer.h"


using namespace qglviewer;
using namespace std;

///////////////////////   uni V i e w e r  ///////////////////////

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
    case Qt::Key_Left:
        currentKP_ = (currentKP_ + nbKeyPoints - 1) % nbKeyPoints;
        setManipulatedFrame(keyPoint_[currentKP_]);
        update();
        break;
    case Qt::Key_Right:
        currentKP_ = (currentKP_ + 1) % nbKeyPoints;
        setManipulatedFrame(keyPoint_[currentKP_]);
        update();
        break;
    case Qt::Key_Up:
        updateOverlayedText("epsilon +1");
        for (unsigned long i = 0; i < kelvinlets.size(); ++i) {
            kelvinlets[i].epsilon += 1.0;
        }
        updatePath();
        break;
    case Qt::Key_Down:
        updateOverlayedText("epsilon -1");
        for (unsigned long i = 0; i < kelvinlets.size(); ++i) {
            kelvinlets[i].epsilon -= 1.0;
        }
        updatePath();
        break;
    case Qt::Key_J:
        updateOverlayedText("Test overlay");
        update();
    case Qt::Key_H:
        help();
    default:
        QGLViewer::keyPressEvent(event);
    }
}

void uniViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
    if( (e->modifiers() & Qt::ControlModifier)  &&  (e->button() == Qt::LeftButton) )
    {
        showControls();
        return;
    }

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
    text += "<p>";
    text += "<h3>Basics</h3>";
    text += "<p>";
    text += "<ul>";
    text += "<li>H   :   make this help appear</li>";
    text += "</ul>";

    text += "<h2>K e y F r a m e s</h2>";
    text += "A <i>KeyFrameInterpolator</i> holds an interpolated path defined by "
            "key frames. ";
    text += "It can then smoothly make its associed frame follow that path. Key "
            "frames can interactively be manipulated, even ";
    text += "during interpolation.<br><br>";
    text += "Note that the camera holds 12 such keyFrameInterpolators, binded to "
            "F1-F12. Press <b>Alt+Fx</b> to define new key ";
    text += "frames, and then press <b>Fx</b> to make the camera follow the "
            "path. Press <b>C</b> to visualize these paths.<br><br>";
    text += "<b>+/-</b> changes the interpolation speed. Negative values are "
            "allowed.<br><br>";
    text += "<b>Return</b> starts-stops the interpolation.<br><br>";
    text += "Use the left and right arrows to change the manipulated KeyFrame. ";
    text += "Press <b>Control</b> to move it or simply hover over it.";
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

    for (unsigned long i = 0; i < kelvinlets.size(); ++i) {
        kelvinlets[i].state = KelvinLet::GRABBED;
        kelvinlets[i].mode = KelvinLet::TRANSLATE;
        kelvinlets[i].scale = KelvinLet::MONOSCALE;
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
    glColor3f(0.5f,0.5f,0.8f);
    glBegin(GL_TRIANGLES);
    for( unsigned int t = 0 ; t < mesh.triangles.size() ; ++t ) {
        point3d const & p0 = mesh.vertices[mesh.triangles[t][0]].p + vertexDisplacements[mesh.triangles[t][0]];
        point3d const & p1 = mesh.vertices[mesh.triangles[t][1]].p + vertexDisplacements[mesh.triangles[t][1]];
        point3d const & p2 = mesh.vertices[mesh.triangles[t][2]].p + vertexDisplacements[mesh.triangles[t][2]];
        point3d const & n = point3d::cross( p1-p0 , p2-p0 ).direction();
        glNormal3f(n[0],n[1],n[2]);
        glVertex3f(p0[0],p0[1],p0[2]);
        glVertex3f(p1[0],p1[1],p1[2]);
        glVertex3f(p2[0],p2[1],p2[2]);
    }
    glEnd();

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

//    glEnable(GL_CULL_FACE);
//    glDisable(GL_DEPTH);
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glColor4f(0,1,1,0.4f);
//    BasicGL::drawSphere(kelvinlets[0].pos[0], kelvinlets[0].pos[1], kelvinlets[0].pos[2], kelvinlets[0].epsilon, 50, 50);

////    glColor4f(1,0,1,0.4f);
////    BasicGL::drawSphere(kelvinlets[1].center[0], kelvinlets[1].center[1], kelvinlets[1].center[2], kelvinlets[1].epsilon, 50, 50);

//    glDisable(GL_BLEND);
//    glEnable(GL_DEPTH);
//    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
}

void uniViewer::postDraw() {
    bool textNeedsDisplay = textOverlay.updateTransparency();
    if( textNeedsDisplay ){
        glColor4f(0,0,0,std::max<double>(0.0, textOverlay.transparency ));
        QFont fontToUse; fontToUse.setPixelSize(50);
        drawText( 50 , 100 ,  textOverlay.text  , fontToUse);
    }
}

void uniViewer::updatePath() {
    path_.clear();

    for (int i = 0; i < nbKeyPoints; ++i) {
      keyPoints_[i].position = keyPoint_[i]->position();
    }

    static Frame fr;
    keyPoint *kf_[4];
    kf_[0] = &keyPoints_[0];
    kf_[1] = kf_[0];
    int index = 1;
    kf_[2] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;
    index++;
    kf_[3] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;

    while (kf_[2] != nullptr) {
        Vec diff = kf_[2]->position - kf_[1]->position;
        Vec tgP_1 = 0.5 * (kf_[2]->position - kf_[0]->position);
        Vec tgP_2 = kf_[3] ? 0.5 * (kf_[3]->position - kf_[1]->position) : kf_[2]->position;

        Vec v1 = 3.0 * diff - 2.0 * tgP_1 - tgP_2;
        Vec v2 = -2.0 * diff + tgP_1 + tgP_2;

        int nbSteps = floor(stepFactor * diff.norm());

        for (int step = 0; step < nbSteps; step++) {
            qreal alpha = step / static_cast<qreal>(nbSteps);
            fr.setPosition(kf_[1]->position + alpha * (tgP_1 + alpha * (v1 + alpha * v2)));
            path_.push_back(fr);
        }

        // Shift
        kf_[0] = kf_[1];
        kf_[1] = kf_[2];
        kf_[2] = kf_[3];
        index++;
        kf_[3] = (index < keyPoints_.size()) ? &keyPoints_[index] : nullptr;
    }

    // Add last KeyFrame
    fr.setPosition(kf_[1]->position);
    path_.push_back(fr);

    //updateUniKelvinLets();
}

//void updateUniKelvinLets() {
//    cout << "prout" << endl;
//}

void uniViewer::updateKelvinlets() {
    kelvinlets.resize(path_.size()-1);

    for (int i = 0; i < path_.size()-1; ++i) {
        kelvinlets[i].center = point3d(path_[i].position().x, path_[i].position().y, path_[i].position().z);
        kelvinlets[i].pos = point3d(path_[i+1].position().x, path_[i+1].position().y, path_[i+1].position().z);
        kelvinlets[i].translation_f = kelvinlets[i].pos - kelvinlets[i].center;
    }

    FieldAdvector fieldAdvector;
    for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
        vertexDisplacements[v] = point3d(0, 0, 0);
    }

    for (unsigned long i = 0; i < kelvinlets.size(); ++i) {
        for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
            vertexDisplacements[v] += fieldAdvector.RungeKutta_RK4(mesh.vertices[v].p , kelvinlets[i] , 10);
        }
    }

//    cout << "Displacements" << endl;
//    for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
//        cout << vertexDisplacements[v] << endl;
//    }
}

void uniViewer::drawPath() {
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    Q_FOREACH (Frame fr, path_)
        glVertex3fv(fr.position());
    glEnd();

    glColor3f(1.f, 0.f, 0.f);
    drawArrow(0.5, 0.0, 12);
    for (int i = 0; i < path_.size() - 1; i++) {
        drawArrow(path_[i].position(), path_[i+1].position(), -1.0, 12);
    }

    glColor3f(1.f, 1.f, 1.f);
    //path_.empty();
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
            adjustCamera(mesh.boundingBox.bb,mesh.boundingBox.BB);
            for (unsigned long i = 0; i < kelvinlets.size(); ++i) {
                kelvinlets[i].epsilon = 0.1 * mesh.boundingBox.diagonal();
                kelvinlets[i].epsilon2 = 1.1 * kelvinlets[i].epsilon;
                kelvinlets[i].epsilon3 = 1.1 * kelvinlets[i].epsilon2;
                kelvinlets[i].setElasticityParameters( 0.1 / mesh.boundingBox.squareDiagonal() , 0.3 );

            }
            vertexDisplacements.clear();
            vertexDisplacements.resize( mesh.vertices.size() , point3d(0,0,0) );
            cout << "vertices: " << mesh.vertices.size() << endl;
            cout << "epsilon: " <<  kelvinlets[0].epsilon << endl;
            update();
        }
        else
            std::cout << fileName.toStdString() << " could not be opened" << std::endl;
    }

    //updateKelvinlets();
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

void uniViewer::showControls()
{
    // Show controls :
    controls->close();
    controls->show();
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

void uniViewer::updateOverlayedText(const QString &t) {
    textOverlay.updateOverlayedText(t);
    if( ! animationIsStarted() ) startAnimation(); // you need this so that the text is refreshed on screen even without mouse/keyboard events
}

void uniViewer::updateKeyPoint() {

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
    update();
    updatePath();
}

void uniViewer::setKpPosY(int i, double y) {
    Vec initPos = keyPoint_[i]->position();
    keyPoint_[i]->setPosition(initPos.x, y, initPos.z);
    update();
    updatePath();
}

void uniViewer::setKpPosZ(int i, double z) {
    Vec initPos = keyPoint_[i]->position();
    keyPoint_[i]->setPosition(initPos.x, initPos.y, z);
    update();
    updatePath();
}
