#ifndef DSL_H
#define DSL_H

#include "translator.h"

#define WritePushReg(reg)   Operation push##reg = {PUSH_##reg, SIZE_PUSH_##reg};      \
                            WriteBinCmd(trans, &push##reg);

#define WritePopReg(reg)   Operation pop##reg = {POP_##reg, SIZE_POP_##reg};      \
                            WriteBinCmd(trans, &pop##reg);



#endif // DSL_H
