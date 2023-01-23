#include "BC95.h"

BC95::BC95(Stream *stream, const int resetPin)
    : _stream(stream), _resetPin(resetPin)
{
}

BC95::~BC95()
{
}

void BC95::begin(void)
{
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, LOW);
}