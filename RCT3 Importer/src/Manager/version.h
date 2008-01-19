#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "19";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.01;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 1;
	static const long BUILD = 259;
	static const long REVISION = 257;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 504;
	#define RC_FILEVERSION 0,1,259,257
	#define RC_FILEVERSION_STRING "0, 1, 259, 257\0"
	static const char FULLVERSION_STRING[] = "0.1.259.257";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_h
