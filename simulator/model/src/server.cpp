//------------------------------------------------------------------------------
//
//      Server to commect with viewer
//      (c) maisvendoo, 18/12/2018
//
//------------------------------------------------------------------------------
/*!
 * \file
 * \brief Server to commect with viewer
 * \copyright maisvendoo
 * \author maisvendoo
 * \date 18/12/2018
 */

#include    "server.h"

#include    "tcp-server.h"
#include    "client-delegates.h"
#include    "abstract-data-engine.h"
#include    "data-engine.h"

#include    <QTimer>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Server::Server(QObject *parent) : QObject (parent)
  , server(Q_NULLPTR)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Server::~Server()
{
    delete server;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Server::init(quint16 port)
{
    server = new TcpServer();

    // Set data engine for viewer
    server->setEngineDefiner(new DataEngine());

    connect(server, &TcpServer::clientAuthorized,
            this, &Server::clientAuthorized, Qt::AutoConnection);

    connect(server, &TcpServer::clientAboutToDisconnect,
            this, &Server::clientDisconnected, Qt::AutoConnection);

    // Start server on required port
    server->start(port);

    if (server->isListening())
        emit logMessage(QString("OK: Server started on port %1").arg(port));
    else
        emit logMessage("ERROR: Server start fail");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
QByteArray Server::getReceivedData()
{
    return server->getClient("viewer")->getInputBuffer();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Server::sendDataToClient(QByteArray data)
{
    if (clients.client)
    {
        clients.client->setOutputBuffer(data);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Server::clientAuthorized(ClientFace *clnt)
{
    QString clientName = clnt->getName();

    if (clientName == "viewer")
    {
        clients.client = clnt;
        clients.client->setOutputBuffer(QString("Hello").toUtf8());
        emit logMessage("OK: Authorized client: " + clientName);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Server::clientDisconnected(ClientFace *clnt)
{
    if (clnt == clients.client)
    {
        emit logMessage("OK: Disconnected client: " + clnt->getName());
        clients.client = Q_NULLPTR;
    }
}
