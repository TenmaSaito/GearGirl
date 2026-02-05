//=========================================================
// 
// "UIアーム処理"	[UIarm.h]
// Author : KikuchiMina
// 
//=========================================================
#ifndef _UIARM_H_		// このマクロ定義がされてなかったら
#define _UIARM_H_		// 2重インクルード防止のマクロ定義

//プロトタイプ宣言
void InitUIarm(void);		// UIアームの初期化処理
void UninitUIarm(void);		// UIアームの終了処理
void UpdateUIarm(void);		// UIアームの更新処理
void DrawUIarm(void);		// UIアームの描画処理

#endif // !_UIARM_H_