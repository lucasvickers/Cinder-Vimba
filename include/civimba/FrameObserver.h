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

#pragma once

#include <queue>
#include <string>
#include <functional>

#include "VimbaCPP/Include/VimbaCPP.h"

#include "civimba/Types.h"

namespace civimba {

class FrameObserver : virtual public AVT::VmbAPI::IFrameObserver
{
public:

    typedef std::function<void( const std::vector<VmbUchar_t>&, VmbUint32_t, VmbUint32_t )> FrameCallback;

    // We pass the camera that will deliver the frames to the constructor
    FrameObserver( AVT::VmbAPI::CameraPtr camera,
                   FrameCallback callback,
                   FrameInfo frameInfo, 
                   ColorProcessing colorProcessing );
    
    // This is our callback routine that will be executed on every received frame
    virtual void FrameReceived( const AVT::VmbAPI::FramePtr frame );

    void setColorProcessing( ColorProcessing cp ) { mColorProcessing = cp; }
    void setFrameInfo( FrameInfo info ) { mFrameInfos = info; }

private:
    void ShowFrameInfos( const AVT::VmbAPI::FramePtr & frame );
    double GetTime();

    template <typename T>
    class ValueWithState
    {
    private:
        T mValue;
        bool mState;
    public:
        ValueWithState()
            : mState( false )
        {}
        ValueWithState( T &value )
            : mValue ( value )
            , mState( true )
        {}
        const T& operator()() const {
            return mValue;
        }
        void operator()( const T &value ) {
            mValue = value;
            mState = true;
        }
        bool IsValid() const {
            return mState;
        }
        void Invalidate() {
            mState = false;
        }
    };

    FrameInfo                       mFrameInfos;
    ColorProcessing                 mColorProcessing;
    ValueWithState<double>          mFrameTime;
    ValueWithState<VmbUint64_t>     mFrameID;
    std::string                     mCameraID;
    FrameCallback                   mFrameCallback;
};

} // namespace civimba