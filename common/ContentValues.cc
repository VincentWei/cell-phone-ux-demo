/*! ============================================================================
 * @file ContentValues.cc 
 * @Synopsis  
 * @author DongKai
 * @version 1.0
 *  Company: Beijing Feynman Software Technology Co., Ltd.
 */

#include <cassert>
#include <sstream>
#include "ContentValues.hh"

const boost::any ContentValues::get(const std::string& key) const
{
    ExtraMap::const_iterator found = m_data.find(key);
    static boost::any s_empty;

    return (found == m_data.end() ? s_empty : found->second);
}

double ContentValues::getAsReal(const std::string& key, double defaultValue) const
{
    boost::any ret = get(key);
    if (!ret.empty()) {
        if (isReal(ret)) {
            return boost::any_cast<double>(ret);
        } else if (isString(ret)){
            double convert = .0;
            std::istringstream ss(boost::any_cast<std::string>(ret));
            ss >> convert;
            return ss.good() ? convert : defaultValue;
        } else {
            assert(0);
        }

    }
    return defaultValue;
}

int ContentValues::getAsInteger(const std::string& key, int defaultValue) const
{
    boost::any ret = get(key);
    if (!ret.empty()) {
        if (isInteger(ret)) {
            return boost::any_cast<int>(ret);
        } else if (isString(ret)){
            int convert = 0;
            std::stringstream ss(boost::any_cast<std::string>(ret));
            ss >> convert;
            return (!ss.fail() ? convert : defaultValue);
        } else {
            assert(0);
        }
    }
    return defaultValue;
}

std::string ContentValues::getAsString(const std::string& key) const
{
    boost::any ret = get(key);
    if (!ret.empty()) {
        if (isString(ret)) {
            return boost::any_cast<std::string>(ret);
        } else {
            std::stringstream tmp(std::stringstream::in | std::stringstream::out);
            if (isInteger(ret)) {
                tmp << boost::any_cast<int>(ret);
            } else if (isReal(ret)) {
                tmp << boost::any_cast<double>(ret);
            }
            return tmp.str();
        }
    }
    return "";
}

void ContentValues::put(const std::string& key, boost::any value)
{
    if (m_data.find(key) != m_data.end()) {
        m_data.erase(key);
    }
    m_data.insert(std::pair<std::string, boost::any>(key, value));
}

void ContentValues::putReal(const std::string& key, double value)
{
    put(key, value);
}

void ContentValues::putInteger(const std::string& key, int value)
{
    put(key, value);
}

void ContentValues::putString(const std::string& key, const std::string& value)
{
    put(key, value);
}

ContentValues::Strings ContentValues::keySet() const
{
    ExtraMap::const_iterator iter = m_data.begin();
    Strings result;
    for (; m_data.end() != iter; ++iter) {
        result.push_back(iter->first);
    }
    return result;
}

