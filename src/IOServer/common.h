/*!
 *	\file	common.h
 *
 *	\brief	简短的文件说明（可选）
 *
 *	详细的文件说明（可选）
 *
 *	\author	yl
 *
 *	\date	2014年7月16日	16:37
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef STARTSERVERS_COMMON_H_
#define STARTSERVERS_COMMON_H_


#define HY_PROC_SUFFIX_WIN							".exe"
#define HY_PROC_SUFFIX_LINUX						""

#define HY_ACE_DEBUG_PREFIX							""

#define HY_MSG_CHECK_MONIPROC_INVALID				"Invalid argument! Please enter -start or -stop!"

#define HY_MSG_CHECK_MONIPROC_ISRUNNING				"Checking whether monitoring process is running..."
#define HY_MSG_CHECK_MONIPROC_RUNNING				"Running, exit and restart please!"
#define HY_MSG_CHECK_MONIPROC_START					"Starting monitoring process..."
#define HY_MSG_CHECK_MONIPROC_STOP					"Stopping monitoring process..."

#define HY_MSG_NOTRUNNING							"Not running."
#define HY_MSG_SUCCESS								"Success!"
#define HY_MSG_FAILURE								"Failure!"

#define HY_MSG_START_SUCCESS						"Start Success!"
#define HY_MSG_START_FAILURE						"Start Failure!"

#define HY_MSG_STOP_SUCCESS							"Stop Success!"
#define HY_MSG_STOP_FAILURE							"Stop Failure!"

#define HY_MSG_SERVICE_START						"Starting service process..."
#define HY_MSG_SERVICE_EXIT_1						"Enter"
#define HY_MSG_SERVICE_EXIT_2						"to exit all service and monitoring processes."
#define HY_MSG_SERVICE_START_FAIL					"Fail to Start service process!"

#define HY_MSG_DIR_SETUP_NOTCONFIG					"SetupDir is not configured!"
#define HY_MSG_DIR_SETUP_NOTEXIST_1					"SetupDir"
#define HY_MSG_DIR_SETUP_NOTEXIST_2					"does not exist!"

#define HY_MSG_DIR_RUN_NOTCONFIG					"RunDir is not configured!"
#define HY_MSG_DIR_RUN_NOTEXIST_1					"RunDir"
#define HY_MSG_DIR_RUN_NOTEXIST_2					"does not exist!"

#define HY_MSG_CHECK_INSTALL						"Installed."
#define HY_MSG_CHECK_NOTINSTALL						"Not installed."
#define HY_MSG_CHECK_RUNNING						"Running! Exiting..."

#define HY_MSG_CHECK_DLSVR_ISINSTALL				"Checking whether file download service is installed..."
#define HY_MSG_CHECK_DLSVR_ISRUNNING				"Checking whether file download service process is running..."
#define HY_MSG_CHECK_DLSVR_START					"Starting file download service process..."

#define HY_MSG_CHECK_DLPROXY_ISINSTALL				"Checking whether file download proxy is installed..."
#define HY_MSG_CHECK_DLPROXY_ISRUNNING				"Checking whether file download proxy process is running..."
#define HY_MSG_CHECK_DLPROXY_START					"Starting file download proxy process..."
#define HY_MSG_FILE_DOWNLOAD_SUCCESS				"File download successful"

#define HY_MSG_CHECK_DLFILE							"Waiting for downloading configuration files..."

#define HY_MSG_CHECK_SVRPROC_ISRUNNING_1			"Checking whether service process"
#define HY_MSG_CHECK_SVRPROC_ISRUNNING_2			"is running..."
#define HY_MSG_CHECK_SVRPROC_START					"Starting service process"
#define HY_MSG_CHECK_SVRPROC_RESTART				"Restarting service process"
#define HY_MSG_CHECK_SVRPROC_STOP					"Stopping service process"
#define HY_MSG_CHECK_SVRPROC_STOP_SUCCE 			"Service process stoped successfully"
#define HY_MSG_CHECK_SVRPROC_START_SUCCE 			"Service process connect successfully "

#endif // STARTSERVERS_COMMON_H_