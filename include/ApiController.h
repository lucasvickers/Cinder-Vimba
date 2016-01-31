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

#include <string>

#include "VimbaCPP/Include/VimbaCPP.h"

#include "CameraController.h"
#include "BaseException.h"

class ApiController
{
  public:

    class ApiControllerException: public BaseException
    {
    public:
        ApiControllerException( const char* const &fun, const char* const &msg, VmbErrorType result = VmbErrorOther )
            : BaseException( fun, msg, result )
        { }
        ApiControllerException( const char* const &fun, const std::string &msg, VmbErrorType result = VmbErrorOther )
            : BaseException( fun, msg, result )
        { }
        ~ApiControllerException() throw()
        { }
    };

    ApiController();
    ~ApiController();

    void startup();
    void shutdown();

    AVT::VmbAPI::CameraPtrVector getCameraList() const;
    CameraControllerPtr getCamera( const std::string &cameraID, const config::ProgramConfig &config );

    std::string  getVersion() const;

  private:
    // configure camera settings to ensure compatability with TransformImage
    // throws
    void prepareCamera( CameraControllerPtr& cam );

    // does not throw
    VmbErrorType setIntFeatureValueModulo2( CameraControllerPtr& cam, const char* const& Name );

    // A reference to our Vimba singleton
    AVT::VmbAPI::VimbaSystem   & mSystem;
};

