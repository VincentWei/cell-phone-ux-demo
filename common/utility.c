/*! ============================================================================
 * @file Utility.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include "utility.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string.h>

#define XML_ENCODE "UTF-8"

HDC GetWindowForeground(HWND hwnd, BOOL set_foreground)
{
    if (HWND_INVALID != hwnd) {
        HDC sec_dc = GetSecondaryDC(hwnd);
        HDC memdc = HDC_INVALID; 
        BOOL has_secondary_dc = (sec_dc != HDC_SCREEN);
        if (!has_secondary_dc) {
            sec_dc = CreateSecondaryDC(hwnd);
        }
        memdc = CreateCompatibleDC(sec_dc);
        // Set memdc as secondary dc, and make the secondary dc update function to do nothing.
        SetSecondaryDC(hwnd, sec_dc, ON_UPDSECDC_DONOTHING);
        if (set_foreground) {
            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd, TRUE);
        }
        else
        {
            if (!IsWindowVisible(hwnd)) {
                IncludeWindowStyle(hwnd, WS_VISIBLE);
                UpdateWindow(hwnd, TRUE);
                ExcludeWindowStyle(hwnd, WS_VISIBLE);
            }
            else {
                UpdateWindow(hwnd, TRUE);
            }
        }
        if (HDC_INVALID != memdc) {
            BitBlt(sec_dc, 0, 0, -1, -1, memdc, 0, 0, -1);
        }
        /* If window haven't secondary dc ago.*/
        if (!has_secondary_dc) {
            DeleteSecondaryDC(hwnd);
        }
        else {
            SetSecondaryDC(hwnd, sec_dc, ON_UPDSECDC_DEFAULT);
        }
        return memdc;
    }
    return HDC_INVALID;
}

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *szXpath) 
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	
	context = xmlXPathNewContext(doc);
	if (context == NULL) 
	{	
		printf("context is NULL\n");
		return NULL; 
	}
	
	result = xmlXPathEvalExpression(szXpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) 
	{
		printf("xmlXPathEvalExpression return NULL\n"); 
		return NULL;  
	}
	
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
//		printf("nodeset is empty\n");
		return NULL;
	}
	
	return result;	
}

#if 0
int ReadXmlConfig(const char* file, const char* path, const char* attr, char* value, size_t size)
{
    if (NULL != file && NULL != path && NULL != value) {
        xmlNodePtr curNode = NULL;
        char* key = NULL;
        int ret = -1;
        xmlChar* szXpath = BAD_CAST(path); 
        xmlXPathObjectPtr app_result;
        xmlDocPtr doc = xmlReadFile(file, XML_ENCODE, XML_PARSE_RECOVER);
        if (NULL == doc) 
        { 	
            fprintf(stderr, "Document not parsed successfully.\n"); 	
            return -1; 
        } 
        if (NULL != attr) {
            key = (char*)calloc(1, strlen (path) + strlen(attr) + strlen("[@]") + 1);
            if (NULL != key) {
                sprintf(key, "%s[@%s]", path, attr);
                szXpath = BAD_CAST(key);
            }
            else {
                fprintf(stderr, "Not enough memory!\n");
                return -1;
            }
        }
        app_result = get_nodeset(doc, szXpath);
        free(key);

        if(NULL != app_result) {
            xmlChar *szValue = NULL;
            xmlNodeSetPtr nodeset = app_result->nodesetval;
            if (nodeset->nodeNr > 0) {
                /* Take the first match node. */
                curNode = nodeset->nodeTab[0];
                if(curNode != NULL) {
                    if (NULL != attr) {
                        szValue = xmlGetProp(curNode, BAD_CAST(attr));
                        if (NULL != szValue) {
                            if (strlen((const char*)szValue) < size) {
                                strcpy(value, (char*)szValue);
                                ret = 0;
                            }
                            xmlFree(szValue);
                        }
                    }
                    else {
                        szValue = xmlNodeGetContent(curNode);
                        if (NULL != szValue) {
                            if (strlen((const char*)szValue) < size) {
                                strcpy(value, (char*)szValue);
                                ret = 0;
                            }
                            xmlFree(szValue);
                        }
                    }
                }
            }
            xmlXPathFreeObject (app_result);
        }
        else {
//            fprintf(stderr, "Node set is empty.\n");
        }
        xmlFreeDoc(doc);
        return ret;
    }
    fprintf(stderr, "Invalid arguments.\n");
    return -1;
}
#endif

