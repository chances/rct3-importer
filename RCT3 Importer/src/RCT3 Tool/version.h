#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.02;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 18;
	static const long MINOR = 12;
	static const long BUILD = 47;
	static const long REVISION = 45;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 172;
	#define RC_FILEVERSION 18,12,47,45
	#define RC_FILEVERSION_STRING "18, 12, 47, 45\0"
	static const char FULLVERSION_STRING[] = "18.12.47.45";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
