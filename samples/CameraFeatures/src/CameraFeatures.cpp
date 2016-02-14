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
#include "cinder/Log.h"
#include "cinder/params/Params.h"

#include "civimba/CiVimba.h"

#include "ParamFeature.h"

using namespace ci;
using namespace ci::app;
using namespace ci::log;

using namespace civimba;
using namespace civimba::featurecontainer;

// We'll create a new Cinder Application by deriving from the App class.
class CameraFeatures : public App {
  public:

    void mouseDrag( MouseEvent event ) override;

    void keyDown( KeyEvent event ) override;

    void draw() override;

    void update() override;

    void setup() override;

    void cleanup() override;

  private:
    ApiController           mController;

    CameraControllerRef     mCamera;
    gl::Texture2dRef        mTexture;

    FeatureDoubleRef        mFrameRateAbs;
    FeatureDoubleRef        mExposureTimeAbs;

    FeatureDoubleRef        mGainAmount;
    FeatureEnumRef          mGainAuto;

    FeatureEnumRef          mPixelFormat;

    params::InterfaceGlRef	mParams;
};

void prepareSettings( CameraFeatures::Settings* settings )
{
#if defined( CINDER_ANDROID )
    settings->setMultiTouchEnabled( false );
#endif
}

void CameraFeatures::mouseDrag( MouseEvent event )
{
    // Store the current mouse position in the list.
}

void CameraFeatures::keyDown( KeyEvent event )
{

}

void CameraFeatures::setup()
{
    mController.startup();
    auto cameras = mController.getCameraList();
    if( cameras.size() > 0 ) {
        std::string id;
        cameras.front()->GetID( id );
        mCamera = mController.getCamera( id );
        mCamera->startContinuousImageAcquisition();
    } else {
        CI_LOG_E( "No cameras detected." );
    }

    mParams = params::InterfaceGl::create( getWindow(), "Camera Parameters", toPixels( ivec2( 200, 300 ) ) );

    mParams->addText( "Camera ID " + mCamera->getID() );
    mParams->addText( "Camera Model " + mCamera->getModel() );
    mParams->addText( "Camera Name " + mCamera->getName() );
    mParams->addSeparator();

    // useful code for identifying the feature types
    /*
    std::vector<std::string> neededFeatures = {
            "AcquisitionFrameRateAbs",  // float64 (double)
            "ExposureTimeAbs",          // float64 (double)
            "ExposureAuto",             // enum
            "Gain",                     // float64 (double)
            "GainAuto",                 // enum
            "PixelFormat"               // enum
            // TODO add white balance
    };

    for( auto name : neededFeatures ) {
        AVT::VmbAPI::FeaturePtr feature = mCamera->getFeatureByName( name );
        console() << "Feature " << name << " is of type " << FeatureAccessor::getDataTypeString( feature ) << std::endl;
    }
    */

    // set up our params
    try {
        mFrameRateAbs = createContainer<FeatureDouble>(mCamera->getFeatureByName("AcquisitionFrameRateAbs"));
        mExposureTimeAbs = createContainer<FeatureDouble>(mCamera->getFeatureByName("ExposureTimeAbs"));
        mGainAmount = createContainer<FeatureDouble>(mCamera->getFeatureByName("Gain"));

        mGainAuto = createContainer<FeatureEnum>(mCamera->getFeatureByName("GainAuto"));
        mPixelFormat = createContainer<FeatureEnum>(mCamera->getFeatureByName("PixelFormat"));
    } catch ( civimba::BaseException& e ) {
        CI_LOG_E( "Exception while setting up containers: " << e.Message() );
    }
}

void CameraFeatures::cleanup()
{
    mCamera->stopContinuousImageAcquisition();
    mController.shutdown();
}

void CameraFeatures::draw()
{
    // Clear the contents of the window. This call will clear
    // both the color and depth buffers. 
    gl::clear( );
    if( mTexture ) {
        gl::draw( mTexture, app::getWindowBounds() );
    }

    // Draw the interface
    mParams->draw();
}

void CameraFeatures::update()
{
    if( mCamera->checkNewFrame() ) {
        mTexture = gl::Texture2d::create( *mCamera->getCurrentFrame() );
    }
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( CameraFeatures, RendererGl, prepareSettings )
