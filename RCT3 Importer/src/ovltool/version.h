#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.08;
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 1;
	static const long BUILD = 24;
	static const long REVISION = 132;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 38;
	#define RC_FILEVERSION 0,1,24,132
	#define RC_FILEVERSION_STRING "0, 1, 24, 132\0"
	static const char FULLVERSION_STRING[] = "0.1.24.132";
	
	//SVN Version
	static const char SVN_REVISION[] = "114";
	static const char SVN_DATE[] = "2008-07-13T03:38:48.817166Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
