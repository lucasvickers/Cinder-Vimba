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
#include "VimbaCPP/Include/VimbaCPP.h"
#include "cinder/params/Params.h"

#include "civimba/CiVimba.h"

namespace civimba {

class ParamFeature {
  public:

    class ParamFeatureException: public BaseException
    {
    public:
        ParamFeatureException( const char* const &fun, const char* const &msg, VmbErrorType result = VmbErrorOther )
                : BaseException( fun, msg, result )
        { }
        ParamFeatureException( const char* const &fun, const std::string &msg, VmbErrorType result = VmbErrorOther )
                : BaseException( fun, msg, result )
        { }
        ~ParamFeatureException() throw()
        { }
    };

    void loadFeature( const char *name, const CameraControllerRef& cam ) {
        std::string stdString( name );
        loadFeature( stdString, cam );
    }

    void loadFeature( const std::string &name, const CameraControllerRef& cam )
    {
        mFeatures.push_back( cam->getFeatureByName( name ) );
    }

    void refreshData()
    {
        // TODO load data off camera to refresh visual
        // TODO make auto callbacks based on GetPollingTime
    }

    void populateParam( cinder::params::InterfaceGlRef interfaceGl )
    {
        for( auto &feature : mFeatures ) {
            VmbFeatureDataType type;
            VmbErrorType err = feature->GetDataType( type );
            if( VmbErrorSuccess != err ) {
                throw ParamFeatureException( __FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage( err ), err );
            }

            std::string name = getName( feature );
            err = feature->GetDataType( type );
            if( VmbErrorSuccess != err ) {
                throw ParamFeatureException( __FUNCTION__, AVT::VmbAPI::ErrorCodeToMessage( err ), err );
            }


            switch( type ) {
                case VmbFeatureDataInt:
                    std::string unit = GetUnit( feature );
                    
                    // add int
                    // need min, max, increment
                    // check that 64bit is not needed (get Unit)
                    break;
                case VmbFeatureDataFloat:
                    // add double
                    // need min, max, increment
                    break;
                case VmbFeatureDataEnum:
                    // add vector<str>
                    //
                    break;
                case VmbFeatureDataString:
                    // add string param
                    break;
                case VmbFeatureDataBool:
                    // add bool object
                    break;
                case VmbFeatureDataCommand:
                    // add button
                    break;
                case VmbFeatureDataRaw:
                    // can't auto support this
                    break;
                case VmbFeatureDataNone:
                    // nothing to do
                    CI_LOG_W( "Feature " << name << " has no associated data." );
                    break;
                case VmbFeatureDataUnknown:
                    stringstream ss;
                    ss << "Feature " << name << " has unknown data type.";
                    CI_LOG_E( ss.str() );
                    throw ParamFeatureException( __FUNCTION__, ss.str(), err );
                    break;
                default:
                    throw ParamFeatureException( __FUNCTION__, ss.str(), err );
                    break;
            }
        }
    }

  protected:

    std::vector<AVT::VmbAPI::FeaturePtr> mFeatures;

    // needed?
    cinder::params::InterfaceGlRef mInterfaceGlRef;
};

} // namespace civimba