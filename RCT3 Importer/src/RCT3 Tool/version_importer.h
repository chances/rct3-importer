#ifndef VERSION_IMPORTER_H
#define VERSION_IMPORTER_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "24";								// av::date:"%d"
	static const char MONTH[] = "10";								// av::date:"%m"
	static const char YEAR[] = "2008";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 08.10;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 18;									// av::var:major
	static const long MINOR = 2;									// av::var:minor
	static const long BUILD = 485;									// av::var:post
	static const long REVISION = 1297;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 1263;							// Ignore as cruft
	#define RC_FILEVERSION 18,2,485,1297								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "18, 2, 485, 1297\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "18.2.485.1297";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"
	
	//SVN Version
	static const char SVN_REVISION[] = "124";						// av::var:svnrev
	static const char SVN_DATE[] = "2008-10-10T00:50:37.294669Z";	// av::var:svndate
	static const char SVN_TIME[] = "00:50:37";						// av::timevar:svndate:"%H:%M:%S"
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	

}
#endif //VERSION_H















































