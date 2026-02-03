#ifndef _TIMER_H_
#define _TIMER_H_

// プロトタイプ宣言
void InitTimer(bool bRanking);
void UninitTimer(void);
void UpdateTimer();
void DrawTimer(void);

void SetTimer(int nScore);
int GetTimer(void);
void AddTimer(int nValue);

#endif