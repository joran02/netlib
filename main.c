





//#include <iostream>

#include    "upload.h"






int main()
{
	//int ret = upload_file_by_http("","");
	//return ret;
	int ret = upload_file_by_ftp("192.168.3.28",21,"ftpusr","root","123.txt");
	return ret;
}




