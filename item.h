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
#include "itemEffector.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_ITEM		(32)		// アイテム最大数
#define ITEM_RANK		(2)			// アイテムの等級段数
#define NUM_PUTOUTITEM	(5)			// 提出アイテム数
#define ITEM_RANGE		(15.0f)		// アイテム当たり判定範囲

#define SPIN_ITEM		(0.02f)		// アイテムの回転速度

//==============================================================
// 型定義
//==============================================================
typedef int IDX_ITEM;				// アイテムインデックス

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

typedef enum
{
	PUTOUT_BG = 0,					// 背景
	PUTOUT_ITEMBOX,					// 所持アイテムの大枠
	PUTOUT_ENTRYBOX,				// 提出アイテムの大枠
	PUTOUT_ENTER,					// 決定ボタン
	PUTOUTUI_MAX
}PUTOUTUI;

//==============================================================
// アイテム情報構造体定義
//==============================================================
typedef struct ItemInfo
{
	const char* pFile;				// ファイル名
	int			nNumGet;			// モデルを読み込めたかの判定が入る

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
	ITEMTYPE		type;				// アイテムの種類
	bool			bUse;				// 構造体を使用しているかどうか
	bool			bGet;				// プレイヤーが取得したかどうか
	bool			bCollision;			// 衝突するか(true)、取得するか(false)
	bool			bGirl;				// 少女に見えるか
	bool			bMouse;				// ネズミに見えるか
	int				nIdxModel;			// モデルデータのインデックス
	D3DXMATRIX		mtxWorld;			// ワールドマトリックス
	int				nIdxQuota;			// 表示用の枠のインデックス
	IDX_EFFECTOR	nIdxEffector;		// エフェクターのインデックス
	D3DXVECTOR3		Dist;				// プレイヤーとアイテムの距離計算用
	float			fDistance;			// 計算結果
	float			fCol;				// 距離による色の変化用
	int				nIdxMesh;			// メッシュシリンダーのインデックス保管用
}Item;
POINTER(Item, P_ITEM);

//=========================================================================================
//プロトタイプ宣言
//=========================================================================================
void InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);
void DrawUIItem(void);
IDX_ITEM SetItem(vec3 pos, vec3 rot, ITEMTYPE type, bool bReflectGirl = true, bool bReflectMouse = true, bool bColi = false);			// アイテム設置
void CollisionItem(vec3 pos, float fRange, int type);	// 当たり判定
void EnableItemPut(void);										// アイテム提出フラグを切り替え
bool IsEnableItemPut(void);
P_ITEM GetItem(void);				// 先頭アドレス取得
ITEMTYPE GetNumSelect(void);		// 提出時選択中のアイテム
#endif // !_ITEM_H_