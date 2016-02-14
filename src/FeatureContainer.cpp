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

#include "civimba/FeatureContainer.h"
#include "civimba/FeatureAccessor.h"

#include <sstream>

namespace civimba {
namespace featurecontainer {

using namespace AVT::VmbAPI;

FeatureDouble::FeatureDouble( const AVT::VmbAPI::FeaturePtr& feature )
: mFeature( feature )
{
    // ensure compatible types
    if( VmbFeatureDataFloat != FeatureAccessor::getDataType( mFeature ) ) {
        std::stringstream ss;
        ss << "Feature " << FeatureAccessor::getName( mFeature ) << " is of type "
           << FeatureAccessor::getDataTypeString( mFeature ) << ", but expected type FLOAT64 (i.e. double)";
        throw CameraControllerException( __FUNCTION__, ss.str(), VmbErrorWrongType );
    }

    // TODO wtf there's no float accessor, whats this shit?
    mValue = FeatureAccessor::getValue<double>( mFeature );

    mMin = FeatureAccessor::getMin<double>( mFeature );
    mMax = FeatureAccessor::getMax<double>( mFeature );

    //mIncrement = FeatureAccessor::hasIncrement( mFeature ) ? FeatureAccessor::getIncrement<double>( mFeature ) : 0;

    mPollingTime = FeatureAccessor::getPollingTime( mFeature );

    // set up callback
}

FeatureDouble::~FeatureDouble()
{

}

void FeatureDouble::update()
{

}

} // namespace featurecontainer
} // namespace civimba
