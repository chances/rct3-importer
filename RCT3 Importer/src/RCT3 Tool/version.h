#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.10;
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 18;
	static const long MINOR = 2;
	static const long BUILD = 472;
	static const long REVISION = 470;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1263;
	#define RC_FILEVERSION 18,2,472,470
	#define RC_FILEVERSION_STRING "18, 2, 472, 470\0"
	static const char FULLVERSION_STRING[] = "18.2.472.470";
	
	//SVN Version
	static const char SVN_REVISION[] = "119";
	static const char SVN_DATE[] = "2008-08-12T20:21:04.566410Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
