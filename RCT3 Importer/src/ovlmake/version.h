#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "20";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.03;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 3;
	static const long BUILD = 167;
	static const long REVISION = 901;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 608;
	#define RC_FILEVERSION 0,3,167,901
	#define RC_FILEVERSION_STRING "0, 3, 167, 901\0"
	static const char FULLVERSION_STRING[] = "0.3.167.901";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
