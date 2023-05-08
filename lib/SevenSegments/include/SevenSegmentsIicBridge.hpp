#ifndef SEVEN_SEGMENTS_IIC_BRIDGE_HPP
#define SEVEN_SEGMENTS_IIC_BRIDGE_HPP

// standard includes
#include <cstdint>

// esp32 includes

// project includes
#include "SevenSegmentsTypes.hpp"

/** @brief An I2C device will be designated by an ID, platform-specific implementation will have to deal with it. */
using IicDeviceId = uint8_t ;

/** @brief model of status after some I2C operation took place, to wrap a platform-specific value type. */
template <typename ReturnCode> 
struct IicStatus {
    bool ok ;
    ReturnCode errorCode;
} ; 

/** @brief Model of a 7 segment display module addressable through an I2C link.
 */
template <typename ReturnCode> 
class SevenSegmentsIicBridge {
    private:

    public:
        virtual ~SevenSegmentsIicBridge() {} ;
        virtual IicStatus<ReturnCode> upload(SevenSegmentsRegisters *registers, IicDeviceId recipient) = 0 ;
} ;

#endif