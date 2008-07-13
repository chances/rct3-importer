#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.07;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 1;
	static const long MINOR = 0;
	static const long BUILD = 258;
	static const long REVISION = 258;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 776;
	#define RC_FILEVERSION 1,0,258,258
	#define RC_FILEVERSION_STRING "1, 0, 258, 258\0"
	static const char FULLVERSION_STRING[] = "1.0.258.258";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
