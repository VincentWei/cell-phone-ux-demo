/*!============================================================================
 * @file SimpleProviderFactory.hh 
 * @Synopsis A simple content provider factory. 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SIMPLEPROVIDERFACTORY_INC
#define  SIMPLEPROVIDERFACTORY_INC

#include <string>
class ContentProvider;

namespace ProviderFactory {

ContentProvider* CreateSystemProvider(const std::string& name);

}
#endif   /* ----- #ifndef SIMPLEPROVIDERFACTORY_INC  ----- */

