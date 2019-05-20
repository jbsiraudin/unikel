#ifndef TEXTUREHANDLER_H
#define TEXTUREHANDLER_H

#include <QImage>

class ScalarTextureHandler{
    GLuint ui_text_id;

public:
    template< class glViewer >
    void initTexture( glViewer * viewer )
    {
        glEnable(GL_TEXTURE_2D);

        glGenTextures(1, &(ui_text_id) );

        QImage textimg , buf;
        buf.load("./icons/textures/texture5.png");
        textimg = QGLWidget::convertToGLFormat( buf );

        glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );

        ui_text_id = viewer->bindTexture( textimg, GL_TEXTURE_2D, GL_RGB );

        glDisable(GL_TEXTURE_2D);
    }

    void enableTexturesAndBind()
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D , ui_text_id );
        glColor3f(1,1,1);
    }
    void unBind()
    {
        glBindTexture( GL_TEXTURE_2D , 0 );
    }
    void bind()
    {
        glBindTexture( GL_TEXTURE_2D , ui_text_id );
        glColor3f(1,1,1);
    }
    void unBindAndDisableTextures()
    {
        glBindTexture( GL_TEXTURE_2D , 0 );
        glDisable(GL_TEXTURE_2D);
    }

    void setVal( float val )
    {
        glTexCoord2d(val, 0.f);
    }

    void setVal( float val , float valMin , float valMax )
    {
        val = std::min< float >( std::max< float >( val , valMin ) , valMax );
        glTexCoord2d((val-valMin) / (valMax - valMin), 0.f);
    }
};




class TextureHandler{
    GLuint ui_text_id;

public:
    template< class glViewer >
    void initTexture( glViewer * viewer , QString textureString , bool b_convertToGLFormat = false )
    {
        glEnable(GL_TEXTURE_2D);

        glGenTextures(1, &(ui_text_id) );

        QImage textimg;
        if(b_convertToGLFormat) {
            QImage buf;
            buf.load(textureString);
            textimg = QGLWidget::convertToGLFormat( buf );
        }
        else
            textimg.load(textureString);

        glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );

        ui_text_id = viewer->bindTexture( textimg, GL_TEXTURE_2D, GL_RGB );

        glDisable(GL_TEXTURE_2D);
    }

    void enableTexturesAndBind()
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D , ui_text_id );
        glColor3f(1,1,1);
    }
    void unBind()
    {
        glBindTexture( GL_TEXTURE_2D , 0 );
    }
    void bind()
    {
        glBindTexture( GL_TEXTURE_2D , ui_text_id );
        glColor3f(1,1,1);
    }
    void unBindAndDisableTextures()
    {
        glBindTexture( GL_TEXTURE_2D , 0 );
        glDisable(GL_TEXTURE_2D);
    }

    void setCoords( float u , float v )
    {
        glTexCoord2d(u,v);
    }
};


#endif // TEXTUREHANDLER_H
