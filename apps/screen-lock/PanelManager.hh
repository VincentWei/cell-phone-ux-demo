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

/*!============================================================================
 * @file PanelManager.hh 
 * @Synopsis  
 * @author DengMin
 * @version 1.0
 * @date 01/12/2010 13:50:42 
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef _PANELMANAGER_HH
#define _PANELMANAGER_HH

#include <string>
#include <map>

/* VW: workaround for compile with g++ 4.8 */
#if __cplusplus < 201103L
typedef unsigned short char16_t;
#endif // __cplusplus

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <mgncs/mgncs.h>

#define PIECE_NAME_MAXLEN  32

typedef struct _NamePiece
{
    char name[PIECE_NAME_MAXLEN];
    mHotPiece* piece;
} NamePiece;

class PanelManager
{
    public:
        PanelManager();
        virtual ~PanelManager();
        void parseXml(const char* xml);
        void parseXmlFile(const char* xmlFile);

        mHotPiece* getRoot();
        mHotPiece* get(const char* name);

        void putData(const char* key, void* data);
        void* getData(const char* key);

    protected:
        void _clear();
        void _createFromXml(const char* xml, int length);
        void _createFromXmlFile(const char* xmlFile);
        void _createNode(xmlNodePtr a_node);
        mHotPiece* _createPieceForNode(xmlNodePtr a_node);
        mHotPiece* _createPiece(const char* type, DWORD add_data);

    protected:
        xmlDocPtr m_doc;
        xmlNodePtr m_rootNode;

        typedef std::map<std::string, void*> DataMap;
        DataMap m_dataMap;
};

void PanelManager::putData(const char* key, void* data)
{
    m_dataMap[key] = data;
}

void* PanelManager::getData(const char* key)
{
    DataMap::iterator iter = m_dataMap.find(key);

    if(iter != m_dataMap.end())
        return iter->second;
    else
        return NULL;
}

static inline const char* getNameFromNode(xmlNodePtr node)
{
    return ((NamePiece*)node->_private)->name;
}

static inline mHotPiece* getPieceFromNode(xmlNodePtr node)
{
    return ((NamePiece*)node->_private)->piece;
}

PanelManager::PanelManager()
    : m_doc(NULL), m_rootNode(NULL)
{
}

PanelManager::~PanelManager()
{
    _clear();
}

#if 0
static xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *szXpath) 
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
#endif

// TODO: optimization
static mHotPiece* _searchNode(xmlNodePtr node, const char* name)
{
    xmlNode *cur_node = NULL;
    mHotPiece* ret = NULL;

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {

            printf("_searchNode: %s\n", getNameFromNode(cur_node));

            if (strcmp(getNameFromNode(cur_node), name) == 0) {
                ret = getPieceFromNode(cur_node);
                break;
            }

            ret = _searchNode(cur_node->children, name);
        }
    }

    return ret;
}

mHotPiece* PanelManager::get(const char* name)
{
    assert(name);
    assert(m_doc);

#if 0
    xmlChar *xpath = BAD_CAST name;

    xmlXPathObjectPtr xpathobj = get_nodeset(m_doc, xpath);

    if(xpathobj) {
        xmlNodeSetPtr nodeset = xpathobj->nodesetval;
        return (mHotPiece*) getPieceFromNode(nodeset->nodeTab[0]);
    }
#else
    return _searchNode(m_rootNode, name);
#endif
}

void PanelManager::parseXml(const char* xml)
{
    assert(xml);

    _clear();

    int length = strlen(xml);
    if (length > 0)
        _createFromXml(xml, length);
}

void PanelManager::parseXmlFile(const char* xmlFile)
{
    assert(xmlFile);

    _clear();

    _createFromXmlFile(xmlFile);
}

void PanelManager::_clear()
{
    if (m_doc != NULL) {
        xmlFreeDoc(m_doc);
        m_doc = NULL;
        m_rootNode = NULL;
    }
}

void PanelManager::_createFromXml(const char* xml, int length)
{
    xmlDocPtr doc; /* the resulting document tree */
    xmlNodePtr root_element = NULL;

    /*
     * The document being in memory, it have no base per RFC 2396,
     * and the "noname.xml" argument will serve as its base.
     */
    doc = xmlReadMemory(xml, length, "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse document\n");
        return;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    m_doc = doc;
    m_rootNode = root_element;

    _createNode(root_element);
}

void PanelManager::_createFromXmlFile(const char* xmlFile)
{
    xmlDocPtr doc; /* the resulting document tree */
    xmlNodePtr root_element = NULL;

    doc = xmlReadFile(xmlFile, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", xmlFile);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    m_doc = doc;
    m_rootNode = root_element;

    _createNode(root_element);
}

void PanelManager::_createNode(xmlNodePtr a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("panel node: name: %s\n", cur_node->name);

            _createPieceForNode(cur_node);
            _createNode(cur_node->children);
        }
    }
}

static int xmlChar2i(const xmlChar* str)
{
    return atoi((const char*)str);
}

mHotPiece* PanelManager::_createPieceForNode(xmlNodePtr a_node)
{
    xmlChar* type = xmlGetProp(a_node, BAD_CAST "type");
    xmlChar* width = xmlGetProp(a_node, BAD_CAST "width");
    xmlChar* height = xmlGetProp(a_node, BAD_CAST "height");
    xmlChar* left = xmlGetProp(a_node, BAD_CAST "left");
    xmlChar* top = xmlGetProp(a_node, BAD_CAST "top");
    xmlChar* name = xmlGetProp(a_node, BAD_CAST "name");
    xmlChar* additonaldata = xmlGetProp(a_node, BAD_CAST "additonaldata");

    RECT rc;
    int left_i = 0;
    int top_i = 0;
    const char* name_c = (const char*) name;
    DWORD add_data = 0;

    assert(type);

    if (additonaldata) {
        add_data = (DWORD)getData((const char*) additonaldata);
    }

    mHotPiece *newPiece = _createPiece((const char*)type, add_data);
    if (name_c == NULL)
        name_c = (const char*) a_node->name;

    // TODO: memory recovery
    NamePiece* np = (NamePiece*)malloc(sizeof(NamePiece));
    strncpy(np->name, name_c, PIECE_NAME_MAXLEN);
    np->piece = newPiece;

    printf("create node: %s %s\n", np->name, type);

    memset(&rc, 0, sizeof(rc));
    if (width) rc.right = xmlChar2i(width);
    if (height) rc.bottom = xmlChar2i(height);

    if (left) left_i = xmlChar2i(left);
    if (top) top_i = xmlChar2i(top);

    _c(newPiece)->setRect(newPiece, &rc);

    a_node->_private = np;

    // TODO: optimization
    if (a_node != m_rootNode) {
        if (INSTANCEOF(getPieceFromNode(a_node->parent), mPanelPiece)) {
            mPanelPiece* parent = (mPanelPiece*) getPieceFromNode(a_node->parent);
            _c(parent)->addContent(parent, newPiece, left_i, top_i);
        }
        else {
            assert(0);
        }
    }

    if (type) xmlFree(type);
    if (width) xmlFree(width);
    if (height) xmlFree(height);
    if (left) xmlFree(left);
    if (top) xmlFree(top);

    return newPiece;
}

#define DEF_PIECE_TYPE(Piece) { #Piece, (mHotPieceClass*)((void *)&(Class(Piece))) }

mHotPiece* PanelManager::_createPiece(const char* type, DWORD add_data)
{
    typedef struct _PieceType {
        const char* name;
        mHotPieceClass* type;
    } PieceType;

    const static PieceType type_list[] = {
        DEF_PIECE_TYPE(mPanelPiece),
        DEF_PIECE_TYPE(mTextPiece),
        DEF_PIECE_TYPE(mShapeTransRoundPiece),
        DEF_PIECE_TYPE(mShutterPiece),
        //DEF_PIECE_TYPE(mTablePanelPiece),
        //DEF_PIECE_TYPE(mScrollViewPiece),

        {NULL, NULL,}
    };

    const PieceType* p = type_list;
    while (p) {
        if (p->name == NULL) break;

        if (strcmp(p->name, type) == 0) {
            break;
        }

        ++p;
    }

    assert(p->name && p->type);

    return (mHotPiece*) ncsNewPiece(p->type, add_data);
}

#endif
