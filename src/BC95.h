#ifndef D_BC95_H
#define D_BC95_H

#include <Arduino.h>
#include "Modem.h"

class BC95 : public Modem
{
public:
    /**
     * @brief Construct a new BC95 object
     *
     * @param stream
     * @param resetPin
     */
    BC95(Stream *stream, const int resetPin);
    /**
     * @brief Destroy the BC95 object
     *
     */
    ~BC95();
    /**
     * @brief
     *
     */
    void begin() override;
    /**
     * @brief
     *
     * @param cmd
     */
    void send(const char *cmd) override;
    /**
     * @brief
     *
     * @param cmd
     */
    void send(const String &cmd) override;
    /**
     * @brief
     *
     * @param fmt
     * @param ...
     */
    void sendf(const char *fmt, ...) override;
    /**
     * @brief
     *
     * @param timeout_ms
     * @param buffer
     * @return int
     */
    int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) override;
    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool isReady();
    /**
     * @brief
     *
     * @param inverted
     * @return int
     */
    int hardReset(bool inverted);
    /**
     * @brief
     *
     * @return int
     */
    int reset();

private:
    Stream *_stream;
    int _resetPin;
    int state_;
    String _lastCmd;
    String response_;
};

#endif