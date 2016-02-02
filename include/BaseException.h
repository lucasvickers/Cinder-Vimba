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

#include <exception>
#include <string>

#include "VimbaCPP/Include/VimbaCPP.h"

namespace ciavt {

class BaseException: public std::exception
{

  private:

    std::string     mFunction;
    std::string     mMessage;
    VmbErrorType    mResult;

  public:

    BaseException( const char* const &fun, const char* const &msg, VmbErrorType result )
    : mResult ( result )
    {
        try
        {
            if( NULL != fun)
            {
                mFunction = std::string( fun );
            }
        }
        catch(...) {}
        try
        {
            if( NULL != msg)
            {
                mMessage = std::string( msg );
            }
        }
        catch(...) {}
    }

    BaseException( const char* const &fun, const std::string &msg, VmbErrorType result )
    : mResult ( result ), mMessage( msg )
    {
        try
        {
            if( NULL != fun)
            {
                mFunction = std::string( fun );
            }
        }
        catch(...) {}
    }

    ~BaseException() throw()
    {}

    const std::string& Function() const
    {
        return mFunction;
    }

    const std::string& Message() const
    {
        return mMessage;
    }
    VmbErrorType Result() const
    {
        return mResult;
    }
};

} // namespace ciavt