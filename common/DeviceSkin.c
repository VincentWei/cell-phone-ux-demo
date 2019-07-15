///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "DeviceSkin.h"

#define PATH_LEN        512
#define KEY_CAP_LEN     64
#define KEY_TXT_LEN     128
#define BUF_LEN         1024 

#ifdef WIN32
#   define strtok_r(x, y, z) strtok(x, y)
#endif /* WIN32 */

#define dbg() printf("%s %d %s\n", __FILE__, __LINE__, __FUNCTION__)

static const char* UP              = "Up";
static const char* DOWN            = "Down";
static const char* CLOSED          = "Closed";
static const char* CLOSED_AREAS    = "ClosedAreas";
static const char* SCREEN          = "Screen";
static const char* BACK_SCREEN     = "BackScreen";
static const char* CLOSED_SCREEN   = "ClosedScreen";
static const char* CURSOR          = "Cursor";
static const char* AREAS           = "Areas";
static const char* MOUSE_HOVER     = "HasMouseHover"; 
static const char* FILE_HEADER     = "[SkinFile]\n";

struct DeviceSkinButtonArea {
    char name[KEY_CAP_LEN];
    int	 scancode;
    RECT rc;
    char text[KEY_TXT_LEN];
    BOOL activeWhenClosed;
    BOOL toggleArea;
    BOOL toggleActiveArea;
};

struct DeviceSkinParameters {
    char skinImageUpFileName[PATH_LEN];
    char skinImageDownFileName[PATH_LEN];
    char skinImageClosedFileName[PATH_LEN];
    char skinCursorFileName[PATH_LEN]; 

    RECT screenRect;
    RECT backScreenRect;
    RECT closedScreenRect;
    POINT cursorHot;
    struct DeviceSkinButtonArea *buttonAreas;
    size_t buttonAreaNum;
    //listhead toggleAreaList;

    int joystick;
    char prefix[PATH_LEN];
    BOOL hasMouseHover;
};

/*
int wordCount(const char* data)
{
    char * str = strdup(data);
    char *token;
    char *delim = " \t\n";
    char *saveptr;
    int j; 
    for (j = 0; ; j++, str = NULL) {
        token = strtok_r(str, delim, &saveptr);
        if (token == NULL) {
            break;
        }
    }
    free(str); 
    return j;
}
*/

int parseRect(char * str, RECT* rc)
{
    char *token;
    char *delim = " ";
    char *saveptr;
    int v[4];
    int j; 

    for (j = 0; ; j++, str = NULL) {
        token = strtok_r(str, delim, &saveptr);
        if (token == NULL) {
            if(j <= 3) {
                printf("parseRect: less than four axis value\n");
                return -1;
            }
            break;
        }
        v[j] = atoi(token);

        if(j>3) {
            break;
        }
    }
    rc->left = v[0];
    rc->top  = v[1];
    rc->right = v[2];
    rc->bottom = v[3];
    return 0;
}

static int parseDeviceSkinButtonArea(char* data, struct DeviceSkinButtonArea *area, int parsedCount) 
{
    char *token;
    char *delim = " \t\n\"";
    char *saveptr;
    int v[4];
    int j; 
    char * str = data;

    for (j = 0; ; j++, str = NULL) {
        token = strtok_r(str, delim, &saveptr);
        if (token == NULL) {
            if(j < 5) {
                printf("btn area config item format error\n"); return -1;
                return -1;
            }
            break;
        }
        if(j == 0) { 
            strcpy(area[parsedCount].name, token); 
        } else if(j == 1) {
            char *endptr;
            area[parsedCount].scancode = strtol(token, &endptr, 0);
            //printf("area[%d].name = %s, scancode = %d\n", parsedCount,  area[parsedCount].name, area[parsedCount].scancode);
        } else {
            v[j-2] = atoi(token);
        }

        if(j>=5) {
            break;
        }
    }
    area[parsedCount].rc.left = v[0];
    area[parsedCount].rc.top  = v[1];
    area[parsedCount].rc.right = v[2];
    area[parsedCount].rc.bottom = v[3];

    return 0;
}

HSKIN SkinLoad(const char* skin_name)
{
    FILE *fp;
    struct DeviceSkinParameters *pskin = NULL;
    char name[PATH_LEN];
    char buf[BUF_LEN];
    int btn_parsed = 0;

    memset(name, 0, PATH_LEN); 
    
    //assume that skin_name never exceed PATH_LEN 
    strcpy(name, skin_name);
    if (name[strlen(name)-1] == '/')
        name[strlen(name)-1] = 0; 

    //form full path name of the skin config file
    fp = fopen(name, "r");
    if(NULL == fp) {
        printf("The skin configuration file %s could not be opened.\n", name);
        goto fail;
    }

    pskin = malloc(sizeof(struct DeviceSkinParameters));
    memset(pskin, 0, sizeof(struct DeviceSkinParameters));
    if(NULL == pskin) {
        printf("Can't allocate memroy for pskin struct.\n");
        goto fail;
    }

    memset(buf, 0, BUF_LEN);
    if(NULL ==fgets(buf, BUF_LEN, fp)) {
        printf("Can't read skin config file %s.\n", name);
        goto fail;
    } 

    if(0 != strncmp(buf, FILE_HEADER, strlen(FILE_HEADER))) {
        printf("Skin config file format error.\n");
        goto fail;
    }

    while(TRUE) {
        //read a line from skin config file
        char *result = fgets(buf, BUF_LEN, fp); 
        int i = 0;

        if(NULL == result)
            break; 

        //skip whitespace
        while(result[i++] == ' ') 
            result++;

        i = strlen(result);
        while(i-- && (result[i] == '\n' || result[i] == ' ' || result[i] == '\r'))
            result[i] = 0;

        if(strlen(result) < 2)
            continue;

        //skip comment
        if(result[0] == '#')
            continue;

        //parse the line
        if(0 == strncmp(result, UP, strlen(UP))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                strcpy(pskin->skinImageUpFileName, result);
        } else if(0 == strncmp(result, DOWN, strlen(DOWN))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                strcpy(pskin->skinImageDownFileName, result); 
        } else if(0 == strncmp(result, CLOSED, strlen(CLOSED))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                strcpy(pskin->skinImageClosedFileName, result); 
        } else if(0 == strncmp(result, CLOSED_AREAS, strlen(CLOSED_AREAS))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                parseRect(result, &pskin->screenRect);
        } else if(0 == strncmp(result, SCREEN, strlen(SCREEN))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                parseRect(result, &pskin->screenRect);
        } else if(0 == strncmp(result, CLOSED_SCREEN, strlen(CLOSED_SCREEN))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                parseRect(result, &pskin->closedScreenRect);
        } else if(0 == strncmp(result, BACK_SCREEN, strlen(BACK_SCREEN))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                parseRect(result, &pskin->backScreenRect);
        } else if(0 == strncmp(result, CURSOR, strlen(CURSOR))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                strcpy(pskin->skinCursorFileName, result); 
        } else if(0 == strncmp(result, AREAS, strlen(AREAS))) { 
            result = strchr(result, '=') + 1;
            if(result) 
                pskin->buttonAreaNum = atoi(result);
            pskin->buttonAreas = malloc(pskin->buttonAreaNum * sizeof(struct DeviceSkinButtonArea));
            memset(pskin->buttonAreas, 0,pskin->buttonAreaNum * sizeof(struct DeviceSkinButtonArea));
        } else if(0 == strncmp(result, MOUSE_HOVER, strlen(MOUSE_HOVER))) { 
            //bool false?
            result = strchr(result, '=') + 1;
            if(result) 
                pskin->hasMouseHover = atoi(result);
        } else if(result[0] == '"') {
            if(0 == pskin->buttonAreaNum) {
                printf("button number not specified\n");
                goto fail;
            }

            if(0 == parseDeviceSkinButtonArea(result,pskin->buttonAreas, btn_parsed)) {
                btn_parsed++; 
            } 
        } 
    }
    if(btn_parsed != pskin->buttonAreaNum) {
        printf("skin file key number error\n");
        goto fail;
    }

    fclose(fp);
    return (void*)pskin;

fail:
    if(pskin->buttonAreas)
        free(pskin->buttonAreas);
    if(pskin)
        free(pskin);
    fclose(fp);
    return NULL;
}

void SkinUnload(HSKIN hskin)
{ 
    struct DeviceSkinParameters * pskin = (struct DeviceSkinParameters*)hskin;
    if(pskin->buttonAreas)
        free(pskin->buttonAreas);
    if(pskin)
        free(pskin);

    return;
}

int SkinGetItem(HSKIN hskin, const char* key, char* value, int len)
{
    struct DeviceSkinParameters * pskin = (struct DeviceSkinParameters*)hskin;
    if(strcmp(key, UP) == 0) { 
        strcpy(value, pskin->skinImageUpFileName);
    } else if(strcmp(key, DOWN) == 0) { 
        strcpy(value, pskin->skinImageDownFileName); 
    } else if(strcmp(key, CLOSED) == 0) { 
        strcpy(value, pskin->skinImageClosedFileName); 
    } else if(strcmp(key, CURSOR) == 0) { 
        strcpy(value, pskin->skinCursorFileName); 
    } else {
        printf("not found value for key %s\n", key);
        return -1;
    } 
    return 0;
}

int  SkinPointToScancode(HSKIN hskin, const POINT* pt)
{ 
    struct DeviceSkinParameters  *pskin =  (struct DeviceSkinParameters*)hskin;
    int i = 0;
    for(; i < pskin->buttonAreaNum; i++) {
        if(PtInRect(&(pskin->buttonAreas[i].rc), pt->x, pt->y)) {
            return pskin->buttonAreas[i].scancode;
        } 
    } 
    return -1;
}


const RECT* SkinScancodeToRect(HSKIN hskin, int scancode)
{ 
    struct DeviceSkinParameters  *pskin =  (struct DeviceSkinParameters*)hskin;
    int i = 0;
    for(; i < pskin->buttonAreaNum; i++) {
        if(pskin->buttonAreas[i].scancode == scancode) {
            return &pskin->buttonAreas[i].rc;
        } 
    } 
    return NULL;
}

const char *SkinScancodeToName(HSKIN hskin, int scancode)
{ 
    struct DeviceSkinParameters  *pskin =  (struct DeviceSkinParameters*)hskin;
    int i = 0;
    for(; i < pskin->buttonAreaNum; i++) {
        if((pskin->buttonAreas[i].scancode) == scancode) {
            return pskin->buttonAreas[i].name;
        } 
    } 
    return NULL;
}


const RECT* SkinPointToRect(HSKIN hskin, const POINT* pt)
{
    struct DeviceSkinParameters  *pskin =  (struct DeviceSkinParameters*)hskin;
    int i = 0;
    for(; i < pskin->buttonAreaNum; i++) {
        if(PtInRect(&(pskin->buttonAreas[i].rc), pt->x, pt->y)) {
            return &pskin->buttonAreas[i].rc;
        } 
    } 

    return NULL;
}

const char* SkinPointToName(HSKIN hskin, const POINT* pt)
{
    struct DeviceSkinParameters  *pskin =  (struct DeviceSkinParameters*)hskin;
    int i = 0;
    for(; i < pskin->buttonAreaNum; i++) {
        if(PtInRect(&(pskin->buttonAreas[i].rc), pt->x, pt->y)) {
            return pskin->buttonAreas[i].name;
        } 
    } 

    return NULL;
}
