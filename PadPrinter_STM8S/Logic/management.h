
#ifndef __MANAGEMENT_H_
#define __MANAGEMENT_H_

#define SEG_FLASH_PERIOD    (2000/2)
#define SEG_FLASH_TIMES     (2)

//记录气缸位置用
#define UPDOWN_MASK         0x01
#define FRONTBACK_MASK      0x02

void Key_Refresh(void);
void ProgramResetDefault(void);
void ProgramSelf(void);
void ProgramSelect(void);
void SegFlash(void);
void ProgramRead(void);
void KeyDistinguish(void);

void NextAction(void);
void AbsorbDown(void);
void AbsorbUp(void);
void Absorb(void);
void Front(void);
void Back(void);
void PrintDown(void);
void PrintUp(void);
void Print(void);
void Shift(void);
void Return(void);
void Push(void);
void Home(void);
void PrintHeadHome(void);

void Project_Run(void);

#endif