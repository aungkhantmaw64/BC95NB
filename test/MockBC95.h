#ifndef D_MOCKBC95_H
#define D_MOCKBC95_H

#include "Modem.h"

class MockBC95 : public Modem
{
public:
    MockBC95(){};
    ~MockBC95(){};
    void begin() override;
    void send(const char *cmd) override;
    void send(const String &cmd) override;
    int waitForResponse(unsigned long timeout_ms, String *buffer = NULL) override;
    ;
    bool isReady() override;
};

void MockBC95::begin()
{
}

void MockBC95::send(const char *cmd)
{
}

void MockBC95::send(const String &cmd)
{
}

int MockBC95::waitForResponse(unsigned long timeout_ms, String *buffer)
{
}

bool MockBC95::isReady()
{
}

#endif