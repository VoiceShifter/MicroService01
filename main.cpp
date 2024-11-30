# include <QCoreApplication>
# include <iostream>
# include <QtNetwork/QTcpServer>
# include <QtNetwork/QTcpSocket>
# include <QtNetwork/QHostAddress>
# include <QtCore>
# include <QTimer>
# include <chrono>
# include <thread>

static void ProcessConnection(QTcpSocket* lSocket)
{
    //std::cout << "Processing connection\n";
    //std::cout << lSocket->socketDescriptor() << '\n';
    //socket->open(QIODeviceBase::ReadWrite);
    std::string lMessage{};
    if (lSocket->waitForReadyRead()) {
        QByteArray data = lSocket->readAll(); // Read data from the socket
        lMessage = data.toStdString();
        std::cout << lMessage;
    } else {
        std::cerr << "Failed to read from socket: " << lSocket->errorString().toStdString() << std::endl;
    }


    //Out << socket->readAll(); // Read data from the socket
    //std::cout << "Received data: " << data.toStdString() << std::endl;
    lSocket->close(); // Close the socket
    delete lSocket;
}

void ProcessVector(std::vector<QTcpSocket*>& SocketVector)
{
    using namespace std::chrono_literals;
    QTcpSocket* lPointer{};
    std::string lMessage{};
    for (;;)
    {
        if (SocketVector.empty())
        {
            std::this_thread::sleep_for(1s);
            continue;
        }
        lPointer = SocketVector[SocketVector.size() - 1];
        if (!lPointer->waitForReadyRead()) {
            std::cerr << "Failed to read from socket: " << lPointer->errorString().toStdString() << std::endl;
            continue;
        }
        QByteArray Data = lPointer->readAll();
        lMessage = Data.toStdString();
        std::cout << lMessage;
    }
}

signed int main(int argc, char *argv[])
{
    std::vector<QTcpSocket*> Sockets{};
    QTcpServer MainSocket {};
    MainSocket.listen(QHostAddress::Any, 32323);

    QObject::connect(&MainSocket, &QTcpServer::newConnection, [&MainSocket, &Sockets]()
    {
        Sockets.push_back(MainSocket.nextPendingConnection());
        std::cout << Sockets.size();
        ProcessConnection(Sockets[0]);
    });
    MainSocket.waitForNewConnection(-1);
    std::cout << "Server is listening on port 32323\n";
    return 0;
}
