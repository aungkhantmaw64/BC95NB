#ifndef D_BC95NB_H
#define D_BC95NB_H

#include "Modem.h"

enum
{
    NB_IDLE,
    NB_MODEM_AWAKEN
};

class NBClass
{
public:
    NBClass(Modem *modem);
    ~NBClass();
    int begin();
    int begin(int band);

private:
    Modem *modem_;
    int connectionState_;
    String buffer_;
};

#endif
