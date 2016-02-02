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

#include <vector>
#include <string>

#include "VmbTransform.h"

namespace ciavt {

VmbErrorType TransformImage( const AVT::VmbAPI::FramePtr & SourceFrame, std::vector<VmbUchar_t> & DestinationData, const std::string &DestinationFormat )
{
    if( SP_ISNULL( SourceFrame) )
    {
        return VmbErrorBadParameter;
    }
    VmbErrorType        Result;
    VmbPixelFormatType  InputFormat;
    VmbUint32_t         InputWidth,InputHeight;
    Result = SP_ACCESS( SourceFrame )->GetPixelFormat( InputFormat ) ;
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetWidth( InputWidth );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetHeight( InputHeight );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    // Prepare source image
    VmbImage SourceImage;
    SourceImage.Size = sizeof( SourceImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromPixelFormat( InputFormat, InputWidth, InputHeight, &SourceImage ));
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    VmbUchar_t *DataBegin = NULL;
    Result = SP_ACCESS( SourceFrame )->GetBuffer( DataBegin );
    if( VmbErrorSuccess != Result ) 
    {
        return Result;
    }
    SourceImage.Data = DataBegin;
    // Prepare destination image
    VmbImage DestinationImage;
    DestinationImage.Size = sizeof( DestinationImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromString( DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage) );
    if ( VmbErrorSuccess != Result )
    {
        return Result;
    }
    const size_t ByteCount = ( DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth* InputHeight ) / 8 ;
    DestinationData.resize( ByteCount );
    DestinationImage.Data = &*DestinationData.begin();
    // Transform data
    Result = static_cast<VmbErrorType>( VmbImageTransform( &SourceImage, &DestinationImage, NULL , 0 ));
    return Result;
}

VmbErrorType TransformImage( const AVT::VmbAPI::FramePtr & SourceFrame, std::vector<VmbUchar_t> & DestinationData, const std::string &DestinationFormat, const VmbFloat_t *Matrix )
{
    if( SP_ISNULL( SourceFrame ))
    {
        return VmbErrorBadParameter;
    }
    if ( NULL == Matrix )
    {
        return VmbErrorBadParameter;
    }
    VmbErrorType        Result;
    VmbPixelFormatType  InputFormat;
    VmbUint32_t         InputWidth,InputHeight;
    Result = SP_ACCESS( SourceFrame )->GetPixelFormat( InputFormat ) ;
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetWidth( InputWidth );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = SP_ACCESS( SourceFrame )->GetHeight( InputHeight );
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    // Prepare source image
    VmbImage SourceImage;
    SourceImage.Size = sizeof( SourceImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromPixelFormat( InputFormat, InputWidth, InputHeight, &SourceImage ));
    if( VmbErrorSuccess != Result)
    {
        return Result;
    }
    VmbUchar_t *DataBegin = NULL;
    Result = SP_ACCESS( SourceFrame )->GetBuffer( DataBegin );
    if( VmbErrorSuccess != Result ) 
    {
        return Result;
    }
    SourceImage.Data = DataBegin;
    // Prepare destination image
    VmbImage DestinationImage;
    DestinationImage.Size = sizeof( DestinationImage );
    Result = static_cast<VmbErrorType>( VmbSetImageInfoFromString( DestinationFormat.c_str(), static_cast<VmbUint32_t>(DestinationFormat.size()), InputWidth, InputHeight, &DestinationImage ));
    if ( VmbErrorSuccess != Result )
    {
        return Result;
    }
    const size_t ByteCount = ( DestinationImage.ImageInfo.PixelInfo.BitsPerPixel * InputWidth* InputHeight ) / 8 ;
    DestinationData.resize( ByteCount );
    DestinationImage.Data = &*DestinationData.begin();
    // Setup Transform parameter

    // Transform data
    VmbTransformInfo TransformInfo;
    Result = static_cast<VmbErrorType>( VmbSetColorCorrectionMatrix3x3( Matrix, &TransformInfo ));
    if( VmbErrorSuccess != Result )
    {
        return Result;
    }
    Result = static_cast<VmbErrorType>( VmbImageTransform( &SourceImage, &DestinationImage, &TransformInfo , 1 ));
    return Result;
}

} // namespace ciavt