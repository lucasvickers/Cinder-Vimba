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

namespace civimba {
namespace featurecontainer {

/*
// TODO perhaps derive from this?
class FeatureContainer {

};
*/

typedef std::shared_ptr<class FeatureDouble> FeatureDoubleRef;

template<typename FeatureT, typename... Args>
std::shared_ptr<FeatureT> createContainer(Args &&... args) {
    return std::make_shared<FeatureT>(FeatureT(std::forward<Args>(args) ...));
}

class CameraControllerException : public BaseException {
public:
    CameraControllerException(const char *const &fun, const char *const &msg, VmbErrorType result = VmbErrorOther)
            : BaseException(fun, msg, result) { }

    CameraControllerException(const char *const &fun, const std::string &msg, VmbErrorType result = VmbErrorOther)
            : BaseException(fun, msg, result) { }

    ~CameraControllerException() throw() { }
};

class FeatureEnum {
public:

protected:
    std::vector<AVT::VmbAPI::EnumEntry> mEntries;
    AVT::VmbAPI::EnumEntry mValue;
};

class FeatureDouble {
public:

    FeatureDouble(const AVT::VmbAPI::FeaturePtr &feature);

    ~FeatureDouble();

    AVT::VmbAPI::FeaturePtr getFeature() { return mFeature; }

    double getValue() { return mValue; }

    double getMin() { return mMin; }

    double getMax() { return mMax; }

    double getIncrement() { return mIncrement; }

    // set value, return what it's set to
    double setValue( double val );

protected:

    double mValue;
    double mMin;
    double mMax;
    double mIncrement;

    AVT::VmbAPI::FeaturePtr mFeature;

    uint32_t mPollingTime;

    void update();
};

} // namespace featurecontainer
} // namespace civimba