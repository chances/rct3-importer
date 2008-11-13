#ifndef VERSION_IMPORTER_H
#define VERSION_IMPORTER_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "13";								// av::date:"%d"
	static const char MONTH[] = "11";								// av::date:"%m"
	static const char YEAR[] = "2008";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 08.11;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";							// av::var:statusshort
	
	//Standard Version Type
	static const long MAJOR = 18;									// av::var:major
	static const long MINOR = 2;									// av::var:minor
	static const long BUILD = 520;									// av::var:post
	static const long REVISION = 1358;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1263;							// Ignore as cruft
	#define RC_FILEVERSION 18,2,520,1358								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "18, 2, 520, 1358\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "18.2.520.1358";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"
	
	//SVN Version
	static const char SVN_REVISION[] = "126";						// av::var:svnrev
	static const char SVN_DATE[] = "2008-10-29T17:48:54.856096Z";	// av::var:svndate
	static const char SVN_TIME[] = "17:48:54";						// av::timevar:svndate:"%H:%M:%S"
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	
	//For parsing during distribution
	#define DIST_DIR "rct3importer-v18b2-svn126-dist"					// av::multi:major:statusshort:minor:svnrev:"rct3importer-v%1%%2%%3%-svn%4%-dist"

}
#endif //VERSION_H















































































































































