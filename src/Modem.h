#ifndef BC95NB_SRC_MODEM_H
#define BC95NB_SRC_MODEM_H

#include <Arduino.h>

#define MODEM_CMD_MAX_SIZE 100
#define MODEM_MAX_RESPONSE_HANDLERS 4

/**
 * @brief Used to indicate the status of the AT command response
 */
enum class ResponseCode
{
    EMPTY,
    OK,
    ERROR,
    CME_ERROR
};

/**
 * @brief Modem Unsolicited Response Handler
 *
 */
class ModemUrcHandler
{
public:
    virtual void onReceive(String &_response) = 0;
};

/**
 * @brief  Class to send/receive raw AT commands/responses to/from the modem
 *
 */
class Modem
{
public:
    /**
     * @brief Construct a new Modem object
     *
     * @param _stream Stream object to send/receive data to/from the modem
     * @param _resetPin Reset pin for rebooting the modem
     * @param _activeHigh Reset circuit type
     * @param _debugStream Debug stream
     */
    Modem(Stream &_stream, uint8_t _resetPin, bool _activeHigh, Stream *_debugStream = nullptr);
    /**
     * @brief reset the modem
     *
     */
    void reset();
    /**
     * @brief Sends an AT command to the modem. (e.g. "AT+CMD\r\n")
     *
     * @param _cmd Command to send
     */
    void send(const char *_cmd);
    /**
     * @brief Wait for the modem to respond back to the command
     * @param _timeoutMs timeout in milliseconds
     * @param _response Response
     * @return ResponseCode
     */
    ResponseCode waitForResponse(uint32_t _timeoutMs, String *_response = nullptr);
    /**
     * @brief Add a urc handler for the modem.
     *
     * @param _handler Urc Handler
     * @return int returns negative value if the max handler is reached.
     */
    int addResponseHandler(ModemUrcHandler *_handler);

private:
    Stream &m_stream;
    Stream *m_debugStream;
    uint8_t m_resetPin;
    bool m_activeHigh;
    ModemUrcHandler *m_respHandlers[MODEM_MAX_RESPONSE_HANDLERS] = {nullptr};
};

#endif
