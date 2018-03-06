/*!
 *	\file	ChmodLinux.h
 *
 *	\brief	修改类linux系统上的串口设置，以取消串口的回显特性
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	15:56
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef CHMODLINUX_20080708_H
#define CHMODLINUX_20080708_H

#ifndef _WINDOWS
#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/

/*!
 *	\brief	取消串口参数中的回显特性
 *
 *	\param	fd	 打开的串口文件句柄
 *
 *	\retval	int	0表示成功，否则表示失败
 */
int cancel_echo(int fd)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) 
    { 
		perror("SetupSerial 1");     
		return -1;  
	}
	//! 设置本地模式为原始输入
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/*! add by taiwanxia at 2010-3-11
	termios.c_iflag; 输入模式 
	termios.c_oflag; 输出模式
	termios.c_cflag; 控制模式
	termios.c_lflag; 本地模式
	!*/

	//! 设置输出模式为原始输出
	options.c_oflag = 0;
	//! 二进制数据传输时必须这样设置
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	tcflush(fd,TCIFLUSH);
	//! end
    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    { 
        perror("SetupSerial 3");   
        return -1;  
    } 
    return 0;  
}

/*!
*	\brief	文件方式打开端口
*
*	\param	Dev	 串口名称
*
*	\retval	int	打开的串口文件句柄，-1则表示打开失败
*/
int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR |O_NOCTTY |O_NDELAY );         //  O_NOCTTY   O_NDELAY	
	if (-1 == fd)	
	{ 			
		perror("Can't Open Serial Port");
		return -1;		
	}	
	else	
		return fd;
}

/*!
*	\brief	修改类linux系统上的串口设置，以取消串口的回显特性
*
*	\param	dev	 串口名称
*
*	\retval	int	0表示成功，否则表示失败
*/
int chComMod(char* dev)
{
	int fd;
	int nread;
	char buff[512];
	//char *dev  = "/dev/ttyS0"; //串口1
	fd = OpenDev(dev);
	if (cancel_echo(fd) != 0)  
	{
		printf("Cancel Echo Error\n");
		return -1;
	}

	close(fd);
	return 0;
}

#endif

#endif