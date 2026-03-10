//================================================================================================================
//
// DirectXのパーティクル処理 [particle.h]
// Author : KOMATSU SHOTA
//
//================================================================================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//**********************************************************************************
//*** インクルードファイル ***
//**********************************************************************************
#include "main.h"

//**********************************************************************************
//*** プロトタイプ宣言 ***
//**********************************************************************************
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);

void SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, D3DXVECTOR3 vecMin, D3DXVECTOR3 vecMax, int nRadius, float fRadiusEffect, int nLife, int nFrameEffect, bool bUseGravity, bool bVisible = false);
#endif