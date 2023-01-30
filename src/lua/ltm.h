//
// Created by shirome on 19/05/2022.
//

#ifndef ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_LTM_H
#define ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_LTM_H

#include "lobject.h"


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM"
*/
typedef enum {
    TM_INDEX,
    TM_NEWINDEX,
    TM_GC,
    TM_MODE,
    TM_EQ,  /* last tag method with `fast' access */
    TM_ADD,
    TM_SUB,
    TM_MUL,
    TM_DIV,
    TM_MOD,
    TM_POW,
    TM_UNM,
    TM_LEN,
    TM_LT,
    TM_LE,
    TM_CONCAT,
    TM_CALL,
    TM_N          /* number of elements in the enum */
} TMS;



#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
  ((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)  gfasttm(G(l), et, e)

LUAI_DATA const char *const luaT_typenames[];


LUAI_FUNC const TValue *luaT_gettm (Table *events, TMS event, TString *ename);
LUAI_FUNC const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o,
                                         TMS event);
LUAI_FUNC void luaT_init (lua_State *L);

#endif //ANDROID_HOOKING_PATCHING_TEMPLATE_MAIN_LTM_H
