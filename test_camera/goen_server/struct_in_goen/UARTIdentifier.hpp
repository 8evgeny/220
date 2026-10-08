/*
 * UARTIdentifier.hpp
 *
 *  Created on: Apr 11, 2025
 *      Author: Daniil
 */

#ifndef CONTROLLERS_UART_UARTIDENTIFIER_HPP_
#define CONTROLLERS_UART_UARTIDENTIFIER_HPP_
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
#include <cstdint>
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
namespace UART{

enum class UARTIdentifierMemsBoard : int16_t
    {
	mode = 0x001,
	controlPosition,
	controlSpeed,
	controlTracking,
	status,
      //---------------
      /** Счётчик элементов enum */
      enumCounter
    };


}

#endif /* CONTROLLERS_UART_UARTIDENTIFIER_HPP_ */
