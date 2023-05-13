#ifndef DSL_H
#define DSL_H

#include "translator.h"

#define WritePushReg(reg)   Operation push##reg = {PUSH_##reg, SIZE_PUSH_##reg};      \
                            WriteBinCmd(trans, &push##reg);

#define WritePopReg(reg)   Operation pop##reg = {POP_##reg, SIZE_POP_##reg};      \
                            WriteBinCmd(trans, &pop##reg);

#define WriteMovRegNum(reg, num)    Operation mov##reg = {MOV_##reg##_NUM, SIZE_MOV_##reg##_NUM};       \
                                    WriteBinCmd(trans, &mov##reg);                                      \
                                    WriteNum(trans, num);

#define WriteMovMemReg(reg, num)                Operation movr11##reg = {MOV_MEM_R11_##reg, SIZE_MOV_MEM_R11_##reg};    \
                                                WriteBinCmd(trans, &movr11##reg);                                       \
                                                WriteNum(trans, num);


#define WriteMovRegMem(reg, num)            Operation mov##reg##r11 = {MOV_##reg##_MEM_R11, SIZE_MOV_##reg##_MEM_R11};  \
                                            WriteBinCmd(trans, &mov##reg##r11);                                 \
                                            WriteNum(trans, num);

#define WriteAddRegNum(reg, num)            Operation add##reg = {ADD_##reg##_NUM, SIZE_ADD_##reg##_NUM};      \
                                            WriteBinCmd(trans, &add##reg);                                     \
                                            WriteNum(trans, num);

#endif // DSL_H
