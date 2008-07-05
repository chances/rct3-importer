#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.07;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 229;
	static const long REVISION = 1247;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 878;
	#define RC_FILEVERSION 0,4,229,1247
	#define RC_FILEVERSION_STRING "0, 4, 229, 1247\0"
	static const char FULLVERSION_STRING[] = "0.4.229.1247";
	
	//SVN Version
	static const char SVN_REVISION[] = "110";
	static const char SVN_DATE[] = "2008-04-23T16:45:55.065404Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
