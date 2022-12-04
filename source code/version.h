#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2022";
	static const char UBUNTU_VERSION_STYLE[] =  "22.12";
	
	//Software Status
	static const char STATUS[] =  "Release";
	static const char STATUS_SHORT[] =  "r";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 5;
	static const long BUILD  = 2;
	static const long REVISION  = 17;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1209;
	#define RC_FILEVERSION 2,5,2,17
	#define RC_FILEVERSION_STRING "2, 5, 2, 17\0"
	static const char FULLVERSION_STRING [] = "2.5.2.17";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 4;
	

}
#endif //VERSION_H
