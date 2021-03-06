/* Copyright (C) 2010 Ntropy (ntropy AT qsp DOT su) */
/*
* This library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2.1 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "../../declarations.h"

#ifdef _JAVA

#include "../../actions.h"
#include "../../callbacks.h"
#include "../../coding.h"
#include "../../common.h"
#include "../../errors.h"
#include "../../game.h"
#include "../../locations.h"
#include "../../mathops.h"
#include "../../menu.h"
#include "../../objects.h"
#include "../../statements.h"
#include "../../text.h"
#include "../../time_qsp.h"
#include "../../variables.h"
#include "../../variant.h"

/* ------------------------------------------------------------ */
void QSPIsInCallBack(QSP_BOOL *res)
{
	*res = qspIsInCallBack;
}
/* ------------------------------------------------------------ */
/* ??????? */

/* ?????????? ??????? ??????? */
void QSPEnableDebugMode(QSP_BOOL isDebug)
{
	qspIsDebug = isDebug;
}
/* ????????? ?????? ???????? ????????? */
void QSPGetCurStateData(QSP_CHAR **loc, int *actIndex, int *line)
{
	*loc = (qspRealCurLoc >= 0 && qspRealCurLoc < qspLocsCount ? qspLocs[qspRealCurLoc].Name : 0);
	*actIndex = qspRealActIndex;
	*line = qspRealLine;
}
/* ------------------------------------------------------------ */
/* ?????????? ? ?????? */

/* ?????? */
void QSPGetVersion(const QSP_CHAR* *res)
{
	*res = QSP_VER;
}
/* ???? ? ????? ?????????? */
void QSPGetCompiledDateTime(const QSP_CHAR* *res)
{
	*res = QSP_FMT(__DATE__) QSP_FMT(", ") QSP_FMT(__TIME__);
}
/* ------------------------------------------------------------ */
/* ?????????? ?????? ?????????? ??????? */
void QSPGetFullRefreshCount(int *res)
{
	*res = qspFullRefreshCount;
}
/* ------------------------------------------------------------ */
/* ?????? ???? ? ???????????? ????? ???? */
void QSPGetQstFullPath(const QSP_CHAR* *res)
{
	*res = qspQstFullPath;
}
/* ------------------------------------------------------------ */
/* ???????? ??????? ??????? */
void QSPGetCurLoc(const QSP_CHAR* *res)
{
	*res = (qspCurLoc >= 0 ? qspLocs[qspCurLoc].Name : 0);
}
/* ------------------------------------------------------------ */
/* ???????? ???????? ??????? */

/* ????? ????????? ???? ???????? ??????? */
void QSPGetMainDesc(const QSP_CHAR* *res)
{
	*res = qspCurDesc;
}
/* ??????????? ????????? ?????? ????????? ???????? */
void QSPIsMainDescChanged(QSP_BOOL *res)
{
	*res = qspIsMainDescChanged;
}
/* ------------------------------------------------------------ */
/* ?????????????? ???????? ??????? */

/* ????? ??????????????? ???? ???????? ??????? */
void QSPGetVarsDesc(const QSP_CHAR* *res)
{
	*res = qspCurVars;
}
/* ??????????? ????????? ?????? ??????????????? ???????? */
void QSPIsVarsDescChanged(QSP_BOOL *res)
{
	*res = qspIsVarsDescChanged;
}
/* ------------------------------------------------------------ */
/* ???????? ???????? ?????????? ????????? */
void QSPGetExprValue(QSP_BOOL *res, const QSP_CHAR *expr, QSP_BOOL *isString, int *numVal, QSP_CHAR *strVal, int strValBufSize)
{
	QSPVariant v;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspResetError();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	v = qspExprValue((QSP_CHAR *)expr);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	*isString = v.IsStr;
	if (v.IsStr)
	{
		qspStrNCopy(strVal, QSP_STR(v), strValBufSize - 1);
		free(QSP_STR(v));
		strVal[strValBufSize - 1] = 0;
	}
	else
		*numVal = QSP_NUM(v);
	*res = QSP_TRUE;
}
/* ------------------------------------------------------------ */
/* ????? ?????? ????? */
void QSPSetInputStrText(const QSP_CHAR *val)
{
	qspCurInputLen = qspAddText(&qspCurInput, (QSP_CHAR *)val, 0, -1, QSP_FALSE);
}
/* ------------------------------------------------------------ */
/* ?????? ???????? */

/* ?????????? ???????? */
void QSPGetActionsCount(int* res)
{
	*res = qspCurActionsCount;
}
/* ?????? ???????? ? ????????? ???????? */
void QSPGetActionData(int ind, QSP_CHAR **image, QSP_CHAR **desc)
{
	if (ind >= 0 && ind < qspCurActionsCount)
	{
		*image = qspCurActions[ind].Image;
		*desc = qspCurActions[ind].Desc;
	}
	else
		*image = *desc = 0;
}
/* ?????????? ???? ?????????? ???????? */
void QSPExecuteSelActionCode(QSP_BOOL *res, QSP_BOOL isRefresh)
{
	if (qspCurSelAction >= 0)
	{
		if (qspIsExitOnError && qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		qspPrepareExecution();
		if (qspIsDisableCodeExec)
		{
			*res = QSP_FALSE;
			return;
		}
		qspExecAction(qspCurSelAction);
		if (qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	}
	*res = QSP_TRUE;
}
/* ?????????? ?????? ?????????? ???????? */
void QSPSetSelActionIndex(QSP_BOOL *res, int ind, QSP_BOOL isRefresh)
{
	if (ind >= 0 && ind < qspCurActionsCount && ind != qspCurSelAction)
	{
		if (qspIsExitOnError && qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		qspPrepareExecution();
		if (qspIsDisableCodeExec)
		{
			*res = QSP_FALSE;
			return;
		}
		qspCurSelAction = ind;
		qspExecLocByVarNameWithArgs(QSP_FMT("ONACTSEL"), 0, 0);
		if (qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	}
	*res = QSP_TRUE;
}
/* ???????? ?????? ?????????? ???????? */
void QSPGetSelActionIndex(int *res)
{
	*res = qspCurSelAction;
}
/* ??????????? ????????? ?????? ???????? */
void QSPIsActionsChanged(QSP_BOOL *res)
{
	*res = qspIsActionsChanged;
}
/* ------------------------------------------------------------ */
/* ?????? ???????? */

/* ?????????? ???????? */
void QSPGetObjectsCount(int *res)
{
	*res = qspCurObjectsCount;
}
/* ?????? ??????? ? ????????? ???????? */
void QSPGetObjectData(int ind, QSP_CHAR **image, QSP_CHAR **desc)
{
	if (ind >= 0 && ind < qspCurObjectsCount)
	{
		*image = qspCurObjects[ind].Image;
		*desc = qspCurObjects[ind].Desc;
	}
	else
		*image = *desc = 0;
}
/* ?????????? ?????? ?????????? ??????? */
void QSPSetSelObjectIndex(QSP_BOOL *res, int ind, QSP_BOOL isRefresh)
{
	if (ind >= 0 && ind < qspCurObjectsCount && ind != qspCurSelObject)
	{
		if (qspIsExitOnError && qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		qspPrepareExecution();
		if (qspIsDisableCodeExec)
		{
			*res = QSP_FALSE;
			return;
		}
		qspCurSelObject = ind;
		qspExecLocByVarNameWithArgs(QSP_FMT("ONOBJSEL"), 0, 0);
		if (qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	}
	*res = QSP_TRUE;
}
/* ???????? ?????? ?????????? ??????? */
void QSPGetSelObjectIndex(int* res)
{
	*res = qspCurSelObject;
}
/* ??????????? ????????? ?????? ???????? */
void QSPIsObjectsChanged(QSP_BOOL *res)
{
	*res = qspIsObjectsChanged;
}
/* ------------------------------------------------------------ */
/* ????? / ??????? ???? */
void QSPShowWindow(int type, QSP_BOOL isShow)
{
	switch (type)
	{
	case QSP_WIN_ACTS:
		qspCurIsShowActs = isShow;
		break;
	case QSP_WIN_OBJS:
		qspCurIsShowObjs = isShow;
		break;
	case QSP_WIN_VARS:
		qspCurIsShowVars = isShow;
		break;
	case QSP_WIN_INPUT:
		qspCurIsShowInput = isShow;
		break;
	}
}
/* ------------------------------------------------------------ */
/* ?????????? */

/* ???????? ?????????? ????????? ??????? */
void QSPGetVarValuesCount(QSP_BOOL *res, const QSP_CHAR *name, int *count)
{
	QSPVar *var;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspResetError();
	var = qspVarReference((QSP_CHAR *)name, QSP_FALSE);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	*count = var->ValsCount;
	*res = QSP_TRUE;
}
/* ???????? ???????? ?????????? ???????? ??????? */
void QSPGetVarValues(QSP_BOOL *res, const QSP_CHAR *name, int ind, int *numVal, QSP_CHAR **strVal)
{
	QSPVar *var;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspResetError();
	var = qspVarReference((QSP_CHAR *)name, QSP_FALSE);
	if (qspErrorNum || ind < 0 || ind >= var->ValsCount)
	{
		*res = QSP_FALSE;
		return;
	}
	*numVal = var->Values[ind].Num;
	*strVal = var->Values[ind].Str;
	*res = QSP_TRUE;
}
/* ???????? ???????????? ?????????? ?????????? */
void QSPGetMaxVarsCount(int *res)
{
	*res = QSP_VARSCOUNT;
}
/* ???????? ??? ?????????? ? ????????? ???????? */
void QSPGetVarNameByIndex(QSP_BOOL *res, int index, QSP_CHAR **name)
{
	if (index < 0 || index >= QSP_VARSCOUNT || !qspVars[index].Name)
	{
		*res = QSP_FALSE;
		return;
	}
	*name = qspVars[index].Name;
	*res = QSP_TRUE;
}
/* ------------------------------------------------------------ */
/* ?????????? ???? */

/* ?????????? ?????? ???? */
void QSPExecString(QSP_BOOL *res, const QSP_CHAR *s, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspExecStringAsCodeWithArgs((QSP_CHAR *)s, 0, 0);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ?????????? ???? ????????? ??????? */
void QSPExecLocationCode(QSP_BOOL *res, const QSP_CHAR *name, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspExecLocByName((QSP_CHAR *)name, QSP_FALSE);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ?????????? ???? ???????-???????? */
void QSPExecCounter(QSP_BOOL *res, QSP_BOOL isRefresh)
{
	if (!qspIsInCallBack)
	{
		qspPrepareExecution();
		qspExecLocByVarNameWithArgs(QSP_FMT("COUNTER"), 0, 0);
		if (qspErrorNum)
		{
			*res = QSP_FALSE;
			return;
		}
		if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	}
	*res = QSP_TRUE;
}
/* ?????????? ???? ???????-??????????? ?????? ????? */
void QSPExecUserInput(QSP_BOOL *res, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspExecLocByVarNameWithArgs(QSP_FMT("USERCOM"), 0, 0);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ------------------------------------------------------------ */
/* ?????? */

/* ???????? ?????????? ? ????????? ?????? */
void QSPGetLastErrorData(int *errorNum, QSP_CHAR **errorLoc, int *errorActIndex, int *errorLine)
{
	*errorNum = qspErrorNum;
	*errorLoc = (qspErrorLoc >= 0 && qspErrorLoc < qspLocsCount ? qspLocs[qspErrorLoc].Name : 0);
	*errorActIndex = qspErrorActIndex;
	*errorLine = qspErrorLine;
}
/* ???????? ???????? ?????? ?? ?? ?????? */
void QSPGetErrorDesc(const QSP_CHAR* *res, int errorNum)
{
	*res = qspGetErrorDesc(errorNum);
}
/* ------------------------------------------------------------ */
/* ?????????? ????? */

/* ???????? ????? ???? ?? ????? */
void QSPLoadGameWorld(QSP_BOOL *res, const QSP_CHAR *fileName)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspResetError();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspOpenQuest((QSP_CHAR *)fileName, QSP_FALSE);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	*res = QSP_TRUE;
}
/* ???????? ????? ???? ?? ?????? */
void QSPLoadGameWorldFromData(QSP_BOOL *res, const void *data, int dataSize, const QSP_CHAR *fileName)
{
	char *ptr;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspResetError();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	ptr = (char *)malloc(dataSize + 3);
	memcpy(ptr, data, dataSize);
	ptr[dataSize] = ptr[dataSize + 1] = ptr[dataSize + 2] = 0;
	qspOpenQuestFromData(ptr, dataSize + 3, (QSP_CHAR *)fileName, QSP_FALSE);
	free(ptr);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	*res = QSP_TRUE;
}
/* ?????????? ????????? ? ???? */
void QSPSaveGame(QSP_BOOL *res, const QSP_CHAR *fileName, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspSaveGameStatus((QSP_CHAR *)fileName);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ?????????? ????????? ? ?????? */
void QSPSaveGameAsData(QSP_BOOL *res, void *buf, int bufSize, int *realSize, QSP_BOOL isRefresh)
{
	int len, size;
	QSP_CHAR *data;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	if (!(len = qspSaveGameStatusToString(&data)))
	{
		*realSize = 0;
		*res = QSP_FALSE;
		return;
	}
	size = len * sizeof(QSP_CHAR);
	*realSize = size;
	if (size > bufSize)
	{
		free(data);
		*res = QSP_FALSE;
		return;
	}
	memcpy(buf, data, size);
	free(data);
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ???????? ????????? ?? ????? */
void QSPOpenSavedGame(QSP_BOOL *res, const QSP_CHAR *fileName, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspOpenGameStatus((QSP_CHAR *)fileName);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ???????? ????????? ?? ?????? */
void QSPOpenSavedGameFromData(QSP_BOOL *res, const void *data, int dataSize, QSP_BOOL isRefresh)
{
	int dataLen;
	QSP_CHAR *ptr;
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	dataLen = dataSize / sizeof(QSP_CHAR);
	ptr = (QSP_CHAR *)malloc((dataLen + 1) * sizeof(QSP_CHAR));
	memcpy(ptr, data, dataSize);
	ptr[dataLen] = 0;
	qspOpenGameStatusFromString(ptr);
	free(ptr);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ?????????? ???? */
void QSPRestartGame(QSP_BOOL *res, QSP_BOOL isRefresh)
{
	if (qspIsExitOnError && qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	qspPrepareExecution();
	if (qspIsDisableCodeExec)
	{
		*res = QSP_FALSE;
		return;
	}
	qspNewGame(QSP_TRUE);
	if (qspErrorNum)
	{
		*res = QSP_FALSE;
		return;
	}
	if (isRefresh) qspCallRefreshInt(QSP_FALSE);
	*res = QSP_TRUE;
}
/* ------------------------------------------------------------ */
/* ???? */
/* ?-? ????????????? ?????? ??? ?????? ?? CallBack'? QSP_CALL_SHOWMENU */
void QSPSelectMenuItem(int index)
{
	QSPVariant arg;
	if (index >= 0 && index < qspCurMenuItems)
	{
		if (qspIsDisableCodeExec) return;
		arg.IsStr = QSP_FALSE;
		QSP_NUM(arg) = index + 1;
		qspExecLocByNameWithArgs(qspCurMenuLocs[index], &arg, 1);
	}
}
/* ------------------------------------------------------------ */
/* ????????? CALLBACK'?? */
void QSPSetCallBack(int type, QSP_CALLBACK func)
{
	qspSetCallBack(type, func);
}
/* ------------------------------------------------------------ */
/* ????????????? */
void QSPInit()
{
	#ifdef _DEBUG
		mwInit();
	#endif
	qspIsDebug = QSP_FALSE;
	qspRefreshCount = qspFullRefreshCount = 0;
	qspQstPath = qspQstFullPath = 0;
	qspQstPathLen = 0;
	qspQstCRC = 0;
	qspRealCurLoc = -1;
	qspRealActIndex = -1;
	qspRealLine = 0;
	qspMSCount = 0;
	qspLocs = 0;
	qspLocsNames = 0;
	qspLocsCount = 0;
	qspCurLoc = -1;
	qspTimerInterval = 0;
	qspCurIsShowObjs = qspCurIsShowActs = qspCurIsShowVars = qspCurIsShowInput = QSP_TRUE;
	setlocale(LC_ALL, QSP_LOCALE);
	qspSetSeed(0);
	qspPrepareExecution();
	qspMemClear(QSP_TRUE);
	qspInitCallBacks();
	qspInitStats();
	qspInitMath();
}
/* ??????????????? */
void QSPDeInit()
{
	qspMemClear(QSP_FALSE);
	qspCreateWorld(0, 0);
	if (qspQstPath) free(qspQstPath);
	if (qspQstFullPath) free(qspQstFullPath);
	#ifdef _DEBUG
		mwTerm();
	#endif
}

#endif
