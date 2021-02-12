#include    "kvt254.h"

#include    <sstream>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
LocoCrane254::LocoCrane254(QObject *parent) : LocoCrane(parent)
  , V1(1e-4)
  , V2(1e-4)
  , Vpz(3e-4)
  , delta_p(0.05)
  , ps(0.1)
  , min_pos(-0.05)
  , max_pos(1.0)
  , pos_duration(1.0)
  , volume(0)
  , p_volume(0)
  , dir(0)
  , pos_num(0)
  , isStop(false)
  , positions({0.0, 0.325, 0.5, 0.752, 1.0})
  , step_pressures({0.0, 0.13, 0.20, 0.30, 0.40})
{
    std::fill(K.begin(), K.end(), 0.0);
    std::fill(k.begin(), k.end(), 0.0);

    pos = 1.0;

    /*DebugLog *log = new DebugLog("kvt254.txt");
    connect(this, &LocoCrane254::DebugPrint, log, &DebugLog::DebugPring);*/
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
LocoCrane254::~LocoCrane254()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double LocoCrane254::getHandlePosition() const
{
    return pos;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double LocoCrane254::getHandleShift() const
{
    return pos;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double LocoCrane254::getAirDistribPressure() const
{
    return getY(0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void LocoCrane254::init(double pTM, double pFL)
{
    Q_UNUSED(pTM)
    Q_UNUSED(pFL)
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void LocoCrane254::ode_system(const state_vector_t &Y,
                              state_vector_t &dYdt,
                              double t)
{
    Q_UNUSED(t)

    // Давление, задаваемое поворотом рукоятки
    double p_handle = K[3] * pf(pos);

    double u1 = hs_p(p_handle - Y[2]);

    double u2 = hs_n(p_handle - Y[2]);

    // Давление, задаваемое уравнительным органом крана
    double pz = p_handle * u1 + Y[2] * u2;

    double dp = pz - pBC;

    double u3 = cut(pf(k[1] * dp), 0.0, 1.0);

    double u4 = cut(nf(k[1] * dp), 0.0, 1.0);

    // Поток воздуха в ТЦ
    Qbc = K[1] * (pFL - pBC) * u3 - K[2] * pBC * u4;

    // Работа повторительной схемы

    double dp12 =  Y[0] - Y[1];

    double u5 = hs_n(dp12 - ps);

    double u6 = hs_n(pos) + is_release;

    double Qpz = K[7] * (Y[1] - Y[2]);

    double Q12 = K[5] * dp12 * u5;

    double Q1 = K[4] * Qvr;

    double Q2 = Q12 - Qpz - K[6] * Y[1] * u6;

    dYdt[0] = Q1 / V1; // p1

    dYdt[1] = Q2 / V2; // p2

    dYdt[2] = Qpz / Vpz; // p_pz

    stepSound();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void LocoCrane254::stepSound()
{
    p_volume = volume;

    // 250000 поправочный коэффициент для перевода 1кг/cм^3 в 1кг/м^3
    // Для звуков взято 400000 с малым запасом
    volume = abs(Qbc) * 400000;

    if (volume > 30)
    {
        if (Qbc > 0)
        {
            if (p_volume <= 30)
            {
                emit soundPlay("254_vpusk");
            }
            emit soundSetVolume("254_vypusk", 0);
            emit soundSetVolume("254_vpusk", volume);
        }

        if (Qbc < 0)
        {
            if (p_volume <= 30)
            {
                emit soundPlay("254_vypusk");
            }
            emit soundSetVolume("254_vpusk", 0);
            emit soundSetVolume("254_vypusk", volume);
        }
        isStop = false;
    }
    else
    {
        if (!isStop)
        {
            emit soundStop("254_vpusk");
            emit soundStop("254_vypusk");
            isStop = true;
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void LocoCrane254::load_config(CfgReader &cfg)
{
    QString secName = "Device";

    for (size_t i = 1; i < K.size(); ++i)
    {
        QString coeff = QString("K%1").arg(i);
        cfg.getDouble(secName, coeff, K[i]);
    }

    for (size_t i = 1; i < k.size(); ++i)
    {
        QString coeff = QString("k%1").arg(i);
        cfg.getDouble(secName, coeff, k[i]);
    }

    cfg.getDouble(secName, "V1", V1);
    cfg.getDouble(secName, "V2", V2);
    cfg.getDouble(secName, "Vpz", Vpz);

    cfg.getDouble(secName, "delta_p", delta_p);

    cfg.getDouble(secName, "ps", ps);

    QString tmp = "";

    cfg.getString(secName, "StepPressures", tmp);

    std::istringstream ss(tmp.toStdString());

    for (size_t i = 0; i < NUM_STEPS; ++i)
    {
        double step_press = 0.0;
        ss >> step_press;

        step_pressures[i] = step_press;
    }

    cfg.getDouble(secName, "MinPos", min_pos);
    cfg.getDouble(secName, "MaxPos", max_pos);
    cfg.getDouble(secName, "PosDuration", pos_duration);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void LocoCrane254::stepKeysControl(double t, double dt)
{
    Q_UNUSED(t)

    // Непрерывное движение ручки
    if (getKeyState(KEY_Leftbracket))
        dir = -1;
    else
    {
        if (pos < 0.0)
           pos = 0.0;

        if (getKeyState(KEY_Rightbracket))
            dir = 1;
        else
            dir = 0;
    }

    int old_pos_n = pos_num;

    pos += dir * pos_duration * dt;

    pos = cut(pos, min_pos, max_pos);

    // Дискретное движение от кнопок
    double max_step = *(step_pressures.end() - 1);

    if (isAlt())
    {
        if (getKeyState(KEY_8))
        {
            pos = step_pressures[0] / max_step;
        }

        if (getKeyState(KEY_9))
        {
            pos = step_pressures[1] / max_step;
        }

        if (getKeyState(KEY_0))
        {
            pos = step_pressures[2] / max_step;
        }

        if (getKeyState(KEY_Minus))
        {
            pos = step_pressures[3] / max_step;
        }

        if (getKeyState(KEY_Equals))
        {
            pos = step_pressures[4] / max_step;
        }
    }

    pos_num = getPositionNumber();

    if (pos_num != old_pos_n && pos_num != -1)
        emit soundPlay("254-chelk");
}

//------------------------------------------------------------------------------
// Позиция крана по канавкам (только для звуков)
//------------------------------------------------------------------------------
int LocoCrane254::getPositionNumber() const
{
    int pos_n = -1;

    if (pos == 0.0)
        return 0;

    if (pos == 1.0)
        return 4;

    for (uint i = 0; i < positions.size() - (dir == -1 ? 2 : 1); ++i)
    {
        if (pos >= positions[i] && pos <= positions[i+1])
            pos_n = static_cast<int>(i);
    }

    return pos_n;
}

GET_LOCO_CRANE(LocoCrane254)
