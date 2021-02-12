#ifndef     CONTROL_SIGNALS_H
#define     CONTROL_SIGNALS_H

#include    <array>
#include    "external-signal.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct control_signals_t
{
    enum
    {
        MAX_CONTROL_SIGNALS = 1000
    };

    std::array<signal_t, MAX_CONTROL_SIGNALS>  analogSignal;

    control_signals_t()
    {

    }
};

Q_DECLARE_METATYPE(control_signals_t)

#endif // CONTROL_SIGNALS_H
