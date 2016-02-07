#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "civimba/CiVimba.h"

using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the App class.
class AsyncGrab : public App {
  public:

    void mouseDrag( MouseEvent event ) override;

    void keyDown( KeyEvent event ) override;

    void draw() override;

    void update() override;

    void setup() override;

    void cleanup() override;

  private:
    civimba::ApiController          mController;

    std::vector<civimba::CameraControllerRef>    mCameras;
    std::vector<gl::Texture2dRef>                mTextures;

};

void prepareSettings( AsyncGrab::Settings* settings )
{
#if defined( CINDER_ANDROID )
    settings->setMultiTouchEnabled( false );
#endif
}

void AsyncGrab::mouseDrag( MouseEvent event )
{
    // Store the current mouse position in the list.
}

void AsyncGrab::keyDown( KeyEvent event )
{
    if( event.getChar() == 'a' || event.getChar() == 'A' ) {
        // stArt
        for( auto &cam : mCameras ) {
            cam->startContinuousImageAcquisition();
        }
        console() << "Started acquisition." << std::endl;
    } else if( event.getChar() == 'o' || event.getChar() == 'O' ) {
        // stOp
        for( auto &cam : mCameras ) {
            cam->stopContinuousImageAcquisition();
        }
        console() << "Stopped acquisition." << std::endl;
    }
}

void AsyncGrab::setup()
{
    mController.startup();
    auto cameras = mController.getCameraList();
    for( auto &cam : cameras ) {
        std::string id;
        cam->GetID( id );
        mCameras.push_back( mController.getCamera( id ) );
    }
    mTextures.resize( mCameras.size() );
    for( auto &cam : mCameras ) {
        cam->startContinuousImageAcquisition();
    }
}

void AsyncGrab::cleanup()
{
    for( auto &cam : mCameras ) {
        cam->stopContinuousImageAcquisition();
    }
    mController.shutdown();
}

void AsyncGrab::draw()
{
    // Clear the contents of the window. This call will clear
    // both the color and depth buffers. 
    gl::clear( );
    int width = app::getWindowWidth() / mTextures.size();

    int offset = 0;
    for( auto& tex : mTextures ) {
        if( tex ) {
            float ratio = ((float)tex->getWidth()) / tex->getHeight();
            int height = width / ratio;
            gl::draw( tex, Area( offset, 0, offset + width, height ) );
        }
        offset += width;
    }
}

void AsyncGrab::update()
{
    int index = 0;
    for( auto& cam : mCameras ) {
        if( cam->checkNewFrame() ) {
            mTextures[index] = gl::Texture2d::create(*cam->getCurrentFrame());
        }
        ++index;
    }
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( AsyncGrab, RendererGl, prepareSettings )
