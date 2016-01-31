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

#include <sstream>
#include <iostream>

#include "ApiController.h"
#include "ErrorCodeToMessage.h"
#include "Common/StreamSystemInfo.h"

using namespace AVT::VmbAPI;

ApiController::ApiController()
    // Get a reference to the Vimba singleton
    : mSystem ( VimbaSystem::GetInstance() )
{ }

ApiController::~ApiController()
{
    mSystem.Shutdown();
}

void ApiController::startup()
{
    VmbErrorType res = mSystem.Startup();
    if( VmbErrorSuccess != res ) {
        throw ApiControllerException( __FUNCTION__, ErrorCodeToMessage( res ), res );
    }
}

void ApiController::shutdown()
{
    mSystem.Shutdown();
}

CameraControllerPtr ApiController::getCamera( const std::string &cameraID, const config::ProgramConfig &config )
{
    CameraControllerPtr cam = std::make_shared<CameraController>();

    VmbErrorType res = mSystem.OpenCameraByID( cameraID.c_str(), VmbAccessModeFull, cam->mCamera );
    if( VmbErrorSuccess != res ) {
        throw ApiControllerException( __FUNCTION__, ErrorCodeToMessage( res ), res );
    }

    // Set the GeV packet size to the highest possible value
    // We assume GigE camera
    FeaturePtr pCommandFeature;
    if ( VmbErrorSuccess == cam->mCamera->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ) ) {
        if ( VmbErrorSuccess == pCommandFeature->RunCommand() ) {
            bool bIsCommandDone = false;
            do {
                if ( VmbErrorSuccess != pCommandFeature->IsCommandDone( bIsCommandDone ) ) {
                     break;
                }
            } while ( false == bIsCommandDone );
        }
    }

    prepareCamera( cam );

    return cam;
}

/**setting a feature to maximum value that is a multiple of 2*/
VmbErrorType ApiController::setIntFeatureValueModulo2(  CameraControllerPtr& cam, const char* const& Name )
{
    VmbErrorType        result;
    FeaturePtr          feature;
    VmbInt64_t          value_min;
    VmbInt64_t          value_max;

    result = SP_ACCESS( cam->mCamera )->GetFeatureByName( Name, feature );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    result = SP_ACCESS( feature )->GetRange( value_min, value_max );
    if( VmbErrorSuccess != result )
    {
        return result;
    }
    value_max =( value_max>>1 )<<1;
    result = SP_ACCESS( feature )->SetValue ( value_max );
    return result;
}

/**prepare camera so that the delivered image will not fail in image transform*/
void ApiController::prepareCamera( CameraControllerPtr& cam )
{
    VmbErrorType result;
    result = setIntFeatureValueModulo2( cam, "Width" );
    if( VmbErrorSuccess != result ) {
        throw ApiControllerException( __FUNCTION__, ErrorCodeToMessage( result ), result );
    }
    result = setIntFeatureValueModulo2( cam, "Height" );
    if( VmbErrorSuccess != result ) {
        throw ApiControllerException( __FUNCTION__, ErrorCodeToMessage( result ), result );
    }
}

CameraPtrVector ApiController::getCameraList() const
{
    CameraPtrVector cameras;
    // Get all known cameras
    if ( VmbErrorSuccess == mSystem.GetCameras( cameras ) ) {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

std::string ApiController::getVersion() const
{
    std::ostringstream  os;
    os << mSystem;
    return os.str();
}

