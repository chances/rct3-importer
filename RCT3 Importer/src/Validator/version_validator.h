#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	// av::set:ext:parameter:"../../.."
	// av::set:ext:command:'svn info --xml "%s" | xmllint -'
	
	//Date Version Types
	static const char DATE[] = "20";								// av::date:"%d"
	static const char MONTH[] = "11";								// av::date:"%m"
	static const char YEAR[] = "2008";								// av::date:"%Y"
	static const double UBUNTU_VERSION_STYLE = 08.11;				// av::date:"%y.%m"
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";							// av::var:statusshort
	
	//Standard Version Type
	static const long MAJOR = 1;									// av::var:major
	static const long MINOR = 0;									// av::var:minor
	static const long BUILD = 292;									// av::var:post
	static const long REVISION = 839;								// av::var:pre
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 828;							// Ignore as cruft
	#define RC_FILEVERSION 1,0,292,839								// av::multi:major:minor:post:pre:"%1%,%2%,%3%,%4%"
	#define RC_FILEVERSION_STRING "1, 0, 292, 839\0"				// av::multi:major:minor:post:pre:"%1%, %2%, %3%, %4%\0"
	static const char FULLVERSION_STRING[] = "1.0.292.839";		// av::multi:major:minor:post:pre:"%1%.%2%.%3%.%4%"
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;							// Ignore as cruft
	

}
#endif //VERSION_H

















