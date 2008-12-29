#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "29";								// av::date:"%d"
	static const char MONTH[] = "11";								// av::date:"%m"
	static const char YEAR[] = "2008";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 08.11;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";							// av::var:statusshort
	
	//Standard Version Type
	static const long MAJOR = 0;									// av::var:major
	static const long MINOR = 1;									// av::var:minor
	static const long BUILD = 10;									// av::var:post
	static const long REVISION = 24;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1;							// Ignore as cruft
	#define RC_FILEVERSION 0,1,10,24								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "0, 1, 10, 24\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "0.1.10.24";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"

	//SVN Version
	static const char SVN_REVISION[] = "135";						// av::var:svnrev
	static const char SVN_DATE[] = "2008-11-25T23:08:11.682931Z";	// av::var:svndate
	static const char SVN_TIME[] = "23:08:11";						// av::timevar:svndate:"%H:%M:%S"
	

	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	
	//For parsing during distribution
	#define DIST_DIR "rct3injector-0.1-svn135"									// av::multi:major:minor:svnrev:"rct3injector-%1%.%2%-svn%3%"

}
#endif //VERSION_H



















































