#ifndef     SLAVE_DATA_H
#define     SLAVE_DATA_H

#include    <QtGlobal>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct slave_data_t
{
    /// Адрес сигнала в сети Modbus
    quint16     address;
    /// Индекс синала в массиве, передаваемом в симулятор
    size_t      index;
    /// Значение сигнала
    quint16      cur_value;
    quint16      prev_value;



    slave_data_t()
        : address(0)
        , index(0)
        , cur_value(0)
        , prev_value(0)
    {

    }
};

#endif // SLAVE_DATA_H
