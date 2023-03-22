#ifndef D_MODEM_H
#define D_MODEM_H

#include <Arduino.h>
#include <stdarg.h>

enum
{
    MODEM_STATUS_VALID_RESPONSE,
    MODEM_STATUS_INVALID_PARAMETERS,
    MODEM_STATUS_TIMEOUT_ERROR,
    MODEM_STATUS_URC_EVENT,
    MODEM_STATUS_UNKNOWN,
};

enum
{
    AT_STATE_READING,
    AT_STATE_IDENTIFYING_RESPONSE,
    AT_STATE_HANDLING_URC
};

class Modem
{
public:
    /**
     * @brief Initializes the modem device
     *
     */
    virtual void begin() = 0;
    /**
     * @brief Sends AT command to the modem device
     *
     * @param cmd The AT command to be sent
     */
    virtual void send(const char *cmd) = 0;
    /**
     * @brief Sends AT command to the modem device
     *
     * @param cmd The AT command to be sent
     */
    virtual void send(const String &cmd) = 0;
    /**
     * @brief Sends AT command to the modem device.
     *        This method allows formatted string as the input.
     *
     * @param fmt The formatted string.
     * @param ...
     */
    virtual void sendf(const char *fmt, ...) = 0;
    /**
     * @brief Wait for the response from the modem device until timeout occurs.
     *
     * @param timeout_ms The timeout duration to wait for the response.
     * @param buffer The pointer to the buffer to store the response.
     * @return int The status of the response
     */
    virtual int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) = 0;
    /**
     * @brief Checks if the modem is ready to receive the AT commands.
     *
     * @return true
     * @return false
     */
    virtual bool isReady() = 0;
};

#endif