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
#include <time.h>

#include "civimba/FrameObserver.h"
#include "civimba/TransformImage.h"
#include "civimba/Types.h"

// TODO bring this file to my standards w/ formatting

namespace civimba {

using namespace AVT::VmbAPI;

FrameObserver::FrameObserver( CameraPtr camera, FrameInfo frameInfo, ColorProcessing colorProcessing )
    :   IFrameObserver( camera ),   
        mFrameInfos( frameInfo ),   
        mColorProcessing( colorProcessing )
{ }

double FrameObserver::GetTime()
{
    double dTime = 0.0;

    struct timespec now;
    clock_gettime( CLOCK_REALTIME, &now );
    dTime = ( (double)now.tv_sec ) + ( (double)now.tv_nsec ) / 1000000000.0;

    return dTime;
}

void PrintFrameInfo( const FramePtr &pFrame )
{
    std::cout<<" Size:";
    VmbUint32_t     nWidth = 0;
    VmbErrorType    res;
    res = pFrame->GetWidth(nWidth);
    if( VmbErrorSuccess == res ) {
        std::cout << nWidth;
    } else {
        std::cout << "?";
    }

    std::cout << "x";
    VmbUint32_t nHeight = 0;
    res = pFrame->GetHeight(nHeight);
    if( VmbErrorSuccess == res ) {
        std::cout<< nHeight;
    } else {
        std::cout << "?";
    }

    std::cout<<" Format:";
    VmbPixelFormatType ePixelFormat = VmbPixelFormatMono8;
    res = pFrame->GetPixelFormat( ePixelFormat );
    if( VmbErrorSuccess == res ) {
        std::cout << "0x" << std::hex << ePixelFormat << std::dec;
    } else {
        std::cout << "?";
    }
}

void PrintFrameStatus( VmbFrameStatusType eFrameStatus )
{
    switch( eFrameStatus ) {
    case VmbFrameStatusComplete:
        std::cout << "Complete";
        break;

    case VmbFrameStatusIncomplete:
        std::cout << "Incomplete";
        break;

    case VmbFrameStatusTooSmall:
        std::cout << "Too small";
        break;

    case VmbFrameStatusInvalid:
        std::cout << "Invalid";
        break;

    default:
        std::cout << "unknown frame status";
        break;
    }
}

void FrameObserver::ShowFrameInfos( const FramePtr &pFrame ) 
{
    bool                bShowFrameInfos     = false;
    VmbUint64_t         nFrameID            = 0;
    bool                bFrameIDValid       = false;
    VmbFrameStatusType  eFrameStatus        = VmbFrameStatusComplete;
    bool                bFrameStatusValid   = false;
    VmbErrorType        res                 = VmbErrorSuccess;
    double              dFPS                = 0.0;
    bool                bFPSValid           = false;
    VmbUint64_t         nFramesMissing      = 0;

    if( FRAME_INFO_SHOW == mFrameInfos ) {
        bShowFrameInfos = true;
    }

    res = pFrame->GetFrameID( nFrameID );
    if( VmbErrorSuccess == res ) {
        bFrameIDValid = true;

        if( mFrameID.IsValid() )
        {
            if( nFrameID != ( mFrameID() + 1 ) )
            {
                nFramesMissing = nFrameID - mFrameID() - 1;
                if( 1 == nFramesMissing ) {
                    std::cout << "1 missing frame detected\n";
                } else {
                    std::cout << nFramesMissing << "missing frames detected\n";
                }
            }
        }

        mFrameID( nFrameID );
        double dFrameTime = GetTime();
        if( ( mFrameTime.IsValid() ) && ( 0 == nFramesMissing ) )
        {
            double dTimeDiff = dFrameTime - mFrameTime();
            if( dTimeDiff > 0.0 )
            {
                dFPS = 1.0 / dTimeDiff;
                bFPSValid = true;
            } 
            else {
                bShowFrameInfos = true;
            }
        }

        mFrameTime( dFrameTime );
    }
    else
    {
        bShowFrameInfos = true;
        mFrameID.Invalidate();
        mFrameTime.Invalidate();
    }

    res = pFrame->GetReceiveStatus( eFrameStatus );
    if( VmbErrorSuccess == res )
    {
        bFrameStatusValid = true;

        if( VmbFrameStatusComplete != eFrameStatus )
        {
            bShowFrameInfos = true;
        }
    }
    else
    {
        bShowFrameInfos = true;
    }

    if( bShowFrameInfos ) {
        std::cout << "Camera ID:" << mCameraID;
        std::cout << " Frame ID:";

        bFrameIDValid ? std::cout << nFrameID : std::cout << "?";

        std::cout << " Status:";
        if( bFrameStatusValid ) {
            PrintFrameStatus( eFrameStatus);
        } else {
            std::cout << "?";
        }

        PrintFrameInfo( pFrame );
        
        std::cout << " FPS:";
        if( bFPSValid )
        {
            std::streamsize s = std::cout.precision();
            std::cout<<std::fixed<<std::setprecision(2)<<dFPS<<std::setprecision(s);
        }
        else 
        {
            std::cout << "?";
        }

        std::cout << "\n";
    }
}

void FrameObserver::FrameReceived( const FramePtr pFrame )
{
    if(! SP_ISNULL( pFrame ) )
    {
        if( FRAME_INFO_OFF != mFrameInfos )
        {
            ShowFrameInfos( pFrame);
        }

        VmbFrameStatusType status;
        VmbErrorType Result;
        Result = SP_ACCESS( pFrame)->GetReceiveStatus( status);

        if( VmbErrorSuccess == Result && VmbFrameStatusComplete == status)
        {
            // could move this to a dedicated thread if it got heavy but hasn't seemed to be a problem yet
            std::vector<VmbUchar_t> TransformedData;
            switch( mColorProcessing )
            {
            default:
                Result = VmbErrorBadParameter;
                std::cout << "unknown color processing parameter\n";
                break;
            case COLOR_PROCESSING_OFF:
                Result = TransformImage( pFrame, TransformedData, "RGB24" );
                break;
            case COLOR_PROCESSING_MATRIX:
                {
                    std::cout << "Color Transform\n";
                    const VmbFloat_t Matrix[] = {   0.6f, 0.3f, 0.1f, 
                                                    0.6f, 0.3f, 0.1f, 
                                                    0.6f, 0.3f, 0.1f};
                    Result = TransformImage( pFrame, TransformedData,"BGR24", Matrix );
                }
                break;

            }

            // TODO probably don't need this unless we're displaying frame info
            if( VmbErrorSuccess == Result && TransformedData.size() >= 3 )
            {
                char old_fill_char = std::cout.fill('0');
                std::cout << std::hex <<"R = 0x"<<std::setw(2)<<(int)TransformedData[0]<<" "
                                      <<"G = 0x"<<std::setw(2)<<(int)TransformedData[1]<<" "
                                      <<"B = 0x"<<std::setw(2)<<(int)TransformedData[2]<<std::dec<<"\n";
                std::cout.fill( old_fill_char );
            }
            else
            {
                std::cout << "Transformation failed.\n";
            }
        }
        else
        {
            std::cout << "Frame incomplete.\n";
        }
    }
    else
    {
        std::cout <<"Frame pointer NULL.\n";
    }

    m_pCamera->QueueFrame( pFrame );
}

} // namespace civimba