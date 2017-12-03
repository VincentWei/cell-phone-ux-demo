#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "mpieceitem.h"
#include "mgrouppieceitem.h"

static void mGroupPieceItem_construct(mGroupPieceItem *self, DWORD add_data)
{
    Class(mPieceItem).construct((mPieceItem*)self, add_data);
    _c(self)->setKey(self, (char*)add_data);
}

static void mGroupPieceItem_destroy(mGroupPieceItem *self)
{
    if (self->key) {
        free(self->key);
    }
    Class(mPieceItem).destroy((mPieceItem*)self);
}

static void mGroupPieceItem_setKey(mGroupPieceItem *self, const char* key)
{
    if (self->key) {
        free(self->key);
    }
    self->key = malloc(sizeof(char)*(strlen(key)+1));
    strcpy(self->key, key);
    self->key[strlen(key)] = '\0';
}

static char* mGroupPieceItem_getKey(mGroupPieceItem *self)
{
    return self->key;
}

BEGIN_MINI_CLASS(mGroupPieceItem, mPieceItem)
	CLASS_METHOD_MAP(mGroupPieceItem, construct)
	CLASS_METHOD_MAP(mGroupPieceItem, destroy)
    CLASS_METHOD_MAP(mGroupPieceItem, getKey)
    CLASS_METHOD_MAP(mGroupPieceItem, setKey)
END_MINI_CLASS

