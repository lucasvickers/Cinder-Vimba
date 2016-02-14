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
#include <iostream>

namespace civimba {
namespace featurecontainer {

using namespace AVT::VmbAPI;

// ----------------------------------------------------------------------------------------------------
// MARK: - FeatureContainer
// ----------------------------------------------------------------------------------------------------
FeatureContainer::FeatureContainer( const AVT::VmbAPI::FeaturePtr &feature )
{
    // TODO possibly set up the update routine?
}

FeatureContainer::~FeatureContainer()
{
    // TODO unregister update
}

// ----------------------------------------------------------------------------------------------------
// MARK: - FeatureEnum
// ----------------------------------------------------------------------------------------------------
FeatureEnum::FeatureEnum( const AVT::VmbAPI::FeaturePtr &feature )
: FeatureContainer( feature ), mCurrent( 0 )
{
    // ensure compatible types
    if( VmbFeatureDataEnum != FeatureAccessor::getDataType( mFeature ) ) {
        std::stringstream ss;
        ss << "Feature " << FeatureAccessor::getName( mFeature ) << " is of type "
        << FeatureAccessor::getDataTypeString( mFeature ) << ", but expected type ENUM";
        throw FeatureContainerException( __FUNCTION__, ss.str(), VmbErrorWrongType );
    }

    mEntries = FeatureAccessor::getEnumEntries( mFeature );
    mCurrent = FeatureAccessor::getValue<long long>( mFeature );

    //mIncrement = FeatureAccessor::hasIncrement( mFeature ) ? FeatureAccessor::getIncrement<double>( mFeature ) : 0;

    mPollingTime = FeatureAccessor::getPollingTime( mFeature );
    std::cout << "Feature " << FeatureAccessor::getName( mFeature ) << " of type double has polling of " << mPollingTime << std::endl;

    // set up callback

}

FeatureEnum::~FeatureEnum()
{

}

AVT::VmbAPI::EnumEntry FeatureEnum::getCurrentEnum()
{
    return mEntries[mCurrent];
}

void FeatureEnum::setCurrentEnum( int index )
{
    if( index < mEntries.size() ) {
        mCurrent = index;
    } else {
        throw FeatureContainerException( __FUNCTION__, "Attempting to set enum to out of bounds index", VmbErrorWrongType );
    }
}


// ----------------------------------------------------------------------------------------------------
// MARK: - FeatureDouble
// ----------------------------------------------------------------------------------------------------
FeatureDouble::FeatureDouble( const AVT::VmbAPI::FeaturePtr& feature )
: FeatureContainer( feature )
{
    // ensure compatible types
    if( VmbFeatureDataFloat != FeatureAccessor::getDataType( mFeature ) ) {
        std::stringstream ss;
        ss << "Feature " << FeatureAccessor::getName( mFeature ) << " is of type "
           << FeatureAccessor::getDataTypeString( mFeature ) << ", but expected type FLOAT64 (i.e. double)";
        throw FeatureContainerException( __FUNCTION__, ss.str(), VmbErrorWrongType );
    }

    mValue = FeatureAccessor::getValue<double>( mFeature );

    mMin = FeatureAccessor::getMin<double>( mFeature );
    mMax = FeatureAccessor::getMax<double>( mFeature );

    //mIncrement = FeatureAccessor::hasIncrement( mFeature ) ? FeatureAccessor::getIncrement<double>( mFeature ) : 0;

    mPollingTime = FeatureAccessor::getPollingTime( mFeature );
    std::cout << "Feature " << FeatureAccessor::getName( mFeature ) << " of type double has polling of " << mPollingTime << std::endl;

    // set up callback
}

FeatureDouble::~FeatureDouble()
{
    // TODO remove callback?
}

double FeatureDouble::setValue( double val )
{

}

void FeatureDouble::update()
{
    // TODO do this
}

} // namespace featurecontainer
} // namespace civimba
