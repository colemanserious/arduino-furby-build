// jenkins returned strings
const char SUCCESS[]= "SUCCESS";
const char FAILURE[]= "FAILURE";
const char UNSTABLE[]= "UNSTABLE";
const char ABORT[]="ABORTED";

enum jenkins_result_enum {
	unknown,		// not yet pulled / initialized
	success,
	failure, 
	unstable,
	aborted
};
