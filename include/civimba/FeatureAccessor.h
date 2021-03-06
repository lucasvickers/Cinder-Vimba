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

#include "VimbaCPP/Include/VimbaCPP.h"

#include "civimba/BaseException.h"
#include "civimba/ErrorCodeToMessage.h"

#include <functional>

namespace civimba {

class FeatureAccessor {
public:

    class FeatureAccessorException : public BaseException {
    public:
        FeatureAccessorException(const char *const &fun, const char *const &msg, VmbErrorType result = VmbErrorOther)
                : BaseException(fun, msg, result) { }

        FeatureAccessorException(const char *const &fun, const std::string &msg, VmbErrorType result = VmbErrorOther)
                : BaseException(fun, msg, result) { }

        ~FeatureAccessorException() throw() { }
    };


    //! *** Get/Set value
    // Available in int64, double, string, bool, string, and some others (see VimbaCPP/include/Feature.h)
    template<typename T>
    static T getValue(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        return wrappedGetter<T>(std::bind(
                static_cast<VmbErrorType(AVT::VmbAPI::Feature::*)(T &) const>( &AVT::VmbAPI::Feature::GetValue ), featurePtr.get(), std::placeholders::_1));
    }

    template<typename T>
    static void setValue(const AVT::VmbAPI::FeaturePtr &featurePtr, const T &value)
    {
        return wrappedSetter<T>(std::bind(
                static_cast<VmbErrorType(AVT::VmbAPI::Feature::*)(const T &)>( &AVT::VmbAPI::Feature::SetValue ), featurePtr.get(), value));
    }

    // for std::string
    static void setStr( const AVT::VmbAPI::FeaturePtr &featurePtr, const std::string &value )
    {
        return wrappedSetter<const char*>(std::bind(
                static_cast<VmbErrorType(AVT::VmbAPI::Feature::*)(const char*)>( &AVT::VmbAPI::Feature::SetValue ), featurePtr.get(), value.c_str()) );
    }

    // for bool, the non referenced type
    static void setBool(const AVT::VmbAPI::FeaturePtr &featurePtr, bool value)
    {
        return wrappedSetter<bool>(std::bind(
                static_cast<VmbErrorType(AVT::VmbAPI::Feature::*)(bool)>( &AVT::VmbAPI::Feature::SetValue ), featurePtr.get(), value));
    }

    //! *** Min/Max
    // Available in double and int64
    template<typename T>
    static T getMin(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        T min, max;
        VmbErrorType res = featurePtr->GetRange(min, max);
        if (VmbErrorSuccess == res) {
            return min;
        }
        throw FeatureAccessorException(__FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage(res), res);
    }

    // Available in double and int64
    template<typename T>
    static T getMax(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        T min, max;
        VmbErrorType res = featurePtr->GetRange(min, max);
        if (VmbErrorSuccess == res) {
            return max;
        }
        throw FeatureAccessorException(__FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage(res), res);
    }

    //! *** Increments
    // Checks if increment is supported
    static bool hasIncrement(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        return wrappedGetter<bool>( std::bind( &AVT::VmbAPI::Feature::HasIncrement, featurePtr.get(), std::placeholders::_1 ) );
    }

    // Available in int64, double
    template<typename T>
    static T getIncrement(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        return wrappedGetter<T>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( T& ) const>( &AVT::VmbAPI::Feature::GetIncrement ),
                featurePtr.get(), std::placeholders::_1));
    }

    //! *** Commands
    static void runCommand(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        VmbErrorType res = featurePtr->RunCommand();
        if( VmbErrorSuccess == res ) {
            return;
        }
        throw FeatureAccessorException( __FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage( res ), res );
    }

    static bool isCommandDone(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        return wrappedGetter<bool>( std::bind( &AVT::VmbAPI::Feature::IsCommandDone, featurePtr.get(), std::placeholders::_1 ) );
    }

    //! *** Getters
    static std::string getName(const AVT::VmbAPI::FeaturePtr &featurePtr)
    {
        return wrappedGetter<std::string>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( std::string& ) const>( &AVT::VmbAPI::Feature::GetName ), featurePtr.get(), std::placeholders::_1));
    }

    static std::string getDisplayName( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<std::string>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( std::string& ) const>( &AVT::VmbAPI::Feature::GetDisplayName ), featurePtr.get(), std::placeholders::_1));
    }

    static VmbFeatureDataType getDataType( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<VmbFeatureDataType>( std::bind(&AVT::VmbAPI::Feature::GetDataType, featurePtr.get(), std::placeholders::_1));
    }

    static std::string getDataTypeString( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        auto type = wrappedGetter<VmbFeatureDataType>( std::bind(&AVT::VmbAPI::Feature::GetDataType, featurePtr.get(), std::placeholders::_1));
        return getDataTypeString( type );
    }


    static std::string getDataTypeString( const VmbFeatureDataType& type )
    {
        switch( type ) {
        case VmbFeatureDataInt:
            return "INT";
            break;
        case VmbFeatureDataFloat:
            // 64 bit floating point feature, ie a DOUBLE
            return "FLOAT64 (double)";
            break;
        case VmbFeatureDataEnum:
            return "ENUM";
            break;
        case VmbFeatureDataString:
            return "STRING";
            break;
        case VmbFeatureDataBool:
            return "BOOL";
            break;
        case VmbFeatureDataCommand:
            return "COMMAND";
            break;
        case VmbFeatureDataRaw:
            return "RAW";
            break;
        case VmbFeatureDataNone:
            return "NONE";
            break;
        case VmbFeatureDataUnknown:
            return "UNKNOWN";
            break;
        default:
            return "UNDEFINED";
            break;
        }
    }

    static std::vector<AVT::VmbAPI::EnumEntry> getEnumEntries( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<AVT::VmbAPI::EnumEntryVector>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( AVT::VmbAPI::EnumEntryVector& )>( &AVT::VmbAPI::Feature::GetEntries ), featurePtr.get(), std::placeholders::_1));
    }

    static uint32_t getPollingTime( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<uint32_t>( std::bind(&AVT::VmbAPI::Feature::GetPollingTime, featurePtr.get(), std::placeholders::_1));
    }

    static std::string getUnit( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<std::string>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( std::string& ) const>( &AVT::VmbAPI::Feature::GetUnit ), featurePtr.get(), std::placeholders::_1));
    }

    static std::string getToolTip( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<std::string>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( std::string& ) const>( &AVT::VmbAPI::Feature::GetToolTip ), featurePtr.get(), std::placeholders::_1));
    }

    static std::string getDescription( const AVT::VmbAPI::FeaturePtr& featurePtr )
    {
        return wrappedGetter<std::string>(std::bind(
                static_cast<VmbErrorType( AVT::VmbAPI::Feature::* )( std::string& ) const>( &AVT::VmbAPI::Feature::GetDescription ), featurePtr.get(), std::placeholders::_1));
    }

protected:

    template <typename T>
    static T wrappedGetter( std::function<VmbErrorType(T&)> func )
    {
        T val;
        VmbErrorType res = func( val );
        if( VmbErrorSuccess == res ) {
            return val;
        }
        throw FeatureAccessorException( __FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage( res ), res );
    }

    template <typename T>
    static void wrappedSetter( std::function<VmbErrorType()> func )
    {
        VmbErrorType res = func();
        if( VmbErrorSuccess == res ) {
            return;
        }
        throw FeatureAccessorException( __FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage( res ), res );
    }

};

} // namespace civimba