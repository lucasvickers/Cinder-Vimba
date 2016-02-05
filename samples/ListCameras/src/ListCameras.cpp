#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "civimba/CiVimba.h"

using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the App class.
class ListCameras : public App {
  public:

    void mouseDrag( MouseEvent event ) override;

    void keyDown( KeyEvent event ) override;

    void draw() override;

    void update() override;

    void setup() override;

  private:
    civimba::ApiController  mController;
};

void prepareSettings( ListCameras::Settings* settings )
{
#if defined( CINDER_ANDROID )
    settings->setMultiTouchEnabled( false );
#endif
}

void ListCameras::mouseDrag( MouseEvent event )
{
    // Store the current mouse position in the list.
}

void ListCameras::keyDown( KeyEvent event )
{

}

void ListCameras::setup()
{
    mController.startup();
    auto cameras = mController.getCameraList();
    for( auto &cam : cameras ) {
        std::string id;
        cam->GetID( id );
        console() << "Camera found, ID: " << id << std::endl;
    }
    console() << "Done listing cameras." << std::endl;
    mController.shutdown();
}

void ListCameras::draw()
{
    // Clear the contents of the window. This call will clear
    // both the color and depth buffers. 
    gl::clear( Color::gray( 0.1f ) );
}

void ListCameras::update()
{

}

// This line tells Cinder to actually create and run the application.
CINDER_APP( ListCameras, RendererGl, prepareSettings )
