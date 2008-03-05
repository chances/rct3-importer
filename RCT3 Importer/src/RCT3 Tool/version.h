#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.02;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 18;
	static const long MINOR = 12;
	static const long BUILD = 61;
	static const long REVISION = 59;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 203;
	#define RC_FILEVERSION 18,12,61,59
	#define RC_FILEVERSION_STRING "18, 12, 61, 59\0"
	static const char FULLVERSION_STRING[] = "18.12.61.59";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
