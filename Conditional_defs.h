//==================================================================================
//
// if条件文関連マクロ定義ヘッダファイル [Conditional_defs.h]
// Author : TENMA
//
//==================================================================================
#ifndef _CONDITIONAL_DEFS_H_		// インクルードガード
#define _CONDITIONAL_DEFS_H_

//**********************************************************************************
//*** インクルード関連 ***
//**********************************************************************************
#include <string.h>

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define CMP_SUCCESS(cmp)		(cmp == 0)			// strcmp,strncmpでの成功判定
#define CMP_FAILED(cmp)			(cmp != 0)			// strcmp,strncmpでの失敗判定
#define STR_SUCCESS(str)		(str != NULL)		// strstrでの成功判定
#define STR_FAILED(str)			(str == NULL)		// strstrでの失敗判定

#define C_SUCCEED(a, b)			CMP_SUCCESS(strcmp(a, b))			// strcmp省略版
#define C_FAILED(a, b)			CMP_FAILED(strcmp(a, b))			// strcmp省略版
#define ST_SUCCEED(a, b)		STR_SUCCESS(strstr(a, b))			// strstr省略版
#define ST_FAILED(a, b)			STR_FAILED(strstr(a, b))			// strstr省略版

#endif // !_CONDITIONAL_DEFS_H_