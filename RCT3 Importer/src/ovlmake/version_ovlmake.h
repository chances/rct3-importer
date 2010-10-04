#ifndef VERSION_OVLMAKE_H
#define VERSION_OVLMAKE_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "04";								// av::date:"%d"
	static const char MONTH[] = "10";								// av::date:"%m"
	static const char YEAR[] = "2010";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 10.10;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Release";
	static const char STATUS_SHORT[] = "r";
	
	//Standard Version Type
	static const long MAJOR = 0;									// av::var:major
	static const long MINOR = 5;									// av::var:minor
	static const long BUILD = 352;									// av::var:post
	static const long REVISION = 1095;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 975;							// Ignore as cruft
	#define RC_FILEVERSION 0,5,352,1095								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "0, 5, 352, 1095\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "0.5.352.1095";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"
	
	//SVN Version
	static const char SVN_REVISION[] = "140";						// av::var:svnrev
	static const char SVN_DATE[] = "2009-03-22T16:56:52.674946Z";	// av::var:svndate
	static const char SVN_TIME[] = "16:56:52";						// av::timevar:svndate:"%H:%M:%S"
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	
	//For parsing during distribution
	#define DIST_DIR "ovlmake-0.5-svn140"									// av::multi:major:minor:svnrev:"ovlmake-%1%.%2%-svn%3%"

}
#endif //VERSION_H





























































































































































































































