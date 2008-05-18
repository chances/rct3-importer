#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.05;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 18;
	static const long MINOR = 14;
	static const long BUILD = 311;
	static const long REVISION = 309;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 930;
	#define RC_FILEVERSION 18,14,311,309
	#define RC_FILEVERSION_STRING "18, 14, 311, 309\0"
	static const char FULLVERSION_STRING[] = "18.14.311.309";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
