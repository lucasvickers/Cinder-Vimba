/*
 Copyright (c) 2016, Lucas Vickers

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
    the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
    the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

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
