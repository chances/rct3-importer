#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "17";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.04;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 192;
	static const long REVISION = 1057;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 736;
	#define RC_FILEVERSION 0,3,192,1057
	#define RC_FILEVERSION_STRING "0, 3, 192, 1057\0"
	static const char FULLVERSION_STRING[] = "0.3.192.1057";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
