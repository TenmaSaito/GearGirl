//==================================================================================
//
// 自作ライブラリヘッダファイル [MyLib.h]
// Author : TENMA
//
//==================================================================================
#ifndef _MYLIB_H_		// インクルードガード
#define _MYLIB_H_		// インクルードガード
#pragma once

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include <Windows.h>
#include <initializer_list>

// stringのヘッダを読み込んでいない場合
#ifndef _INC_STRING
#if __has_include(<string.h>)
#include <string.h>
#else
#error "You can not use this header."			// string使用不可時、エラー
#endif
#endif

//==================================================================================
// 
// --- 自作ベクター ---
// 
//==================================================================================
namespace MyLib
{
	//==================================================================================
	// --- キャパシティチェック関数 ---
	//==================================================================================
	inline bool IsCapacityOver(UINT num, UINT capasity)
	{
		return (num >= capasity) ? true : false;
	}

	//==================================================================================
	// --- c言語自作vectorクラス ---
	//==================================================================================
	template<class T>
	class CVector
	{
		//------------------------------------------------------------
		// --- 型宣言 ---
		//------------------------------------------------------------
		using ElemType = T;

		//------------------------------------------------------------
		// --- 変数宣言 ---
		//------------------------------------------------------------
		ElemType* pHead;		// 確保したコンテナの先頭ポインタ
		UINT vectorNum;			// 使用中の配列数
		UINT vectorCapacity;	// メモリ予約済みの配列数

		//------------------------------------------------------------
		// --- メモリ容量の拡張 ---
		//------------------------------------------------------------
		bool GetCapacity(void)
		{
			// キャパシティを計算(新規なら1, 既にある場合は2倍確保)
			UINT newVectorCapacity = (vectorCapacity == 0) ? 1 : (vectorCapacity * 2);

			// メモリをキャパシティ分新規で確保(拡張)
			ElemType* pMem = (ElemType*)realloc(pHead, sizeof(ElemType) * newVectorCapacity);
			if (pMem == nullptr) return false;		// 確保失敗時はfalse

			/*** 成功時 ***/
			pHead = pMem;							// 確保したメモリポインタを割り当て
			vectorCapacity = newVectorCapacity;		// キャパシティを更新
			return true;
		}

	public:
		//------------------------------------------------------------
		// --- コンストラクタ --- 
		//------------------------------------------------------------
		// 通常コンストラクタ
		CVector(void) : pHead(nullptr), vectorNum(0), vectorCapacity(0) {}

		// 要素指定コンストラクタ
		CVector(const ElemType& Elem) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			Add(Elem);
		}

		// 配列要素指定コンストラクタ
		template<size_t N>
		CVector(const ElemType(&Elem)[N]) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			AddArray(Elem);
		}

		// 配列要素初期化コンストラクタ
		CVector(std::initializer_list<ElemType> init) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			// サイズ0の場合終了
			if (init.size() == 0) return;

			// サイズを取得
			vectorCapacity = static_cast<UINT>(init.size());
			vectorNum = static_cast<UINT>(init.size());

			// メモリを新規で確保
			pHead = (ElemType*)malloc(sizeof(ElemType) * vectorCapacity);
			if (pHead == nullptr)
			{ // 失敗時は各サイズをリセット
				vectorCapacity = 0;
				vectorNum = 0;
				return;
			}

			// コンテナの値を格納
			memcpy(pHead, init.begin(), GetCapacitySize());
		}

		// コピーコンストラクタ
		CVector(const CVector& Copy) : pHead(nullptr), vectorNum(0), vectorCapacity(0)
		{
			if (Copy.vectorNum > 0 && Copy.vectorCapacity > 0)
			{ // 使用中の配列数が1以上なら
				// メモリを確保
				pHead = (ElemType*)malloc(sizeof(ElemType) * Copy.vectorCapacity);
				if (pHead != nullptr)
				{ // 確保成功時
					// 値を格納
					memcpy(pHead, Copy.pHead, sizeof(ElemType) * Copy.vectorNum);
					vectorCapacity = Copy.vectorCapacity;	// キャパシティをコピー
					vectorNum = Copy.vectorNum;				// 使用中の配列数をコピー
				}
			}
		}

		//------------------------------------------------------------
		// --- デストラクタ --- 
		//------------------------------------------------------------
		~CVector(void)
		{
			if (pHead)
			{ // NULLではない場合
				free(pHead);		// メモリを解放
				pHead = nullptr;	// ポインタをnullに
				vectorNum = 0;		// 使用中の配列数をリセット
				vectorCapacity = 0;	// キャパシティをリセット
			}
		}

		//------------------------------------------------------------
		// --- 先頭アドレス取得 --- 
		//------------------------------------------------------------
		ElemType* GetHead(void)
		{
			return pHead;
		}

		//------------------------------------------------------------
		// --- 最後尾に追加 --- 
		//------------------------------------------------------------
		bool Add(const ElemType& Elem)
		{
			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			pHead[vectorNum] = Elem;	// 最後尾に値を格納
			vectorNum++;				// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 先端に追加 --- 
		//------------------------------------------------------------
		bool AddHead(const ElemType& Elem)
		{
			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			memmove(&pHead[1], &pHead[0], GetVectorSize());		// 先頭から1ずらした場所にデータを移動
			pHead[0] = Elem;		// 先頭に値を格納
			vectorNum++;			// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 指定した番目に追加 --- 
		//------------------------------------------------------------
		bool AddMiddle(const ElemType& Elem, UINT nIdx)
		{
			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return Add(Elem);
			if (nIdx == 0) return AddHead(Elem);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum, vectorCapacity))
			{ // キャパシティオーバーの場合
				// メモリを拡張
				if (!GetCapacity()) return false;
			}

			/*** 成功時 ***/
			memmove(&pHead[nIdx + 1], &pHead[nIdx], GetDesignationVectorSize(nIdx));	// メモリをずらす
			pHead[nIdx] = Elem;		// 指定位置に値を格納
			vectorNum++;			// 使用中の配列数を増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を最後尾に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArray(const ElemType(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memcpy(&pHead[vectorNum], ElemArray, sizeof(ElemType) * N);	// 配列サイズ分だけ最後尾に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を先端に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArrayHead(const ElemType(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memmove(&pHead[N], &pHead[0], GetVectorSize());		// 先頭から配列サイズ分ずらす
			memcpy(&pHead[0], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ先頭に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 配列を特定の場所に追加 --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddArrayMiddle(const ElemType(&ElemArray)[N], UINT nIdx)
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return AddArray(ElemArray);
			if (nIdx == 0) return AddArrayHead(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + N, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + N, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			// 指定位置から配列サイズ分ずらす
			memmove(&pHead[nIdx + N], &pHead[nIdx], GetDesignationVectorSize(nIdx));
			memcpy(&pHead[nIdx], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += N;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 最後尾への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddString(const char(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memcpy(&pHead[vectorNum - 1], ElemArray, sizeof(ElemType) * N);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 先頭への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddStringHead(const char(&ElemArray)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			memmove(&pHead[len], &pHead[0], GetVectorSize());			// 先頭から配列サイズ分ずらす
			memcpy(&pHead[0], ElemArray, sizeof(ElemType) * len);				// 配列サイズ分だけ先頭に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- 特定の配列への文字列の追加(char型限定) --- 
		//------------------------------------------------------------
		template<size_t N>
		bool AddStringMiddle(const char(&ElemArray)[N], UINT nIdx)
		{
			// 配列数が0の場合失敗
			if (N <= 0) return false;

			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return AddString(ElemArray);
			if (nIdx == 0) return AddStringHead(ElemArray);

			if (sizeof(ElemType) != sizeof(char)) return false;
			size_t len = strlen(ElemArray);

			// キャパシティチェック
			if (IsCapacityOver(vectorNum + len, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(vectorNum + len, vectorCapacity)) break;
				}
			}

			/*** 成功時 ***/
			// 指定位置から配列サイズ分ずらす
			memmove(&pHead[nIdx + len], &pHead[nIdx], GetDesignationVectorSize(nIdx));
			memcpy(&pHead[nIdx], ElemArray, sizeof(ElemType) * len);		// 配列サイズ分だけ指定位置に値を格納
			vectorNum += len;		// 使用中の配列数を配列サイズ分増加
			return true;
		}

		//------------------------------------------------------------
		// --- ポインタから追加 --- 
		//------------------------------------------------------------
		bool AddPointer(const ElemType *pElem, UINT count)
		{
			if (pElem && count > 0)
			{
				// キャパシティチェック
				if (IsCapacityOver(vectorNum + count, vectorCapacity))
				{// キャパシティオーバーの場合
					while (1)
					{
						// メモリを拡張
						if (!GetCapacity()) return false;

						// 再度確認し、オーバーなら再度拡張
						if (!IsCapacityOver(vectorNum + count, vectorCapacity)) break;
					}
				}

				/*** 成功時 ***/
				memcpy(&pHead[vectorNum], pElem, sizeof(ElemType) * count);	// 配列サイズ分だけ最後尾に値を格納
				vectorNum += count;		// 使用中の配列数を配列サイズ分増加
				return true;
			}
			else
			{
				return false;
			}
		}

		//------------------------------------------------------------
		// --- ポインタから先頭に追加 --- 
		//------------------------------------------------------------
		bool AddPointerHead(const ElemType *pElem, UINT count)
		{
			if (pElem && count > 0)
			{
				// キャパシティチェック
				if (IsCapacityOver(vectorNum + count, vectorCapacity))
				{// キャパシティオーバーの場合
					while (1)
					{
						// メモリを拡張
						if (!GetCapacity()) return false;

						// 再度確認し、オーバーなら再度拡張
						if (!IsCapacityOver(vectorNum + count, vectorCapacity)) break;
					}
				}

				/*** 成功時 ***/
				memmove(&pHead[count], &pHead[0], GetVectorSize());			// 先頭から配列サイズ分ずらす
				memcpy(&pHead[vectorNum], pElem, sizeof(ElemType) * count);	// 配列サイズ分だけ最後尾に値を格納
				vectorNum += count;		// 使用中の配列数を配列サイズ分増加
				return true;
			}
			else
			{
				return false;
			}
		}

		//------------------------------------------------------------
		// --- ポインタから先頭に追加 --- 
		//------------------------------------------------------------
		bool AddPointerMiddle(const ElemType* pElem, UINT count, UINT nIdx)
		{
			// 指定位置が先頭もしくは最後尾だった場合、各関数呼び出し
			if (nIdx >= vectorNum) return AddPointer(pElem, count);
			if (nIdx == 0) return AddPointerHead(pElem, count);

			if (pElem && count > 0)
			{
				// キャパシティチェック
				if (IsCapacityOver(vectorNum + count, vectorCapacity))
				{// キャパシティオーバーの場合
					while (1)
					{
						// メモリを拡張
						if (!GetCapacity()) return false;

						// 再度確認し、オーバーなら再度拡張
						if (!IsCapacityOver(vectorNum + count, vectorCapacity)) break;
					}
				}

				/*** 成功時 ***/
				// 指定位置から配列サイズ分ずらす
				memmove(&pHead[nIdx + count], &pHead[nIdx], GetDesignationVectorSize(nIdx));
				memcpy(&pHead[nIdx], pElem, sizeof(ElemType) * count);		// 配列サイズ分だけ指定位置に値を格納
				vectorNum += count;		// 使用中の配列数を配列サイズ分増加
				return true;
			}
			else
			{
				return false;
			}
		}

		//------------------------------------------------------------
		// --- サイズを変更 --- 
		//------------------------------------------------------------
		bool Reserve(UINT num)
		{
			// サイズチェック
			if (num == 0)
			{ // 0の場合
				if (pHead)
				{ // メモリを解放
					free(pHead);
					pHead = nullptr;
					vectorNum = 0;
					vectorCapacity = 0;
					return true;
				}
				else
				{ // 解放済み
					return false;
				}
			}
			else
			{ // 0以上の場合
				// メモリを指定サイズ分確保
				ElemType* pMem = (ElemType*)realloc(pHead, sizeof(ElemType) * num);
				if (pMem)
				{ // 成功時
					pHead = pMem;			// ポインタを保存
					vectorNum = num;		// 使用中配列数を保存
					vectorCapacity = num;	// キャパシティを保存
					return true;
				}
				else
				{ // resize失敗
					return false;
				}
			}
		}

		//------------------------------------------------------------
		// --- 要素数を変更 --- 
		//------------------------------------------------------------
		bool Resize(UINT num)
		{
			if (num == vectorNum) return true;

			// キャパシティチェック
			if (IsCapacityOver(num, vectorCapacity))
			{// キャパシティオーバーの場合
				while (1)
				{
					// メモリを拡張
					if (!GetCapacity()) return false;

					// 再度確認し、オーバーなら再度拡張
					if (!IsCapacityOver(num, vectorCapacity)) break;
				}
			}

			int vectorNumOld = vectorNum;		// 過去の配列数を保存
			vectorNum = num;					// 配列数を更新

			// 元の配列数と比較
			if (num > vectorNumOld)
			{ // 多くなっていた場合
				Fill(vectorNum, 0, num);		// 多くなった分を0で初期化
			}

			return true;
		}

		//------------------------------------------------------------
		// --- 最後尾の値を取得 --- 
		//------------------------------------------------------------
		ElemType* GetEndElem(void)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			return &pHead[vectorNum - 1];
		}

		//------------------------------------------------------------
		// --- 最前列の値を取得 --- 
		//------------------------------------------------------------
		ElemType* GetHeadElem(void)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			return &pHead[0];
		}

		//------------------------------------------------------------
		// --- 特定の番号の値を取得 --- 
		//------------------------------------------------------------
		ElemType* GetElem(UINT nIdx)
		{
			// 使用中の配列がない場合NULL
			if (vectorNum == 0U) return NULL;

			UINT IDX = nIdx;
			if (IDX >= vectorNum) IDX = vectorNum - 1;

			return &pHead[IDX];
		}

		//------------------------------------------------------------
		// --- 最後尾を消去 --- 
		//------------------------------------------------------------
		UINT DeleteEnd(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			pHead[vectorNum - 1] = {};		// 値を消去
			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 先端を消去 --- 
		//------------------------------------------------------------
		UINT DeleteHead(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			memmove(&pHead[0], &pHead[1], GetDesignationVectorSize(1));		// メモリをずらす

			pHead[vectorNum - 1] = {};		// 値を消去
			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 特定の配列の値を消去 --- 
		//------------------------------------------------------------
		UINT Delete(UINT nIdx)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			memmove(&pHead[nIdx], &pHead[nIdx + 1], GetDesignationVectorSize(nIdx));		// 値を消去

			vectorNum--;					// 使用中の配列数を減少

			return vectorNum;				// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 特定の配列の範囲を消去 --- 
		//------------------------------------------------------------
		UINT Delete(UINT nIdx, UINT nIdxEnd)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return 0U;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return 0U;

			// 範囲指定が間違っている場合NULL
			if (nIdxEnd < nIdx) return 0U;

			// メモリを範囲で消去し、ずらす
			memmove(&pHead[nIdx], &pHead[nIdxEnd], GetDesignationVectorSize(nIdxEnd));

			vectorNum -= 1 + (nIdxEnd - nIdx);		// 使用中の配列数を減少

			return vectorNum;						// 使用中の配列数を返す
		}

		//------------------------------------------------------------
		// --- 全範囲を消去 --- 
		//------------------------------------------------------------
		void Clear(void)
		{
			// ポインタがない場合NULL
			if (pHead == nullptr) return;

			// 使用中の配列がない場合NULL
			if (vectorNum == 0) return;

			// 配列内を全て初期化
			memset(pHead, NULL, GetVectorSize());

			vectorNum = 0;		// 使用中の配列数を0に
		}

		//------------------------------------------------------------
		// --- 全範囲を特定の値で埋める --- 
		//------------------------------------------------------------
		void Fill(const ElemType& Elem)
		{
			// ポインタがない場合終了
			if (pHead == nullptr) return;

			// 使用中の配列がない場合終了
			if (vectorNum == 0) return;

			size_t size = GetVectorSize();

			// 配列内を全て特定の値で埋める
			for (UINT nCntFill = 0; nCntFill < vectorNum; nCntFill++)
			{
				pHead[nCntFill] = Elem;
			}
		}

		//------------------------------------------------------------
		// --- 特定の範囲を特定の値で埋める --- 
		//------------------------------------------------------------
		void Fill(UINT start, const ElemType& Elem, UINT end)
		{
			// ポインタがない場合終了
			if (pHead == nullptr) return;

			// 使用中の配列がない場合終了
			if (vectorNum == 0) return;

			// 開始位置が配列以上の場合終了
			if (start >= vectorNum) return;

			// 順番が逆転している場合終了
			if (start > end) return;

			UINT endIn = end;

			// 終点が配列以上の場合終了
			if (endIn > vectorNum) endIn = vectorNum;

			// 配列内を全て特定の値で埋める
			for (UINT nCntFill = start; nCntFill < endIn; nCntFill++)
			{
				pHead[nCntFill] = Elem;
			}
		}

		//------------------------------------------------------------
		// --- バッファを新規作成 --- 
		//------------------------------------------------------------
		void Start(const ElemType& Elem)
		{
			// 全要素のクリア
			Clear();

			// バッファの作成
			AddHead(Elem);
		}

		//------------------------------------------------------------
		// --- 配列バッファを新規作成 --- 
		//------------------------------------------------------------
		template<size_t N>
		void Start(const ElemType(&Elem)[N])
		{
			// 配列数が0の場合失敗
			if (N <= 0) return;

			// 全要素のクリア
			Clear();

			// バッファの作成
			AddArrayHead(Elem);
		}

		//------------------------------------------------------------
		// --- バッファを破棄 --- 
		//------------------------------------------------------------
		void End(void)
		{
			if (pHead != nullptr)
			{ // NULLではないなら
				free(pHead);		// メモリを解放
				pHead = nullptr;	// ポインタをnullに
				vectorNum = 0;		// 使用中の配列数をリセット
				vectorCapacity = 0;	// キャパシティをリセット
			}
		}

		//------------------------------------------------------------
		// --- 使用されている配列サイズの取得 --- 
		//------------------------------------------------------------
		size_t GetVectorSize(void)
		{
			return sizeof(ElemType) * vectorNum;
		}

		//-------------------------------------------------------------
		// --- 指定した配列番号を先頭とした使用中の配列サイズの取得 --- 
		//-------------------------------------------------------------
		size_t GetDesignationVectorSize(UINT nIdx)
		{
			UINT nGetIdx = nIdx;

			// idxが使用中の配列数よりも大きい場合、最大値に修正
			if (nGetIdx >= vectorNum) nGetIdx = vectorNum - 1;
			return GetVectorSize() - (sizeof(ElemType) * nGetIdx);
		}

		//------------------------------------------------------------
		// --- 予約済みの配列サイズの取得 --- 
		//------------------------------------------------------------
		size_t GetCapacitySize(void)
		{
			return sizeof(ElemType) * vectorCapacity;
		}

		//------------------------------------------------------------
		// --- 使用中の配列数の取得 --- 
		//------------------------------------------------------------
		UINT GetVectorNum(void)
		{
			return vectorNum;
		}

		//------------------------------------------------------------
		// --- 予約済みの配列数の取得 --- 
		//------------------------------------------------------------
		UINT GetCapacityNum(void)
		{
			return vectorCapacity;
		}

		//------------------------------------------------------------
		// --- 四則演算 --- 
		//------------------------------------------------------------
		CVector& operator += (const CVector& Elem)
		{
			if (Elem.vectorNum != vectorNum) return *this;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] += Elem.pHead[nCntElem];
			}

			return *this;
		}

		CVector& operator -= (const CVector& Elem)
		{
			if (Elem.vectorNum != vectorNum) return *this;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] -= Elem.pHead[nCntElem];
			}

			return *this;
		}

		CVector& operator *= (const float& Elem)
		{
			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] *= Elem;
			}

			return *this;
		}

		CVector& operator /= (const float& Elem)
		{
			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				pHead[nCntElem] /= Elem;
			}

			return *this;
		}

		//------------------------------------------------------------
		// --- 正判定 --- 
		//------------------------------------------------------------
		bool operator == (const CVector& Elem) const
		{
			if (Elem.vectorNum != vectorNum) return false;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				if (pHead[nCntElem] != Elem.pHead[nCntElem])
				{
					return false;
				}
			}

			return true;
		}

		//------------------------------------------------------------
		// --- 誤判定 --- 
		//------------------------------------------------------------
		bool operator != (const CVector& Elem) const
		{
			if (Elem.vectorNum != vectorNum) return true;

			for (UINT nCntElem = 0; nCntElem < vectorNum; nCntElem++)
			{
				if (pHead[nCntElem] != Elem.pHead[nCntElem])
				{
					return true;
				}
			}

			return false;
		}

		//------------------------------------------------------------
		// --- コピー演算子 --- 
		//------------------------------------------------------------
		CVector& operator = (const CVector& Copy)
		{
			// コピー元が自分自身の場合
			if (this == &Copy) return *this;

			// メモリ確保済みの場合
			if (pHead != nullptr)
			{ // メモリを解放
				End();
			}

			if (Copy.vectorNum > 0 && Copy.vectorCapacity > 0)
			{ // 使用中の配列数が1以上なら
				// メモリを確保
				pHead = (ElemType*)malloc(sizeof(ElemType) * Copy.vectorCapacity);
				if (pHead != nullptr)
				{ // 確保成功時
					// 値を格納
					memcpy(pHead, Copy.pHead, sizeof(ElemType) * Copy.vectorNum);
					vectorCapacity = Copy.vectorCapacity;	// キャパシティをコピー
					vectorNum = Copy.vectorNum;				// 使用中の配列数をコピー
				}
			}

			return *this;
		}
	};

	// 省略
	using String = CVector<char>;
	using WString = CVector<wchar_t>;
	using Integer = CVector<int>;
	using Floating = CVector<float>;
}
#endif