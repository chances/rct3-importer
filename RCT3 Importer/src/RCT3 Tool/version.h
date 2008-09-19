#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "09";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.09;
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 18;
	static const long MINOR = 2;
	static const long BUILD = 460;
	static const long REVISION = 458;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1248;
	#define RC_FILEVERSION 18,2,460,458
	#define RC_FILEVERSION_STRING "18, 2, 460, 458\0"
	static const char FULLVERSION_STRING[] = "18.2.460.458";
	
	//SVN Version
	static const char SVN_REVISION[] = "114";
	static const char SVN_DATE[] = "2008-07-13T03:38:48.817166Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
