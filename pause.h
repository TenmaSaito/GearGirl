//=========================================================
// 
// "ポーズ処理"	[pause.h]
// Author : KikuchiMina
// 
//=========================================================
#ifndef _PAUSE_H_				//このマクロ定義がされてなかったら
#define _PAUSE_H_				//2重インクルード防止のマクロ定義

//ポーズメニュー
typedef enum
{
	PAUSE_MENU_RESTART = 0,		//最初から
	PAUSE_MENU_RETITLE,			//タイトルに戻る
	PAUSE_MENU_CLOSE,			//閉じる
	PAUSE_MENU_MAX
}PAUSE_MENU;

//プロトタイプ宣言
void InitPause(void);			//ポーズの初期化処理
void UninitPause(void);			//ポーズの終了処理
void UpdatePause();				//ポーズの更新処理
void DrawPause(void);			//ポーズの描画処理
void UpdateMovePause(void);		//ポーズの移動処理

#endif // !_PAUSE_H_