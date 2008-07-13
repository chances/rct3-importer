#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2008";
	static const double UBUNTU_VERSION_STYLE = 8.07;
	
	//Software Status
	static const char STATUS[] = "Release";
	static const char STATUS_SHORT[] = "r";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 237;
	static const long REVISION = 1288;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 911;
	#define RC_FILEVERSION 0,4,237,1288
	#define RC_FILEVERSION_STRING "0, 4, 237, 1288\0"
	static const char FULLVERSION_STRING[] = "0.4.237.1288";
	
	//SVN Version
	static const char SVN_REVISION[] = "114";
	static const char SVN_DATE[] = "2008-07-13T03:38:48.817166Z";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
