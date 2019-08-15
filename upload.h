/*
 * @file serverconfig.h
 * @brief 浏览器表单接口函数定义
 * @author Joran
 * @version 1.0.0.1
 * @date 2018-8-13
 *
 * change history:
 * <Date>    |  <version>  |  <Author>    |  <Description>  
 * 
 */

/************************** define *************************/

#ifndef _UPLOAD_H
#define _UPLOAD_H
#ifdef __cplusplus
extern "C" {
#endif

int upload_file_by_http(const char * url,const char * file_path);
int upload_file_by_https(const char * url,const char * file_path);
int upload_file_by_ftp(const char * sever,int port,const char *account,const char *password,const char * file_path);



#ifdef __cplusplus
  }
#endif


#endif
