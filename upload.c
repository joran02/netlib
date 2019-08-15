



#include    "upload.h"
#include <stdio.h>
#include <string.h>
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


#define TRUE   		1			/* Boolean constants */
#define FALSE 		0

#ifdef __cplusplus
extern "C" {
#endif

#include <curl/curl.h>

#ifdef __cplusplus
  }
#endif

size_t read_file(void* buff, size_t size, size_t nmemb, void* userp)
{
	size_t sizes = fread(buff, size, nmemb, (FILE *)userp);
	return sizes;
}


int upload_file_by_http(const char * url,const char * file_path)
{
	CURLcode res;	//easy_handle定义的一些错误码
 
		//初始化libcurl
		res = curl_global_init(CURL_GLOBAL_ALL);
		if (res != CURLE_OK)
		{
			//std::cerr << "init libcurl failed." << std::endl;
			return -1;
		}
 
		//获取要上传的文件指针	
		FILE* r_file = fopen(file_path, "rb");
		if (0 == r_file)
		{
			//std::cerr << "the file /" " << file_name << " /"that you will read is not exist" << std::endl;
			return -1;
		}
 
		CURL* easy_handle;
 
		easy_handle = curl_easy_init();
		if (0 == easy_handle)
		{
			//std::cout << "get a easy_handle handle fail!" << std::endl;
			fclose(r_file);
			curl_global_cleanup();
			curl_easy_cleanup(easy_handle);
			return -1;
		}
 
		// 获取文件大小
		fseek(r_file, 0, 2);	
		int file_size = ftell(r_file);
		rewind(r_file);
/*
		if (strncmp(args->url, "https://", 8) == 0)
	{
		#if 1	
		// 方法1, 设定为不验证证书和HOST
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#else
		// 方法2, 设定一个SSL判别证书
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(curl,CURLOPT_CAINFO,"ca-cert.pem"); 	// TODO: 设置一个证书文件
		#endif 
	}*/
		
		curl_easy_setopt(easy_handle, CURLOPT_URL, url);		//获取URL地址
		curl_easy_setopt(easy_handle, CURLOPT_UPLOAD, TRUE);	//告诉easy_handle是做上传操作
		curl_easy_setopt(easy_handle, CURLOPT_READFUNCTION, &read_file);	//调用重写的读文件流函数
		curl_easy_setopt(easy_handle, CURLOPT_READDATA, r_file);	//往read_file()函数中传入用户自定义的数据类型
		curl_easy_setopt(easy_handle, CURLOPT_INFILE, r_file);		//定位作为上传的输入文件
		curl_easy_setopt(easy_handle, CURLOPT_INFILESIZE, file_size);	//上传的字节数
 
		//执行设置好的操作
		res = curl_easy_perform(easy_handle);
 
		//获取HTTP错误码
		int HTTP_flag = 0;
		curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE , &HTTP_flag);
 
		//一般清理动作
		fclose(r_file);
		curl_global_cleanup();
		curl_easy_cleanup(easy_handle);
 
		//std::cout << "操作本地文件:" << file_name << "; url:" << url << ".";
		//检测HTTP错误码和执行操作结果
		if (/*(0 != check_error(HTTP_flag)) || */(CURLE_OK != res))
		{
			//std::cout << "失败!" << std::endl;
			return -1;
		}
		else
		{
			//std::cout << "成功!" << std::endl;
			return 0;
		}
	return 0;
}
int upload_file_by_https(const char * url,const char * file_path)
{
	return 0;
}


#define LOCAL_FILE      "/tmp/uploadthis.txt"
#define UPLOAD_FILE_AS  "while-uploading.txt"
#define REMOTE_URL      "ftp://192.168.3.28//home/ftp/pub/"  UPLOAD_FILE_AS
#define RENAME_FILE_TO  "renamed-and-fine.txt"


static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  curl_off_t nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */ 
  size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);
 
  nread = (curl_off_t)retcode;
 
  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
          " bytes from file\n", nread);
  return retcode;
}


int upload_file_by_ftp(const char * sever,int port,const char *account,const char *password,const char * file_path)
{
	CURL *curl;
	CURLcode res;
	FILE *hd_src;
	struct stat file_info;
	curl_off_t fsize;

	struct curl_slist *headerlist = NULL;
	static const char buf_1 [] = "RNFR " UPLOAD_FILE_AS;
	static const char buf_2 [] = "RNTO " RENAME_FILE_TO;

	/* get the file size of the local file */ 
	if(stat(file_path, &file_info)) {
	printf("Couldn't open '%s': %s\n", file_path, strerror(errno));
	return -1;
	}
	fsize = (curl_off_t)file_info.st_size;

	printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

	/* get a FILE * of the same file */ 
	hd_src = fopen(file_path, "rb");

	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* build a list of commands to pass to libcurl */ 
		headerlist = curl_slist_append(headerlist, buf_1);
		headerlist = curl_slist_append(headerlist, buf_2);

		/* we want to use our own read function */ 
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

		/* enable uploading */ 
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* specify target */ 
		curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);
		printf("CURLOPT_URL = %s\n",REMOTE_URL);

		/* pass in that last of FTP commands to run after the transfer */ 
		//curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

		/* now specify which file to upload */ 
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		char usrpasswd[100]={0};
		sprintf(usrpasswd, "%s:%s", account,password );	

		printf("user password %s\n",usrpasswd);
		
		curl_easy_setopt(curl, CURLOPT_USERPWD, usrpasswd);
		

		/* Set the size of the file to upload (optional).  If you give a *_LARGE
		   option you MUST make sure that the type of the passed-in argument is a
		   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		   make sure that to pass in a type 'long' argument. */ 
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
		                 (curl_off_t)fsize);

		/* Now run off and do what you've been told! */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		  fprintf(stderr, "curl_easy_perform() failed: %s\n",
		          curl_easy_strerror(res));

		/* clean up the FTP commands list */ 
		curl_slist_free_all(headerlist);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	fclose(hd_src); /* close the local file */ 

	curl_global_cleanup();
	return 0;
}


