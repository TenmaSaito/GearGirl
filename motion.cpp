//==================================================================================
//
// DirectXのモーション読み込み表示処理 [motion.cpp]
// Author : TENMA
//
//==================================================================================
//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "motion.h"
#include "modeldata.h"
#include "mathUtil.h"

//**********************************************************************************
//*** マクロ定義 ***
//**********************************************************************************
#define READ_MODELNUM			"NUM_MODEL"			// 読み込むモデル数の前文字列
#define READ_MODELPATH			"MODEL_FILENAME"	// ファイル名を読み込む印
#define READ_CHARACTER			"CHARACTERSET"		// 階層構造のキャラクター情報
#define READ_ENDCHARACTER		"END_CHARACTERSET"	// キャラクター情報設定の解除
#define READ_PARTS				"NUM_PARTS"			// パーツ数
#define READ_INDEX				"INDEX"				// インデックス
#define READ_POS				"POS"				// 位置
#define READ_ROT				"ROT"				// 角度
#define READ_RADIUS				"RADIUS"			// 球の当たり判定の半径
#define READ_HEIGHT				"HEIGHT"			// 上下の当たり判定の大きさ
#define READ_JUMP				"JUMP"				// 跳躍力
#define READ_MOVE				"MOVE"				// 移動量
#define READ_PARTSSET			"PARTSSET"			// パーツ情報の設定
#define READ_PARENT				"PARENT"			// パーツの親インデックス
#define READ_ENDPARTSSET		"END_PARTSSET"		// パーツ情報設定の解除
#define READ_MOTIONSET			"MOTIONSET"			// モーションデータの読み込み
#define READ_ENDMOTIONSET		"END_MOTIONSET"		// モーション設定の解除
#define READ_KEYSET				"KEYSET"			// キー情報の読み込み
#define READ_ENDKEYSET			"END_KEYSET"		// キー情報設定の解除
#define READ_KEY				"KEY"				// キーの読み込み
#define READ_ENDKEY				"END_KEY"			// キー設定の解除
#define READ_LOOP				"LOOP"				// ループ情報
#define	READ_NUMKEY				"NUM_KEY"			// キー情報の数
#define READ_FRAME				"FRAME"				// フレーム数

#define CMP_SUCCESS(cmp)		(cmp == 0)			// strcmp,strncmpでの成功判定
#define CMP_FAILED(cmp)			(cmp != 0)			// strcmp,strncmpでの失敗判定
#define STR_SUCCESS(str)		(str != NULL)		// strstrでの成功判定
#define STR_FAILED(str)			(str == NULL)		// strstrでの失敗判定

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
bool LoadFileFromMotionScript(const char* pMotionFileName);
bool DeleteComments(char* pStr);

//**********************************************************************************
//*** グローバル変数 ***
//**********************************************************************************
MOTIONSCRIPT_INFO g_aMotionInfo[MAX_MOTION];			// モーション情報
PARTS_INFO g_aPartsInfo[MAX_MOTION];					// パーツ情報
int g_nNumScriptInfo;									// 読み込んだスクリプトの数

//==================================================================================
// --- 初期化 ---
//==================================================================================
void InitMotion(void)
{
	// モーション情報の初期化
	ZeroMemory(&g_aMotionInfo[0], sizeof(MOTIONSCRIPT_INFO) * (MAX_MOTION));

	// パーツ情報の初期化
	ZeroMemory(&g_aPartsInfo[0], sizeof(PARTS_INFO) * (MAX_MOTION));

	// スクリプトを読み込んだ回数を初期化
	g_nNumScriptInfo = 0;
}

//==================================================================================
// --- モーション情報の読み込み ---
//==================================================================================
bool LoadMotion(_In_ const char* pMotionFileName, int* pOutIdx)
{
	LPMOTIONSCRIPT_INFO pMotionInfo = &g_aMotionInfo[0];

	if (pOutIdx)
	{ // エラー番号を格納
		*pOutIdx = ERROR_MOTION;
	}

	/*** 過去に読み込んでいないか確認 ***/
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++, pMotionInfo++)
	{
		if (strcmp(&pMotionInfo->aFileName[0], pMotionFileName) == NULL)
		{
			if (pOutIdx)
			{ // モーション番号を格納
				*pOutIdx = nCntMotion;
			}

			return true;
		}
	}

	pMotionInfo = &g_aMotionInfo[0];

	/*** モーションの読み込み ***/
	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++, pMotionInfo++)
	{
		if (pMotionInfo->bUse != true)
		{
			// 使用状態に
			pMotionInfo->bUse = true;

			if (LoadFileFromMotionScript(pMotionFileName) == false)
			{ // 読み込み失敗時
				MyMathUtil::GenerateMessageBox(MB_ICONERROR,
					"MotionLoadError",
					"モーション読み込みに失敗。\n対象パス:%s",
					pMotionFileName);

				return false;
			}

			// 相対パスになっているか確認
			MyMathUtil::CheckPath(pMotionFileName);

			// モーションパスを保存
			ZeroMemory(&pMotionInfo->aFileName[nCntMotion], sizeof(char) * MAX_PATH);
			strcpy(&pMotionInfo->aFileName[nCntMotion], pMotionFileName);

			if (pOutIdx)
			{ // テクスチャ番号を格納
				*pOutIdx = nCntMotion;
			}

			g_nNumScriptInfo++;

			/*** 成功 ***/
			return true;
		}
	}

	/*** 上限オーバー ***/
	return false;
}

//==================================================================================
// ---モーション情報の終了 ---
//==================================================================================
void UninitMotion(void)
{
	// motion.cppにて動的にメモリを確保した場合、ここで解放
}

//==================================================================================
// --- モーション情報の取得 ---
//==================================================================================
LPMOTIONSCRIPT_INFO GetMotionScriptInfo(int nType)
{
	if (nType < 0 || nType >= MAX_MOTION) return NULL;
	LPMOTIONSCRIPT_INFO pMotionInfo = &g_aMotionInfo[nType];
	if (pMotionInfo->bUse == true)
	{
		return pMotionInfo;
	}
	else
	{
		return NULL;
	}
}

//==================================================================================
// --- パーツ情報の取得 ---
//==================================================================================
LPPARTS_INFO GetPartsInfo(int nType)
{
	if (nType < 0 || nType >= MAX_MOTION) return NULL;
	LPPARTS_INFO pPartsInfo = &g_aPartsInfo[nType];
	if (pPartsInfo->bUse == true)
	{
		return pPartsInfo;
	}
	else
	{
		return NULL;
	}
}

//==================================================================================
// --- モーション情報の読み取り ---
//==================================================================================
bool LoadFileFromMotionScript(const char* pMotionFileName)
{
	/** スクリプト読み込み関連 **/
	FILE* pFile = NULL;				// ファイルポインタ
	HRESULT hr;						// 正誤判定
	char aStr[MAX_PATH] = {};		// Xファイルのファイル名
	char aModelPath[MAX_PATH] = {};	// Xファイルパス
	char* PosTrash;					// ゴミ捨て場(コメント消去用変数)
	char* pStart;					// 値の開始位置
	const char* pNull = "\0";		// 何もなし
	char aErrorText[MAX_PATH] = {};	// エラー文用

	int nModel = 0;					// 設置したモデル数
	int nLoadModelPath = 0;			// 読み込んだモデル数
	int aIdxObject[MAX_KEY] = { -1 };		// 読み込んだモデルパスのオブジェクトでのインデックス
	int nIndexModel = 0;			// 読み込んだモデルのインデックス
	int nTypeModel = 1;				// 読み込んだモデルのタイプ
	int nParts = 0;					// 読み込んだパーツ情報
	int nIdxHead = 0;				// 頭
	int nIdxHand = 0;				// 手

	/*** モーション変数 ***/
	int nNumParts = 0;						// パーツ総数
	int nLoop = 0;							// ループの有無
	int nKeySet = 0;						// 現在設定中のキー情報
	int nKey = 0;							// 現在設定中のキー
	bool bLoop = false;						// キャスト用
	float fRadius = 0.0f;					// 半径
	float fHeight = 0.0f;					// 高さ
	float fMove = 0.0f;						// 移動量
	float fJump = 0.0f;						// 跳躍力
	LPPARTS_INFO pPInfo = &g_aPartsInfo[g_nNumScriptInfo];			// パーツ情報
	LPMOTIONSCRIPT_INFO pMInfo = &g_aMotionInfo[g_nNumScriptInfo];		// モーション情報
	MOTIONTYPE mType = MOTIONTYPE_NEUTRAL;	// モーションタイプ
	int nType = MOTIONTYPE_NEUTRAL;			// 現在設定中のモーションタイプ(キャスト及び判定用)

	/*** ファイルオープン ***/
	pFile = fopen(pMotionFileName, "r");
	if (pFile == NULL)
	{ // ファイルオープン失敗時
		MyMathUtil::GenerateMessageBox(MB_ICONERROR,
			"Error (0)",
			"Scriptファイルの読み込みに失敗しました。");
		return false;
	}

	/*** SCRIPT確認 ***/
	while (1)
	{
		memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
		(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得
		/*** コメント消去処理 ***/
		if (DeleteComments(&aStr[0]))
		{
			if (CMP_SUCCESS(strcmp(aStr, "SCRIPT")))
			{ // SCRIPT発見時読み込み開始
				break;
			}
			else if (feof(pFile) != NULL)
			{ // 発見できなかった場合読み込み終了 E_FAIL
				MyMathUtil::GenerateMessageBox(MB_ICONERROR,
					"Error (1)",
					"Scriptの読み込み開始位置が指定されていません。");

				/*** 読み込み終了 ***/
				fclose(pFile);

				return false;
			}
		}
	}

	/*** 読み込みループ処理 ***/
	while (1)
	{
		memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
		(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

		if (feof(pFile) != 0)
		{
			MyMathUtil::GenerateMessageBox(MB_ICONERROR,
				"Error (2)",
				"END_SCRIPT無しに読み込みが異常終了しました。");

			/*** 読み込み終了 ***/
			fclose(pFile);

			return false;				// もし取得後EOFの場合、読み込み終了
		}

		/*** コメント消去処理 ***/
		if (DeleteComments(&aStr[0]))
		{ // 取得後、最初の文字が#(コメントアウト宣言)だった場合、読み込まない
			/*** END_SCRIPTか確認 ***/
			if (strstr(aStr, "END_SCRIPT") != NULL)
			{ // 読み込み終了の合図
				break;
			}
			else if (strstr(aStr, READ_MODELNUM) != NULL)
			{
				pStart = strchr(aStr, '=');

				/** パーツ数の読み込み **/
				(void)sscanf(pStart + 1, "%d", &nModel);
			}
			else if (strstr(aStr, READ_MODELPATH) != NULL)
			{
				/** モデルパス初期化 **/
				memset(&aModelPath[0], NULL, sizeof(char) * MAX_PATH);

				/** インデックスを初期化 **/
				aIdxObject[nLoadModelPath] = -1;

				pStart = strchr(aStr, '=');

				/** Xファイル名の読み込み **/
				(void)sscanf(pStart + 1, "%s", &aModelPath[0]);

				/** 過去に読み込んだファイル名と重複していないか確認 **/
				hr = LoadModelData(aModelPath, &aIdxObject[nLoadModelPath]);
				if (FAILED(hr) && hr != E_FAIL)
				{ // 原因特定が可能なエラーなら、エラー文を表示
					LPVOID* errorString;

					FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER					// テキストのメモリ割り当てを要求する
						| FORMAT_MESSAGE_FROM_SYSTEM					// エラーメッセージはWindowsが用意しているものを使用
						| FORMAT_MESSAGE_IGNORE_INSERTS,				// 次の引数を無視してエラーコードに対するエラーメッセージを作成する
						NULL,
						hr,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),		// 言語を指定
						(LPTSTR)&errorString,							// メッセージテキストが保存されるバッファへのポインタ
						0,
						NULL);

					MyMathUtil::GenerateMessageBox(MB_ICONERROR,
						"error",
						(LPCTSTR)errorString);

					// メモリ開放
					LocalFree(errorString);
				}

				// 読み込んだパーツ数を増加数
				nLoadModelPath++;
			}
			else if (strstr(aStr, READ_CHARACTER) != NULL)
			{
				pPInfo->bUse = true;

				while (1)
				{
					memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
					(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

					if (feof(pFile) != 0)
					{
						MyMathUtil::GenerateMessageBox(MB_ICONERROR,
							"Error (3)",
							"END_CHARACTER無しに読み込みが異常終了しました。");

						/*** 読み込み終了 ***/
						fclose(pFile);

						return false;				// もし取得後EOFの場合、読み込み終了
					}

					if (DeleteComments(&aStr[0]))
					{
						if (strstr(aStr, READ_ENDCHARACTER) != NULL)
						{
							// 取得した情報を渡す
							pPInfo->nNumParts = nNumParts;
							break;
						}
						else if (strstr(aStr, READ_PARTS) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** パーツ数の読み込み **/
							(void)sscanf(pStart + 1, "%d", &nNumParts);
						}
						else if (strstr(aStr, READ_RADIUS) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** 当たり判定の半径の読み込み **/
							(void)sscanf(pStart + 1, "%f", &fRadius);
						}
						else if (strstr(aStr, READ_HEIGHT) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** 当たり判定の高さの読み込み **/
							(void)sscanf(pStart + 1, "%f", &fHeight);
						}
						else if (strstr(aStr, READ_MOVE) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** 移動量の読み込み **/
							(void)sscanf(pStart + 1, "%f", &fMove);
						}
						else if (strstr(aStr, READ_JUMP) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** 跳躍力の読み込み **/
							(void)sscanf(pStart + 1, "%f", &fJump);
						}
						else if (strstr(aStr, READ_PARTSSET) != NULL)
						{
							while (1)
							{
								memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
								(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

								if (feof(pFile) != 0)
								{
									MyMathUtil::GenerateMessageBox(MB_ICONERROR,
										"Error (4)",
										"END_PARTSSET無しに読み込みが異常終了しました。");

									/*** 読み込み終了 ***/
									fclose(pFile);

									return false;				// もし取得後EOFの場合、読み込み終了
								}

								if (DeleteComments(&aStr[0]))
								{
									if (strstr(aStr, READ_ENDPARTSSET) != NULL)
									{
										// パーツ情報を渡す

										nParts++;
										pPInfo->nNumParts++;
										break;
									}
									else if (strstr(aStr, READ_INDEX) != NULL)
									{
										int nIdxModel;

										pStart = strchr(aStr, '=');

										/** モデルインデックスの読み込み **/
										(void)sscanf(pStart + 1, "%d",
											&nIdxModel);

										/** モデルインデックスに対応した、オブジェクトインデックスの取得 **/
										pPInfo->aParts[nParts].nIdxModel = aIdxObject[nIdxModel];
									}
									else if (strstr(aStr, READ_PARENT) != NULL)
									{
										pStart = strchr(aStr, '=');

										/** 親の読み込み **/
										(void)sscanf(pStart + 1, "%d",
											&pPInfo->aParts[nParts].nIdxModelParent);
									}
									else if (strstr(aStr, READ_POS) != NULL)
									{
										pStart = strchr(aStr, '=');

										/** オフセットの読み込み **/
										(void)sscanf(pStart + 1, "%f %f %f",
											&pPInfo->aParts[nParts].pos.x,
											&pPInfo->aParts[nParts].pos.y,
											&pPInfo->aParts[nParts].pos.z);
									}
									else if (strstr(aStr, READ_ROT) != NULL)
									{
										pStart = strchr(aStr, '=');

										/** 角度の読み込み **/
										(void)sscanf(pStart + 1, "%f %f %f",
											&pPInfo->aParts[nParts].rot.x,
											&pPInfo->aParts[nParts].rot.y,
											&pPInfo->aParts[nParts].rot.z);
									}
								}
							}
						}

					}
				}
			}
			else if (strstr(aStr, READ_MOTIONSET) != NULL)
			{ // モーションの読み込み
				while (1)
				{
					memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
					(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

					if (feof(pFile) != 0)
					{
						MyMathUtil::GenerateMessageBox(MB_ICONERROR,
							"Error (5)",
							"END_MOTIONSET無しに読み込みが異常終了しました。");

						/*** 読み込み終了 ***/
						fclose(pFile);

						return false;				// もし取得後EOFの場合、読み込み終了
					}

					if (DeleteComments(&aStr[0]))
					{
						if (strstr(aStr, READ_ENDMOTIONSET) != NULL)
						{
							nKeySet = 0;
							nType = mType;
							nType++;
							if (nType >= MOTIONTYPE_MAX)
							{
								nType = MOTIONTYPE_MAX - 1;
							}

							mType = (MOTIONTYPE)nType;
							pMInfo->nNumMotion++;

							break;
						}
						else if (strstr(aStr, READ_LOOP) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** ループの読み込み **/
							(void)sscanf(pStart + 1, "%d", &nLoop);

							bLoop = (nLoop > 0) ? true : false;

							pMInfo->aMotionInfo[mType].bLoop = bLoop;
						}
						else if (strstr(aStr, READ_NUMKEY) != NULL)
						{
							pStart = strchr(aStr, '=');

							/** 移動量の読み込み **/
							(void)sscanf(pStart + 1, "%d", &pMInfo->aMotionInfo[mType].nNumKey);
						}
						else if (strcmp(aStr, READ_KEYSET) == NULL)
						{
							while (1)
							{
								memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
								(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

								if (feof(pFile) != 0)
								{
									MyMathUtil::GenerateMessageBox(MB_ICONERROR,
										"Error (6)",
										"END_KEYSET無しに読み込みが異常終了しました。");

									/*** 読み込み終了 ***/
									fclose(pFile);

									return false;				// もし取得後EOFの場合、読み込み終了
								}

								if (DeleteComments(&aStr[0]))
								{
									if (strstr(aStr, READ_ENDKEYSET) != NULL)
									{
										nKeySet++;
										nKey = 0;
										break;
									}
									else if (strstr(aStr, READ_FRAME) != NULL)
									{
										pStart = strchr(aStr, '=');

										/** 移動量の読み込み **/
										(void)sscanf(pStart + 1, "%d", &pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].nFrame);
									}
									else if (strcmp(aStr, READ_KEY) == NULL)
									{
										while (1)
										{
											memset(aStr, NULL, sizeof(aStr));					// 文字列を初期化
											(void)fgets(aStr, sizeof(aStr), pFile);				// メモ帳から一列取得

											if (feof(pFile) != 0)
											{
												MyMathUtil::GenerateMessageBox(MB_ICONERROR,
													"Error (7)",
													"END_KEY無しに読み込みが異常終了しました。");

												/*** 読み込み終了 ***/
												fclose(pFile);

												return false;				// もし取得後EOFの場合、読み込み終了
											}

											if (DeleteComments(&aStr[0]))
											{
												if (strstr(aStr, READ_ENDKEY) != NULL)
												{
													nKey++;
													break;
												}
												else if (strstr(aStr, READ_POS) != NULL)
												{
													pStart = strchr(aStr, '=');

													/** 位置の読み込み **/
													(void)sscanf(pStart + 1, "%f %f %f",
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].pos.x,
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].pos.y,
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].pos.z);
												}
												else if (strstr(aStr, READ_ROT) != NULL)
												{
													pStart = strchr(aStr, '=');

													/** 角度の読み込み **/
													(void)sscanf(pStart + 1, "%f %f %f",
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].rot.x,
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].rot.y,
														&pMInfo->aMotionInfo[mType].aKeyInfo[nKeySet].aKey[nKey].rot.z);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*** 読み込み終了 ***/
	fclose(pFile);

	/*** 読み込み成功 ***/
	return true;
}

//==================================================================================
// --- コメントアウト消去処理 ---
//==================================================================================
bool DeleteComments(char* pStr)
{
	char* PosTrash;					// ゴミ捨て場(コメント消去用変数)

	/*** コメント消去処理 ***/
	if (CMP_FAILED(strncmp(pStr, "#", 1)) && CMP_FAILED(strncmp(pStr, "\n", 1)))
	{ // 取得後、最初の文字が#(コメントアウト宣言)だった場合、読み込まない
		PosTrash = strstr(pStr, "\n");					// 改行がないか確認
		if (PosTrash != NULL) strcpy(PosTrash, "");		// あれば消去

		PosTrash = strstr(pStr, "#");					// 列の途中に#がないか確認
		if (PosTrash != NULL) strcpy(PosTrash, "");		// あれば、それ以降の文字列を消去

		PosTrash = strstr(pStr, "\t");					// タブスペースがないか確認
		while (PosTrash != NULL)
		{
			if (PosTrash != NULL)
			{
				if (PosTrash[0] == pStr[0])
				{
					strcpy(pStr, &pStr[1]);
					PosTrash = &PosTrash[1];				// あれば消去
				}
				else
				{
					strncpy(PosTrash, "", sizeof(char));	// あれば消去
				}
			}

			PosTrash = strstr(pStr, "\t");					// タブスペースがないか確認
		}

		/*** 読み込み開始 ***/
		return true;
	}
	else
	{
		/*** 読み込み終了***/
		return false;
	}
}