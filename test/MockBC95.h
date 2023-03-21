#ifndef D_MOCKBC95_H
#define D_MOCKBC95_H

#include "Modem.h"
#include <queue>

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

    /* Mock functions */
    void responseExpect(const char *expectedBuffer, int expectedStatus);

private:
    void bufferExpect(const char *expectedBuffer);
    void statusExpect(int expectedStatus);

private:
    std::queue<String> buffers_;
    std::queue<int> responseStatuses_;
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
    int status = responseStatuses_.front();
    responseStatuses_.pop();
    *buffer = buffers_.front();
    buffers_.pop();
    return status;
}

bool MockBC95::isReady()
{
}

void MockBC95::bufferExpect(const char *expectedBuffer)
{
    buffers_.push(String(expectedBuffer));
}

void MockBC95::statusExpect(int status)
{
    responseStatuses_.push(status);
}

void MockBC95::responseExpect(const char *expectedBuffer, int expectedStatus)
{
    bufferExpect(expectedBuffer);
    statusExpect(expectedStatus);
}

#endif