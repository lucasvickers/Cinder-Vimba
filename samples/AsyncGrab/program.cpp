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

#include <string>
#include <cstring>
#include <iostream>

#include "VimbaCPP/Include/VimbaCPP.h"

#include "ApiController.h"
#include "CameraController.h"
#include "ProgramConfig.h"

int main( int argc, char* argv[] )
{
    ApiController apiController;

    VmbErrorType err = VmbErrorSuccess;

    std::cout<<"///////////////////////////////////////////\n";
    std::cout<<"///          SAO-PP AVT Accessor        ///\n";
    std::cout<<"///////////////////////////////////////////\n\n";

    //! Prse command line
    config::ProgramConfig config;
    err = config.ParseCommandline( argc, argv);
    if ( VmbErrorBadParameter == err ) {
        std::cout<< "Invalid parameters!\n\n" ;
        config.PrintHelp( std::cout );
        return err;
    }
    
    // Print out version of Vimba
    std::cout << "Vimba Version V" << apiController.getVersion() << "\n";
    
    // Startup Vimba
    apiController.startup();


    // TODO migrate to support multiple cameras
    if( config.getCameraID().empty() ) {
        AVT::VmbAPI::CameraPtrVector cameras = apiController.getCameraList();
        if( cameras.empty() ) {
            std::cout << "Unable to assign default camera, could not obtain camera list." << std::endl;
            return 1;
        }

        std::string strCameraID;
        err = cameras.front()->GetID( strCameraID );
        if( VmbErrorSuccess == err ) {
            config.setCameraID( strCameraID );
        } else {
            std::cout << "Unable to assign default camera, no cameras present." << std::endl;
        }
    }

    std::cout << "Opening camera with ID: " << config.getCameraID() << std::endl;
    CameraControllerPtr camera = apiController.getCamera( config.getCameraID(), config );

    // TODO grab images and enter control loop

    apiController.shutdown();

    return 0;
}
