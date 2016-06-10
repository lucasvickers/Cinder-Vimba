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
#include "cinder/app/App.h"
#include "cinder/Log.h"

#include <sstream>
#include <iostream>
#include <chrono>

namespace civimba {
namespace featurecontainer {

using namespace AVT::VmbAPI;

// ----------------------------------------------------------------------------------------------------
// MARK: - FeatureContainer
// ----------------------------------------------------------------------------------------------------
FeatureContainer::FeatureContainer( const AVT::VmbAPI::FeaturePtr &feature )
: mFeature( feature ), mPollingTime ( 0 )
{
}

FeatureContainer::~FeatureContainer()
{
    mUpdateConnection.disconnect();
}

void FeatureContainer::setupUpdate()
{
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds > ( system_clock::now().time_since_epoch() );
    mNextPoll = ms + milliseconds( mPollingTime );

    mUpdateConnection = ci::app::App::get()->getSignalUpdate().connect( [this]() { update(); } );
}

void FeatureContainer::update()
{
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds > ( system_clock::now().time_since_epoch() );

    if( ms > mNextPoll ) {
        mNextPoll = ms + milliseconds(mPollingTime);
        updateImpl();
    }
}

// ----------------------------------------------------------------------------------------------------
// MARK: - FeatureEnum
// ----------------------------------------------------------------------------------------------------
FeatureEnum::FeatureEnum( const AVT::VmbAPI::FeaturePtr &feature )
: FeatureContainer( feature ), mCurrentIndex( 0 )
{
    // ensure compatible types
    if( VmbFeatureDataEnum != FeatureAccessor::getDataType( mFeature ) ) {
        std::stringstream ss;
        ss << "Feature " << FeatureAccessor::getName( mFeature ) << " is of type "
        << FeatureAccessor::getDataTypeString( mFeature ) << ", but expected type ENUM";
        throw FeatureContainerException( __FUNCTION__, ss.str(), VmbErrorWrongType );
    }

    mEntries = FeatureAccessor::getEnumEntries( mFeature );
    mCurrentName = FeatureAccessor::getValue<std::string>( mFeature );

    // pull the current enum index using update's logic
    updateImpl();
    //mIncrement = FeatureAccessor::hasIncrement( mFeature ) ? FeatureAccessor::getIncrement<double>( mFeature ) : 0;

    mPollingTime = FeatureAccessor::getPollingTime( mFeature );
    //std::cout << "Feature " << FeatureAccessor::getName( mFeature ) << " has polling time of " << mPollingTime << std::endl;
    if( mPollingTime ) {
        setupUpdate();
    }

}

FeatureEnum::~FeatureEnum()
{

}

void FeatureEnum::updateImpl()
{
    mCurrentName = FeatureAccessor::getValue<std::string>( mFeature );

    // there's only like 2-5 options, so no need to optimize this
    int index = 0;
    for( auto &option : mEntries ) {
        std::string name;
        option.GetName( name );
        if( name == mCurrentName ) {
            mCurrentIndex = index;
        }
        ++index;
    }
}

AVT::VmbAPI::EnumEntry FeatureEnum::getCurrentEnum()
{
    return mEntries[mCurrentIndex];
}

std::vector<std::string> FeatureEnum::getEnumsStr()
{
    std::vector<std::string> ret;
    auto entries = getEnums();
    for( auto& entry : entries ) {
        std::string name;
        entry.GetName( name );
        ret.push_back( name );
    }

    return ret;
}

void FeatureEnum::setCurrentEnumIndex( int index )
{
    if( index < mEntries.size() ) {
        mCurrentIndex = index;
        std::string name;
        mEntries[mCurrentIndex].GetName( name );
        FeatureAccessor::setStr( mFeature, name );
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

    // call updateImpl to grab values
    updateImpl();
    //mIncrement = FeatureAccessor::hasIncrement( mFeature ) ? FeatureAccessor::getIncrement<double>( mFeature ) : 0;

    mPollingTime = FeatureAccessor::getPollingTime( mFeature );
    if( mPollingTime ) {
        setupUpdate();
    }
}

FeatureDouble::~FeatureDouble()
{

}

double FeatureDouble::setValue( double val )
{
    double target = std::max( std::min( val, mMax ), mMin);
    FeatureAccessor::setValue<double>( mFeature, target );
    mValue = FeatureAccessor::getValue<double>( mFeature );

    return mValue;
}

void FeatureDouble::updateImpl()
{
    try {
        mValue = FeatureAccessor::getValue<double>( mFeature );
        mMin = FeatureAccessor::getMin<double>( mFeature );
        mMax = FeatureAccessor::getMax<double>( mFeature );
    } catch( FeatureAccessor::FeatureAccessorException& e ) {
        CI_LOG_W( "Error accessing camera features: " << e.what() );
    }
}

} // namespace featurecontainer
} // namespace civimba
