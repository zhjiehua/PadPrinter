
#ifndef __MANAGEMENT_H_
#define __MANAGEMENT_H_

//记录气缸位置用
#define UPDOWN_MASK         0x01
#define FRONTBACK_MASK      0x02

void Key_Refresh(void);
void ProgramResetDefault(void);
void ProgramSelf(void);
void ProgramSelect(void);

void ProgramRead(void);
void Key_Distinguish(void);

void NextAction(void);

void Home_Check(void);
void Push_Check(void);

void Project_Run(void);

#endif