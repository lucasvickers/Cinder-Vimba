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

#include "civimba/CameraController.h"
#include "civimba/ErrorCodeToMessage.h"

#include <thread>

namespace civimba {

using namespace AVT::VmbAPI;

CameraController::CameraController( uint32_t numberFrames )
:   mColorProcessing( COLOR_PROCESSING_OFF ),
    mFrameInfo( FRAME_INFO_AUTO ),
    mNewFrame( false ),
    mFrameObserver( nullptr ),
    mNumberFrames( numberFrames )
{
    if( mNumberFrames == 0 ) {
        throw CameraControllerException( __FUNCTION__, "CameraController needs at least one frame.", VmbErrorBadParameter );
    }
}

CameraController::~CameraController()
{
    if( mCamera ) {
        mCamera->Close();
    }
}

void CameraController::setFrameInfo( FrameInfo info )
{
    if( mFrameObserver ) {
        mFrameObserver->setFrameInfo( info );
    }
}

void CameraController::setColorProcessing( ColorProcessing cp )
{
    if( mFrameObserver ) {
        mFrameObserver->setColorProcessing( cp );
    }
}

cinder::Surface8uRef CameraController::getCurrentFrame()
{
    // lock isn't really needed
    std::lock_guard<std::mutex> lock( mFrameMutex );
    return mCurrentFrame;
}

bool CameraController::checkNewFrame()
{
    std::lock_guard<std::mutex> lock( mCheckFrameMutex );
    bool status = mNewFrame;
    mNewFrame = false;
    return status;
}

void CameraController::frameObservedCallback( cinder::Surface8uRef &frame )
{
    // lock and swap surfaces
    std::lock_guard<std::mutex> lock( mFrameMutex );
    mCurrentFrame = frame;
    mNewFrame = true;
}

void CameraController::startContinuousImageAcquisition()
{
    using namespace std::placeholders;

    if( mFrameObserver ) {
        // TODO log that we ignored this
        return;
    }
    // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
    mFrameObserver = new FrameObserver( mCamera, std::bind( &CameraController::frameObservedCallback, this, _1 ), mFrameInfo, mColorProcessing );
    
    // Start streaming.  FrameObserver* gets managed elsewhere
    VmbErrorType res = mCamera->StartContinuousImageAcquisition( NUM_FRAMES, IFrameObserverPtr( mFrameObserver ));

    if( res != VmbErrorSuccess ) {
        throw CameraControllerException( __FUNCTION__, ErrorCodeToMessage( res ), VmbErrorOther );      
    }
}
    

void CameraController::stopContinuousImageAcquisition()
{
    // Stop streaming
    mCamera->StopContinuousImageAcquisition();
    mFrameObserver = nullptr;
}

} // namespace civimba