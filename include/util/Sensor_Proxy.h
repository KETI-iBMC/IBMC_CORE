
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx_____include_util_Sensor_Proxy_h__PROXY_MARSHAL_H
#define __dbusxx_____include_util_Sensor_Proxy_h__PROXY_MARSHAL_H

#include <dbus-c++-1/dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace keti {
namespace bmc {

class ADC_proxy
: public ::DBus::InterfaceProxy
{
public:

    ADC_proxy()
    : ::DBus::InterfaceProxy("org.freedesktop.keti.bmc.ADC")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    int32_t lightning_sensor_read(const uint8_t& fru, const uint8_t& sensor_num)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << fru;
        wi << sensor_num;
        call.member("lightning_sensor_read");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        int32_t argout;
        ri >> argout;
        return argout;
    }

    int32_t read_adc_value(const int32_t& pin, const std::string& device)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << pin;
        wi << device;
        call.member("read_adc_value");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        int32_t argout;
        ri >> argout;
        return argout;
    }


public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } } } 
namespace org {
namespace freedesktop {
namespace keti {
namespace bmc {

class edge_proxy
: public ::DBus::InterfaceProxy
{
public:

    edge_proxy()
    : ::DBus::InterfaceProxy("org.freedesktop.keti.bmc.edge")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */

public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } } } 
namespace org {
namespace freedesktop {
namespace keti {
namespace bmc {

class FAN_proxy
: public ::DBus::InterfaceProxy
{
public:

    FAN_proxy()
    : ::DBus::InterfaceProxy("org.freedesktop.keti.bmc.FAN")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    int32_t read_fan_value(const int32_t& fanno)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << fanno;
        call.member("read_fan_value");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        int32_t argout;
        ri >> argout;
        return argout;
    }


public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } } } 
namespace org {
namespace freedesktop {
namespace keti {
namespace bmc {

class PSU_proxy
: public ::DBus::InterfaceProxy
{
public:

    PSU_proxy()
    : ::DBus::InterfaceProxy("org.freedesktop.keti.bmc.PSU")
    {
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    double peci_CPU_TEMP0()
    {
        ::DBus::CallMessage call;
        call.member("peci_CPU_TEMP0");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        double argout;
        ri >> argout;
        return argout;
    }

    double read_lm75_value(const int32_t& no)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << no;
        call.member("read_lm75_value");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        double argout;
        ri >> argout;
        return argout;
    }


public:

    /* signal handlers for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
};

} } } } 
#endif //__dbusxx_____include_util_Sensor_Proxy_h__PROXY_MARSHAL_H
