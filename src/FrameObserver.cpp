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

#include <iostream>
#include <iomanip>
#include <chrono>

#include "civimba/FrameObserver.h"
#include "civimba/TransformImage.h"
#include "civimba/Types.h"
#include "cinder/Log.h"

// TODO bring this file to my standards w/ formatting
// TODO move to cinder logging

using namespace ci::log;
using namespace std::chrono;
using namespace std;

namespace civimba {

using namespace AVT::VmbAPI;

FrameObserver::FrameObserver( CameraPtr camera,
                              FrameCallback callback,
                              FrameLoggingInfo frameLogging,
                              ColorProcessing colorProcessing )
		: IFrameObserver( camera ),
		  mFrameCallback( callback ),
		  mFrameLogging( frameLogging ),
		  mColorProcessing( colorProcessing )
{
	camera->GetID( mCameraID );
}

double FrameObserver::getTime()
{
	auto t1 = high_resolution_clock::now();
	double dTime = duration_cast<milliseconds>( t1.time_since_epoch()).count();
	return dTime / 1000;
}

void FrameObserver::printFrameSizeFormat( const FramePtr &pFrame, stringstream &ss )
{
	ss << " Size:";
	VmbUint32_t nWidth = 0;
	VmbErrorType res;
	res = pFrame->GetWidth( nWidth );
	if( VmbErrorSuccess == res ) {
		ss << nWidth;
	} else {
		ss << "?";
	}

	ss << "x";
	VmbUint32_t nHeight = 0;
	res = pFrame->GetHeight( nHeight );
	if( VmbErrorSuccess == res ) {
		ss << nHeight;
	} else {
		ss << "?";
	}

	ss << " Format:";
	VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
	res = pFrame->GetPixelFormat( ePixelFormat );
	if( VmbErrorSuccess == res ) {
		ss << std::hex;
		ss << "0x" << ePixelFormat;
		ss << std::dec;
	} else {
		ss << "?";
	}
}

void FrameObserver::printFrameStatus( VmbFrameStatusType eFrameStatus, stringstream &ss )
{
	switch( eFrameStatus ) {
		case VmbFrameStatusComplete:
			ss << "Complete";
			break;

		case VmbFrameStatusIncomplete:
			ss << "Incomplete";
			break;

		case VmbFrameStatusTooSmall:
			ss << "Too small";
			break;

		case VmbFrameStatusInvalid:
			ss << "Invalid";
			break;

		default:
			ss << "unknown frame status";
			break;
	}
}

void printFrameInfos( const FramePtr &pFrame, VmbFrameStatusType eFrameStatus, stringstream &ss )
{

}


void FrameObserver::logFrameInfos( const FramePtr &frame )
{
	VmbUint64_t frameID = 0;
	bool frameIDValid = false;
	VmbFrameStatusType frameStatus = VmbFrameStatusComplete;
	VmbErrorType res = VmbErrorSuccess;
	double fps = 0.0;
	bool fpsValid = false;
	VmbUint64_t framesMissing = 0;

	res = frame->GetFrameID( frameID );
	if( VmbErrorSuccess == res ) {
		frameIDValid = true;

		if( mFrameID.IsValid()) {
			if( frameID != (mFrameID() + 1)) {
				framesMissing = frameID - mFrameID() - 1;

				if( mFrameLogging >= FRAME_INFO_WARNINGS ) {
					if( 1 == framesMissing ) {
						CI_LOG_W( "1 missing frame detected" );
					} else {
						CI_LOG_W( framesMissing << " missing frames detected" );
					}
				}
			}
		}
		mFrameID( frameID );

		double frameTime = getTime();
		if( mFrameTime.IsValid() && 0 == framesMissing ) {
			double timeDiff = frameTime - mFrameTime();
			if( timeDiff > 0.0 ) {
				fps = 1.0 / timeDiff;
				fpsValid = true;
			}
		}
		mFrameTime( frameTime );

	} else {

		mFrameID.Invalidate();
		mFrameTime.Invalidate();
	}

	stringstream ss;

	res = frame->GetReceiveStatus( frameStatus );
	if( VmbErrorSuccess == res ) {

		if( VmbFrameStatusComplete == frameStatus ) {

			// only print if we're printing all frames
			if( FRAME_INFO_SHOW <= mFrameLogging ) {
				ss << "Camera ID:" << mCameraID << " Frame ID: ";
				frameIDValid ? ss << frameID << " " : ss << "? ";

				printFrameStatus( frameStatus, ss );
				printFrameSizeFormat( frame, ss );
				ss << " FPS:";
				if( fpsValid ) {
					std::streamsize s = ss.precision();
					ss << std::fixed << std::setprecision( 2 ) << fps << std::setprecision( s );
				} else {
					ss << "?";
				}
				CI_LOG_I( ss.str());
			}
		} else {
			// print warnings
			if( FRAME_INFO_WARNINGS <= mFrameLogging ) {
				ss << "Camera ID:" << mCameraID << " Frame ID: ";
				frameIDValid ? ss << frameID << " " : ss << "? ";

				printFrameStatus( frameStatus, ss );
				printFrameSizeFormat( frame, ss );
				ss << " FPS:";
				if( fpsValid ) {
					std::streamsize s = ss.precision();
					ss << std::fixed << std::setprecision( 2 ) << fps << std::setprecision( s );
				} else {
					ss << "?";
				}
				CI_LOG_W( ss.str());
			}
		}

	} else {
		// print error
		if( FRAME_INFO_ERRORS <= mFrameLogging ) {
			CI_LOG_E( "Error receiving frame status." );
		}
	}
}

void FrameObserver::FrameReceived( const FramePtr pFrame )
{
	if( ! SP_ISNULL( pFrame )) {

		logFrameInfos( pFrame );

		VmbFrameStatusType status;
		VmbErrorType Result;
		Result = SP_ACCESS( pFrame )->GetReceiveStatus( status );

		if( VmbErrorSuccess == Result && VmbFrameStatusComplete == status ) {

			//TODO this is specific to image format, needs to be generalized via templating
			cinder::Surface8uRef newFrame;

			VmbUint32_t frameWidth, frameHeight;
			pFrame->GetWidth( frameWidth );
			pFrame->GetHeight( frameHeight );

			switch( mColorProcessing ) {
				default:
					Result = VmbErrorBadParameter;
					std::cout << "unknown color processing parameter\n";
					break;
				case COLOR_PROCESSING_OFF:
					newFrame = std::shared_ptr<cinder::Surface8u>(
							new cinder::Surface8u( frameWidth, frameHeight, false,
							                       cinder::SurfaceChannelOrder::RGB ));
					Result = TransformImage::transform( pFrame, newFrame, "RGB24" );
					break;
				case COLOR_PROCESSING_MATRIX: {
					std::cout << "Color Transform\n";
					const VmbFloat_t Matrix[] = {0.6f, 0.3f, 0.1f,
					                             0.6f, 0.3f, 0.1f,
					                             0.6f, 0.3f, 0.1f};
					newFrame = std::shared_ptr<cinder::Surface8u>(
							new cinder::Surface8u( frameWidth, frameHeight, false,
							                       cinder::SurfaceChannelOrder::BGR ));
					Result = TransformImage::transform( pFrame, newFrame, "BGR24", Matrix );
				}
					break;

			}

			// TODO probably don't need this unless we're displaying frame info
			if( VmbErrorSuccess == Result ) {
				mFrameCallback( newFrame );
			}
		}
	}
	else {
		if( FRAME_INFO_ERRORS >= mFrameLogging ) {
			CI_LOG_E( "Error receiving frame status." );
		}
	}

	m_pCamera->QueueFrame( pFrame );
}

} // namespace civimba