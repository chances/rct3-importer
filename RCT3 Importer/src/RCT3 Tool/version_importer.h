#ifndef VERSION_IMPORTER_H
#define VERSION_IMPORTER_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "20";								// av::date:"%d"
	static const char MONTH[] = "01";								// av::date:"%m"
	static const char YEAR[] = "2009";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 09.01;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";							// av::var:statusshort
	
	//Standard Version Type
	static const long MAJOR = 18;									// av::var:major
	static const long MINOR = 3;									// av::var:minor
	static const long BUILD = 538;									// av::var:post
	static const long REVISION = 1382;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1263;							// Ignore as cruft
	#define RC_FILEVERSION 18,3,538,1382								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "18, 3, 538, 1382\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "18.3.538.1382";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"
	
	//SVN Version
	static const char SVN_REVISION[] = "135";						// av::var:svnrev
	static const char SVN_DATE[] = "2008-11-25T23:08:11.682931Z";	// av::var:svndate
	static const char SVN_TIME[] = "23:08:11";						// av::timevar:svndate:"%H:%M:%S"
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	
	//For parsing during distribution
	#define DIST_DIR "rct3importer-v18b3-svn135-dist"					// av::multi:major:statusshort:minor:svnrev:"rct3importer-v%1%%2%%3%-svn%4%-dist"

}
#endif //VERSION_H

























































































































































































