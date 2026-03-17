//=============================================================================
//
// サウンド処理 [sound.h]
// Author : TENMA SAITO (SPACIAL THANKS : AKIRA TANAKA)
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

// マクロ定義
#define SOUND_BGMSTART		(SOUND_LABEL_BGM_TITLE)		// BGMの開始位置
#define SOUND_SESTART		(SOUND_LABEL_SE_GETPARTS)	// SEの開始位置
#define SOUND_STANDARD		(0.8f)						// 初期音量

#ifndef NDEBUG
#undef SOUND_STANDARD
#define SOUND_STANDARD		(0.0f)						// 初期音量
#endif

#define _3DAUDIO
#define SOUND_SPD			D3DXVECTOR3(1, 1, 1)		// 音速度

//*****************************************************************************
// サウンド一覧 (sound.cppのsoundinfoにも追加する！)
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM_TITLE = 0,		// タイトル画面のBGM
	SOUND_LABEL_BGM_GAME,			// ゲーム画面のBGM
	SOUND_LABEL_BGM_BADEND,			// バッドエンド画面のBGM
	SOUND_LABEL_BGM_NORMALEND,		// 通常エンド画面のBGM
	SOUND_LABEL_BGM_SUPERHAPPYEND,	// スーパーハッピーエンド画面のBGM
	SOUND_LABEL_SE_GETPARTS,		// パーツ取得音
	SOUND_LABEL_SE_T_SELECT,		// タイトル画面での選択音
	SOUND_LABEL_SE_T_ENTER,			// タイトル画面での決定音
	SOUND_LABEL_SE_T_CANCEL,		// タイトル画面での取消音
	SOUND_LABEL_SE_G_MOVE,			// 移動音
	SOUND_LABEL_SE_G_MOVEDASH,		// 移動音(ダッシュ時)
	SOUND_LABEL_SE_G_MOUSEMOVE,		// マウスの移動音
	SOUND_LABEL_SE_G_JUMP,			// 跳躍音
	SOUND_LABEL_SE_G_MOUSEJUMP,		// マウスの跳躍音
	SOUND_LABEL_SE_G_LANDING,		// 着地音
	SOUND_LABEL_SE_G_MOUSELANDING,	// マウスの着地音
	SOUND_LABEL_SE_G_THROW,			// 投擲音
	SOUND_LABEL_SE_G_THROWPOINT,	// 射出方向切り替え音
	SOUND_LABEL_SE_G_ARMSWITCH,		// アーム切り替え音
	SOUND_LABEL_SE_G_DASH,			// ダッシュ時の風の音
	SOUND_LABEL_SE_G_FALLENTREE,	// 木を切った後の消滅音
	SOUND_LABEL_SE_G_DIAROG,		// ダイアログ表示の音
	SOUND_LABEL_SE_G_CHAINSAW,		// チェンソーの音
	SOUND_LABEL_SE_G_VALVE,			// バルブが回転する音
	SOUND_LABEL_SE_G_FOUNTAIN,		// 噴水の音
	SOUND_LABEL_SE_G_CANCEL,		// キャンセル音
	SOUND_LABEL_SE_G_GETITEM,		// アイテム取得時の音
	SOUND_LABEL_SE_G_TUTORIAL,		// チュートリアル表示の音
	SOUND_LABEL_SE_G_CHARACHANGE,	// 操作対象切り替えの音
	SOUND_LABEL_SE_G_OPENDOOR,		// ドアが開く音
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// サウンドの種類
//*****************************************************************************
typedef enum
{
	SETSOUND_GAME = 0,			// ゲーム内音声
	SETSOUND_MENU_SE,			// システム音声
	SETSOUND_MAX
}SETSOUND;

//*****************************************************************************
// サウンドチェック構造体
//*****************************************************************************
STRUCT(SoundChecker) PARENT(IXAudio2VoiceCallback)
{
private:
	bool m_bPlay, m_bSubmit;
	HRESULT m_hrLastError;

	// バッファ―処理終了時
	void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) override
	{
		OutputDebugString(TEXT("バッファー処理が終了したよ！\n"));

		// 必要に応じてvoid*をキャストして使用
		if (pBufferContext != nullptr)
		{ // ポインタを使用した処理

		}

		m_bSubmit = false;
		m_bPlay = false;
	}

	// バッファ―処理開始時
	void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override
	{
		OutputDebugString(TEXT("バッファー処理スタート！\n"));

		// 必要に応じてvoid*をキャストして使用
		if (pBufferContext != nullptr)
		{ // ポインタを使用した処理

		}

		m_bSubmit = true;
		m_bPlay = true;
	}

	// ループ終了時
	void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override
	{
		// 必要に応じてvoid*をキャストして使用
		if (pBufferContext != nullptr)
		{ // ポインタを使用した処理

		}
	}

	// 再生終了時
	void STDMETHODCALLTYPE OnStreamEnd(void) override
	{
		OutputDebugString(TEXT("音源の再生が終了したよ！\n"));

		m_bPlay = false;
	}

	// エラー発生時
	void STDMETHODCALLTYPE OnVoiceError(
		void* pBufferContext,
		HRESULT Error
	) override
	{
		OutputDebugString(TEXT("ひぃん......エラー発生......\n"));

		// 必要に応じてvoid*をキャストして使用
		if (pBufferContext != nullptr)
		{ // ポインタを使用した処理

		}

		m_hrLastError = Error;
	}

	// 音声の処理パス終了直後
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd(void) override
	{

	}

	// データを読み取る直前
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(
		UINT32 BytesRequired
	) override
	{

	}

	friend IXAudio2SourceVoice;
	friend IXAudio2MasteringVoice;
	friend IXAudio2;

public:
	// コンストラクタ
	SoundChecker() : m_bPlay(false), m_bSubmit(false), m_hrLastError(S_OK) {}

	// デストラクタ
	~SoundChecker() {}

	// サウンドの再生状況を取得
	bool GetPlayingSound(void)
	{
		return m_bPlay;
	}

	// サウンドが登録されているか
	bool GetSubmitSound(void)
	{
		return m_bSubmit;
	}

	// 最後に発生したエラーを取得
	HRESULT GetLastError(void)
	{
		return m_hrLastError;
	}

	// サウンドの停止
	void StopSound(void)
	{
		m_bPlay = false;
	}
} SoundChecker;

typedef SoundChecker *PSOUNDCHECKER, *LPSOUNDCHECKER;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

// 追加プロトタイプ宣言
bool GetSound(SOUND_LABEL label);
void SetVolume(float volume, SETSOUND sound);
float GetVolume(SETSOUND sound);
void FadeSound(SOUND_LABEL label);
SOUND_LABEL GetPlaySound(void);
void UpdateSound(void);
void Play3DSound(SOUND_LABEL label, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 posU);

bool IsPlayingSound(SOUND_LABEL label);
#endif
