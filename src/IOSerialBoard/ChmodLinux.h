/*!
 *	\file	ChmodLinux.h
 *
 *	\brief	�޸���linuxϵͳ�ϵĴ������ã���ȡ�����ڵĻ�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	15:56
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef CHMODLINUX_20080708_H
#define CHMODLINUX_20080708_H

#ifndef _WINDOWS
#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/

/*!
 *	\brief	ȡ�����ڲ����еĻ�������
 *
 *	\param	fd	 �򿪵Ĵ����ļ����
 *
 *	\retval	int	0��ʾ�ɹ��������ʾʧ��
 */
int cancel_echo(int fd)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) 
    { 
		perror("SetupSerial 1");     
		return -1;  
	}
	//! ���ñ���ģʽΪԭʼ����
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/*! add by taiwanxia at 2010-3-11
	termios.c_iflag; ����ģʽ 
	termios.c_oflag; ���ģʽ
	termios.c_cflag; ����ģʽ
	termios.c_lflag; ����ģʽ
	!*/

	//! �������ģʽΪԭʼ���
	options.c_oflag = 0;
	//! ���������ݴ���ʱ������������
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
*	\brief	�ļ���ʽ�򿪶˿�
*
*	\param	Dev	 ��������
*
*	\retval	int	�򿪵Ĵ����ļ������-1���ʾ��ʧ��
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
*	\brief	�޸���linuxϵͳ�ϵĴ������ã���ȡ�����ڵĻ�������
*
*	\param	dev	 ��������
*
*	\retval	int	0��ʾ�ɹ��������ʾʧ��
*/
int chComMod(char* dev)
{
	int fd;
	int nread;
	char buff[512];
	//char *dev  = "/dev/ttyS0"; //����1
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