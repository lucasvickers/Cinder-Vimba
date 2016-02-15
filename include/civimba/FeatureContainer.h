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
#include "civimba/FeatureAccessor.h"

#include <memory>
#include <chrono>

#include "cinder/Signals.h"

namespace civimba {
namespace featurecontainer {

typedef std::shared_ptr<class FeatureDouble> FeatureDoubleRef;
typedef std::shared_ptr<class FeatureEnum> FeatureEnumRef;


template<typename FeatureT, typename... Args>
std::shared_ptr<FeatureT> createContainer(Args &&... args) {
    return std::shared_ptr<FeatureT>(new FeatureT(std::forward<Args>(args) ...));
}

class FeatureContainerException : public BaseException {
public:
    FeatureContainerException(const char *const &fun, const char *const &msg, VmbErrorType result = VmbErrorOther)
            : BaseException(fun, msg, result) { }

    FeatureContainerException(const char *const &fun, const std::string &msg, VmbErrorType result = VmbErrorOther)
            : BaseException(fun, msg, result) { }

    ~FeatureContainerException() throw() { }
};

class FeatureContainer : private cinder::Noncopyable
{
  public:

    FeatureContainer( const AVT::VmbAPI::FeaturePtr &feature );
    virtual ~FeatureContainer();

    AVT::VmbAPI::FeaturePtr getFeature() { return mFeature; }

    // TODO fix this
    //double getIncrement() { return mIncrement; }

protected:

    // TODO fix this
    //double                    mIncrement;
    AVT::VmbAPI::FeaturePtr     mFeature;
    uint32_t                    mPollingTime;
    cinder::signals::Connection mUpdateConnection;

    std::chrono::milliseconds   mNextPoll;

    void setupUpdate();
    void update();

    virtual void updateImpl() = 0;

};

class FeatureEnum : public FeatureContainer, public std::enable_shared_from_this<FeatureEnum>
{
  public:

    FeatureEnum( const AVT::VmbAPI::FeaturePtr &feature );
    ~FeatureEnum();

    std::vector<AVT::VmbAPI::EnumEntry> getEnums() { return mEntries; }
    std::vector<std::string> getEnumsStr();
    AVT::VmbAPI::EnumEntry getCurrentEnum();

    int getCurrentEnumIndex() { return mCurrentIndex; }

    void setCurrentEnumIndex( int index );

  protected:

    std::vector<AVT::VmbAPI::EnumEntry> mEntries;
    int mCurrentIndex;
    std::string mCurrentName;

    void updateImpl() override;
};

class FeatureDouble : public FeatureContainer, public std::enable_shared_from_this<FeatureDouble>
{
  public:

    FeatureDouble( const AVT::VmbAPI::FeaturePtr &feature );
    ~FeatureDouble();

    double getValue() { return mValue; }

    double getMin() { return mMin; }

    double getMax() { return mMax; }

    // set value, return what it's set to
    double setValue( double val );

  protected:

    double mValue;
    double mMin;
    double mMax;

    void updateImpl() override;
};

} // namespace featurecontainer
} // namespace civimba