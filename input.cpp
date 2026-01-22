//=========================================================
// 
// "入力処理"	[input.cpp]
// Author : KikuchiMina
// 
//=========================================================
#include "main.h"
#include "input.h"

//=========================================================
//マクロ定義
//=========================================================

//=========================================================
//グローバル変数
//=========================================================
LPDIRECTINPUT8			g_pInput = NULL;			//DirectInputオブジェクトのポインタ
LPDIRECTINPUTDEVICE8	g_pDevKeyboard = NULL;			//入力デバイス(キーボード)へのポインタ
BYTE					g_aKeyState[NUM_KEY_MAX];			//キーボードのプレス情報
BYTE					g_aKeyStateTrigger[NUM_KEY_MAX];	//キーボードのトリガー情報
int 					g_nKeyStateRepeat[NUM_KEY_MAX] = {};//キーボードのリピート情報
BYTE 					g_aKeyStateRelease[NUM_KEY_MAX];	//キーボードのリリース情報

XINPUT_PAD				g_Joypad[MAX_PLAYER];				//パッドの情報
int						g_nTime;
int						g_nPlayer;

//=========================================================================================================
//キーボードの初期化処理
//=========================================================================================================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	//DirectInputオブジェクトの作成
	if (FAILED(DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&g_pInput,
		NULL)))
	{
		return E_FAIL;
	}

	//入力デバイス(キーボード)の設定
	if (FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマットを設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//協調モードを設定
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権を取得
	g_pDevKeyboard->Acquire();

	return S_OK;
}

//=========================================================
//キーボードの終了処理
//=========================================================
void UninitKeyboard(void)
{
	// 入力デバイス(キーボード)の破棄
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}

	// DirectInputオブジェクトの破棄
	if (g_pInput != NULL)
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}

//=========================================================
//キーボードの更新処理
//=========================================================
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];			// キーボードの入力情報
	int nCntKey;
	//g_bUseKeyAny = false;

	// 入力デバイスからデータを取得
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			g_aKeyStateRelease[nCntKey] = (g_aKeyState[nCntKey] & (g_aKeyState[nCntKey] ^ aKeyState[nCntKey]));	// キーボードのリリース情報を保存
			g_aKeyStateTrigger[nCntKey] = ((aKeyState[nCntKey] ^ g_aKeyState[nCntKey]) & aKeyState[nCntKey]);	// キーボードのトリガー情報を保存
			g_aKeyState[nCntKey] = aKeyState[nCntKey];															// キーボードのプレス情報を保存
			
			if (GetKeyboardPress(nCntKey))		//キーボードのリピート情報を保存
			{
				g_nKeyStateRepeat[nCntKey]++;
			}
			else
			{
				g_nKeyStateRepeat[nCntKey] = 0;
			}
		}			
	}
	else
	{
		g_pDevKeyboard->Acquire();			// キーボードへのアクセス権を取得
	}
}                                              

//=========================================================
// キーボードのプレス情報を取得
//=========================================================
bool GetKeyboardPress(int nKey)
{
	// 3項演算子
	return (g_aKeyState[nKey] & 0x80) ? true : false;
}

//=========================================================
// キーボードのトリガー情報を取得
//=========================================================
bool GetKeyboardTrigger(int nKey)
{
	return (g_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=========================================================
// キーボードのリリース情報を取得
//=========================================================
bool GetKeyboardRelease(int nKey)
{
	return (g_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//=========================================================
// キーボードのリピート情報を取得
//=========================================================
bool GetKeyboardRepeat(int nKey)
{
	if (g_nKeyStateRepeat[nKey] == 1 || g_nKeyStateRepeat[nKey] % 10 == 0 && g_nKeyStateRepeat[nKey] >= 30)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//=========================================================================================================
// ジョイパッドの初期化処理
//=========================================================================================================
HRESULT InitJoypad(void)
{
	//メモリのクリア
	for (int nCntPad = 0; nCntPad < MAX_PLAYER; nCntPad++)
	{
		memset(&g_Joypad[nCntPad].joykeyState, 0, sizeof(XINPUT_STATE));
		memset(&g_Joypad[nCntPad].joykeyStateTrigger, 0, sizeof(XINPUT_STATE));
		memset(&g_Joypad[nCntPad].vibration, 0, sizeof(XINPUT_VIBRATION));
	}

	//XInputのステートを設定(有効にする)
	XInputEnable(true);

	//振動時間の初期化
	g_nTime = 10;
	g_nPlayer = 0;

	return S_OK;
}

//=========================================================
// ジョイパッドの終了処理
//=========================================================
void UninitJoypad(void)
{
	//XInputのステートを設定(無効にする)
	XInputEnable(false);
}

//=========================================================
// ジョイパッドの更新処理
//=========================================================
void UpdateJoypad(void)
{
	XINPUT_STATE joykeyState;							//ジョイパッドの入力情報

	for (int nCntPad = 0; nCntPad < MAX_PLAYER; nCntPad++)
	{
		//ジョイパッドの状態を取得
		if (XInputGetState(nCntPad, &joykeyState) == ERROR_SUCCESS)
		{
			//ジョイパッドのリリース情報を保存
			g_Joypad[nCntPad].joykeyStateRelease->Gamepad.wButtons = g_Joypad[nCntPad].joykeyState.Gamepad.wButtons & (joykeyState.Gamepad.wButtons ^ joykeyState.Gamepad.wButtons);

			//ジョイパッドのトリガー情報を保存
			g_Joypad[nCntPad].joykeyStateTrigger.Gamepad.wButtons = (g_Joypad[nCntPad].joykeyState.Gamepad.wButtons ^ joykeyState.Gamepad.wButtons) & joykeyState.Gamepad.wButtons;

			//ジョイパッドの情報を保存
			g_Joypad[nCntPad].joykeyState = joykeyState;

			for (int nCntJoy = 0; nCntJoy < NUM_JOY_MAX; nCntJoy++)
			{
				//ジョイパッドのリピート情報
				if (GetJoypadPress(nCntPad, (JOYKEY)nCntJoy))
				{
					g_Joypad[nCntPad].nJoykeyStateRepeat[nCntJoy]++;
				}
				else
				{
					g_Joypad[nCntPad].nJoykeyStateRepeat[nCntJoy] = 0;
				}
			}
		}
	}

	//振動時間の減り
	g_nTime--;

	if (g_nTime <= 0)
	{
		StopVibration(g_nPlayer);
		g_nTime = 0;
	}
}

//=========================================================
//ジョイパッドのプレス情報を取得
//=========================================================
bool GetJoypadPress(int nPlayer, JOYKEY key)
{
	return(g_Joypad[nPlayer].joykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=========================================================
//ジョイパッドのトリガー情報を取得
//=========================================================
bool GetJoypadTrigger(int nPlayer, JOYKEY key)
{
	return(g_Joypad[nPlayer].joykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=========================================================
// ジョイパッドのリリース情報を取得
//=========================================================
bool GetJoypadRelease(int nPlayer, JOYKEY Key)
{
	return (g_Joypad[nPlayer].joykeyStateRelease->Gamepad.wButtons & (0x01 << Key)) ? true : false;
}

//=========================================================
//ジョイパッドのリピート情報を取得
//=========================================================
bool GetJoypadRepeat(int nPlayer, JOYKEY key)
{
	if (g_Joypad[nPlayer].nJoykeyStateRepeat[key] == 1 ||
		g_Joypad[nPlayer].nJoykeyStateRepeat[key] % 10 == 0 &&
		g_Joypad[nPlayer].nJoykeyStateRepeat[key] >= 30)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//=========================================================
//ジョイパッドの左スティック情報を取得
//=========================================================
bool GetJoypadStickLeft(int nPlayer, JOYKEY key)
{

	if (JOYKEY_LEFT_STICK_LEFT == key)
	{
		return(g_Joypad[nPlayer].joykeyState.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else if (JOYKEY_LEFT_STICK_RIGHT == key)
	{
		return(g_Joypad[nPlayer].joykeyState.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (JOYKEY_LEFT_STICK_UP == key)
	{
		return(g_Joypad[nPlayer].joykeyState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else if (JOYKEY_LEFT_STICK_DOWN == key)
	{
		return(g_Joypad[nPlayer].joykeyState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else
	{
		return false;
	}
}

//=========================================================================================================
//振動の情報
//=========================================================================================================
XINPUT_VIBRATION* GetXInput(void)
{
	return &g_Joypad[0].vibration;					//振動情報の先頭アドレスを返す
}

//=========================================================
//振動をスタートする処理
//=========================================================
void SetVibration(int nPlayer, int nLeftMotor, int nRightMotor, int nTime)
{
	g_nTime = nTime;
	g_nPlayer = nPlayer;

	g_Joypad[nPlayer].vibration.wLeftMotorSpeed = nLeftMotor;		// use any value between 0-65535 here
	g_Joypad[nPlayer].vibration.wRightMotorSpeed = nRightMotor;	// use any value between 0-65535 here

	XInputSetState(0, &g_Joypad[nPlayer].vibration);
}

//=========================================================
//振動をストップする処理
//=========================================================
void StopVibration(int nPlayer)
{
	g_Joypad[nPlayer].vibration.wLeftMotorSpeed = 0;		// use any value between 0-65535 here
	g_Joypad[nPlayer].vibration.wRightMotorSpeed = 0;		// use any value between 0-65535 here

	XInputSetState(0, &g_Joypad[nPlayer].vibration);
}