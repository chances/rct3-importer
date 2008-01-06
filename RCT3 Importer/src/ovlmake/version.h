#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.01;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 6;
	static const long BUILD = 7;
	static const long REVISION = 19;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 7;
	#define RC_FILEVERSION 1,6,7,19
	#define RC_FILEVERSION_STRING "1, 6, 7, 19\0"
	static const char FULLVERSION_STRING[] = "1.6.7.19";
	
	//SVN Version
	static const char SVN_REVISION[] = "0";
	static const char SVN_DATE[] = "unknown date";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
