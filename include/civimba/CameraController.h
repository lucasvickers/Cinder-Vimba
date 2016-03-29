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

#include <memory>
#include <mutex>
#include <functional>

#include "VimbaCPP/Include/VimbaCPP.h"

#include "civimba/FrameObserver.h"
#include "civimba/Types.h"
#include "civimba/BaseException.h"

#include "cinder/Surface.h"

namespace civimba {

typedef std::shared_ptr<class CameraController> CameraControllerRef;

class CameraController {
	friend class ApiController;

  public:

	class CameraControllerException : public BaseException
	{

	  public:
		CameraControllerException( const char *const &fun, const char *const &msg,
		                           VmbErrorType result = VmbErrorOther )
			: BaseException( fun, msg, result )
		{ }

		CameraControllerException( const char *const &fun, const std::string &msg,
		                           VmbErrorType result = VmbErrorOther )
			: BaseException( fun, msg, result )
		{ }

		~CameraControllerException() throw()
		{ }
	};

	CameraController( uint32_t numberFrames = 5 );

	~CameraController();

	ColorProcessing getColorProcessing() { return mColorProcessing; }

	void setColorProcessing( ColorProcessing cp );

	std::vector<AVT::VmbAPI::FeaturePtr> getFeatures();

	AVT::VmbAPI::FeaturePtr getFeatureByName( const char *name );

	AVT::VmbAPI::FeaturePtr getFeatureByName( const std::string &name );

	void startContinuousImageAcquisition();

	void stopContinuousImageAcquisition();

	void setFrameLogging( FrameLoggingInfo logging );

	FrameLoggingInfo getFrameLogging() { return mFrameLoggingInfo; }

	cinder::Surface8uRef getCurrentFrame();

	bool checkNewFrame();

	std::string getID();

	std::string getName();

	std::string getModel();

	// expose raw camera for low level requests that are not exposed.
	AVT::VmbAPI::CameraPtr getCamera() { return mCamera; }

  private:

	void frameObservedCallback( cinder::Surface8uRef &frame );

	AVT::VmbAPI::CameraPtr mCamera;
	FrameObserver *mFrameObserver;

	std::mutex mFrameMutex;
	// TODO support other formats
	cinder::Surface8uRef mCurrentFrame;
	std::mutex mCheckFrameMutex;
	bool mNewFrame;

	ColorProcessing mColorProcessing;
	FrameLoggingInfo mFrameLoggingInfo;

	uint32_t mNumberFrames;
};

} // namespace civimba