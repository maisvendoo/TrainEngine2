//------------------------------------------------------------------------------
//
//      Train's brakepipe model
//      (с) maisvendoo, 06/11/2016
//      Developer: Dmitry Pritykin
//
//------------------------------------------------------------------------------
/*!
 *  \file
 *  \brief Train's brakepipe model
 *  \copyright maisvendoo
 *  \author Dmitry Pritykin
 *  \date 06/11/2016
 */

#ifndef BRAKEPIPE_H
#define BRAKEPIPE_H

#include    <QString>
#include    <QtGlobal>

#include    "physics.h"

#include    <vector>

#if defined(BRAKEPIPE_LIB)
    #define BRAKEPIPE_EXPORT    Q_DECL_EXPORT
#else
    #define BRAKEPIPE_EXPORT    Q_DECL_IMPORT
#endif

/*!
 *  \class
 *  \brief Brakepipe model
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class BRAKEPIPE_EXPORT BrakePipe
{
public:

    /// Constructor
    BrakePipe();
    /// Destructor
    ~BrakePipe();

    /// Set brakepipe length
    void setLength(double L);

    /// Set nodes count
    void setNodesNum(size_t N);

    /// Set pressure in begin node of pipe
    void setBeginPressure(double p0);

    /// Set pressure rate in node
    void setAuxRate(size_t i, double auxRate);

    /// PDE integration step
    bool step(double t, double dt);

    /// Initialization
    bool init(QString cfg_path);

    /// Get pressure in node
    double getPressure(size_t i);

private:

    double lambda;          ///< Air friction coefficient
    double d;               ///< Pipe tube diameter
    double T;               ///< Air temperature
    double muf;             ///< Equal hode area (for 1 meter of pipe)

    size_t N;                  ///< Nodes count

    double c0;
    double a;
    double a1;
    double L;               ///< Length of pipe tube

    std::vector<double> p;              ///< Pressures in nodes
    std::vector<double> V;              ///< Pressure rate in node

    std::vector<double> A;              ///< Bottom matrix diagonal of node's equations
    std::vector<double> B;              ///< Top matrix diagonal of node's equations
    std::vector<double> C;              ///< Main matrix diagonal of node's equations
    std::vector<double> f;              ///< Right part column

    double h;

    /// Leak function
    double Q(size_t i);

    /// Config loading
    bool loadCfg(QString cfg_path);
};

#endif // BRAKEPIPE_H
