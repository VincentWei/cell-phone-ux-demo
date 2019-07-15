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
 * @file SignalEventWithParameters.hh 
 * @Synopsis Signal Event with any parameters. 
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#ifndef  SIGNALEVENTWITHPARAMETERS_INC
#define  SIGNALEVENTWITHPARAMETERS_INC
#include <vector>
#include "boost/any.hpp"
#include "Event.hh"

class SignalEventWithParameters : public SignalEvent {
public:
    typedef std::vector<boost::any> ParametersCollection;
    SignalEventWithParameters(const void *sender, const char *signal) : 
        SignalEvent(sender, signal) {}
    void appendParameter(const boost::any& param) {
        m_params.push_back(param);
    }
    const ParametersCollection& parameters() const {
        return m_params;
    }
private:
    ParametersCollection m_params;
};

#endif   /* ----- #ifndef SIGNALEVENTWITHPARAMETERS_INC  ----- */

