#ifndef SHUTDOWN_INTERRUPT_H
#define SHUTDOWN_INTERRUPT_H

// Shutdown Interrupt ---------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2026.02.06
//
// Description: Object and functions representing a GPIO interrupt indicating a shutdown event.

// Includes -------------------------------------------------------------------------------------------------------------------

// LibGPIOD
#include <gpiod.h>

// C Standard Library
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct shutdownInterrupt shutdownInterrupt_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes a shutdown interrupt structure.
 * @param consumer The consumer of the interrupt. Used to identify what application is requesting the GPIO.
 * @param chipName The name of the GPIO chip to open. Ex. 'gpiochip0'.
 * @param lineNumber The GPIO line to request.
 * @return The dynamically allocated interrupt structure. Must be deallocated via @c shutdownInterruptDealloc .
 */
shutdownInterrupt_t* shutdownInterruptInit (const char* consumer, const char* chipName, unsigned int lineNumber);

/**
 * @brief Polls a shutdown interrupt for a shutdown event. This function blocks until the interrupt is detected, or an
 * unrecoverable error occurs.
 * @param interrupt The interrupt to poll.
 * @return 0 if the interrupt was detected, The error code if an error occurred.
 */
int shutdownInterruptPoll (shutdownInterrupt_t* interrupt);

/**
 * @brief Deallocates a shutdown interrupt and releases the GPIO line.
 * @param interrupt The interrupt to deallocate.
 */
void shutdownInterruptDealloc (shutdownInterrupt_t* interrupt);

#endif // SHUTDOWN_INTERRUPT_H