#Cinder-Vimba
Cinder wrapper for [Allied Vision Technology (AVT)](https://www.alliedvision.com/en/digital-industrial-camera-solutions.html) cameras using their [Vimba SDK](https://www.alliedvision.com/en/products/software.html).  Vimba is a cross platform C/C++ (and .NET) API used for accessing AVT GigE and USB cameras.

Currently developed against Vimba 1.4

##Supported Operating Systems / Installation Notes
###Linux
This is supported and tested.  To install:
* ensure that the .so objects in [_libs/linux/x64/_](https://github.com/lucasvickers/Cinder-Vimba/tree/master/libs/linux/x64) are in a shared .so directory (_/usr/local/lib_), or local to the executable.
* Ensure you install the AVT GigE Transport layer (see Vimba manuals)

###OS X
* Vimba OS X support should come in the 1.5 release, I've been told Q1 2016.

###Windows
* Not yet tested, should work once libs are figured out.

##Tested
* Single / Simultaneous GigE Mako Cameras

##Limitations
* Cinder-Vimba is currently hard coded to return either a RGB24 or BGR24 Surface.  Support for other formats not yet implemented.
* Vimba SDK runs on its own thread, and it does not seem possible to run multiple instances of the SDK on multiple threads inside the same application (feedback pending regarding this.)
