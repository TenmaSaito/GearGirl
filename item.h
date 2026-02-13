//==================================================================================================================================
//
//			アイテム処理 [item.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _ITEM_H_
#define _ITEM_H_

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "modeldata.h"
#include "endomacro.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_ITEM		(16)		// アイテム最大数
#define ITEM_RANK		(2)			// アイテムの等級段数
#define ITEM_RANGE		(25.0f)		// アイテム当たり判定範囲

#define SPIN_ITEM		(0.02f)		// アイテムの回転速度

//==============================================================
// 列挙型
//==============================================================
typedef enum
{
	ITEMTYPE_SCREW_TRUE = 0,		// [0] 新品ののねじ
	ITEMTYPE_GEARS_TRUE,			// [1] 磨かれた小さな歯車
	ITEMTYPE_GEARL_TRUE,			// [2] 綺麗な大きい歯車
	ITEMTYPE_SHAFT_TRUE,			// [3] まっすぐな軸
	ITEMTYPE_SPRING_TRUE,			// [4] 丁寧に直されたぜんまい

	ITEMTYPE_SCREW_FALSE,			// [5] 古いねじ
	ITEMTYPE_GEARS_FALSE,			// [6] 錆びた小さい歯車
	ITEMTYPE_GEARL_FALSE,			// [7] 欠けた大きい歯車
	ITEMTYPE_SHAFT_FALSE,			// [8] 少しい曲がった軸
	ITEMTYPE_SPRING_FALSE,			// [9] ゆがんだぜんまい
	ITEMTYPE_MAX
}ITEMTYPE;

//==============================================================
// アイテム情報構造体定義
//==============================================================
typedef struct ItemInfo
{
	const char* pModelFile;
	int			nNumGet;

}ItemInfo;
POINTER(ItemInfo, P_ITEMINFO);

//==============================================================
// アイテム配置構造体定義
//==============================================================
typedef struct Item
{
	vec3			pos;
	vec3			rot;
	float			fRange;				// 当たり判定範囲（球）
	ITEMTYPE		type;
	bool			bUse;				// 構造体を使用しているかどうか
	bool			bGet;				// プレイヤーが取得したかどうか
	bool			bCollision;			// 衝突するか(true)、取得するか(false)
	bool			bGirl;				// 少女に見えるか
	bool			bMouse;				// ネズミに見えるか
	int				nIdxModel;			// モデルデータのインデックス
	D3DXMATRIX		mtxWorld;			// ワールドマトリックス
}Item;
POINTER(Item, P_ITEM);



//=========================================================================================
//プロトタイプ宣言
//=========================================================================================
void InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);
void CollisionItem(vec3 pos,float fRange);			// 当たり判定
bool IsGetItem(ITEMTYPE type);

#endif // !_ITEM_H_