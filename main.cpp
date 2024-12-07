# include <QCoreApplication>
# include <iostream>
# include <QtNetwork/QTcpServer>
# include <QtNetwork/QTcpSocket>
# include <QtNetwork/QHostAddress>
# include <QtCore>
# include <QTimer>
# include <chrono>
# include <thread>
# include <QSqlDatabase>

std::mutex CoutMutex{};

void ProcessVector(std::vector<QTcpSocket*>& SocketVector)
{
    std::cout << "Thread started\n";
    using namespace std::chrono_literals;

    QTcpSocket* lPointer{};
    std::string lMessage{};
    for (;;)
    {
        if (SocketVector.empty())
        {

            std::this_thread::sleep_for(500ms);
            CoutMutex.lock();
            std::cout << "Is empty\n";
            CoutMutex.unlock();
            continue;
        }
        std::cout << "SocketVector is not empty!\n";
        lPointer = SocketVector[SocketVector.size() - 1];
        if (!lPointer->waitForReadyRead()) {
            std::cerr << "Failed to read from socket: " << lPointer->errorString().toStdString() << std::endl;
            continue;
        }
        QByteArray Data = lPointer->readAll();
        lMessage = Data.toStdString();
        std::cout << lMessage << '\n';

        char* Token{ std::strtok(const_cast<char*>(lMessage.c_str()), " ") };
        std::vector<std::string> Tokens{};
        for (size_t Iterator{}; Token != nullptr; ++Iterator)
        {
            Tokens.push_back(Token);
            Token = std::strtok(nullptr, " ");
        }
        CoutMutex.lock();
        for (const auto& Iterator : Tokens)
        {
            std::cout << Iterator << ' ';
        }
        std::cout << "\n MESSAGE READ \n";
        CoutMutex.unlock();


        if (Tokens[1].size() < 2) //standart path
        {
            lMessage = "{\"This is a \": \"test path\"}";
            lPointer->write(lMessage.c_str());
            std::cout << "Standart path entered\n Message Sent\n";
        }
        else
        {
            std::cout << "\n NOT STANDART \n";
            size_t PathEnd{Tokens[1].find_last_of('/')};
            if (PathEnd == 0) //FUCK FAVICON
            {
                lMessage = "/";
                lPointer->waitForDisconnected(30);
                lPointer->close();
                delete lPointer;
                SocketVector.pop_back();
                std::cout << "\nFUCK FAVICON\n";
                return;
            }
            std::string ApiPath{Tokens[1].substr(1, PathEnd)},
                         Params{Tokens[1].substr(PathEnd, Tokens[1].size())};
            std::cout << ApiPath << " - api path\n" << Params << " - params\n";

            if (strcmp(ApiPath.c_str(), "Api/User/Register/"))
            {
                lMessage = "You registered user\n";
            }
            else if (strcmp(ApiPath.c_str(), "Api/User/Auth/"))
            {
                lMessage = "You authorized\n";
            }
            else if (strcmp(ApiPath.c_str(), "Api/User/Delete/"))
            {
                lMessage = "You deleted user\n";
            }
            else if (strcmp(ApiPath.c_str(), "favicon.ico"))
            {
                lMessage = "/";
            }
            lPointer->write(lMessage.c_str());
            std::cout << "\n MESSAGE SENT\n";
        }
        lPointer->waitForDisconnected(30);
        lPointer->close();
        delete lPointer;
        SocketVector.pop_back();
        std::cout << "\n CONNECTION TERMINATED \n";
    }
}

signed int main(int argc, char *argv[])
{
    std::vector<QTcpSocket*> Sockets{};
    QTcpServer MainSocket {};
    MainSocket.listen(QHostAddress::Any, 32323);
    std::mutex VectorMutex{};

    QObject::connect(&MainSocket, &QTcpServer::newConnection, [&MainSocket, &Sockets, &VectorMutex]()
    {
        VectorMutex.lock();
        Sockets.push_back(MainSocket.nextPendingConnection());        
        VectorMutex.unlock();
    });
    std::thread ProcessingThread(&ProcessVector, std::ref(Sockets));
    for(;;)
    {
        MainSocket.waitForNewConnection(-1);
    }
    ProcessingThread.detach();
    std::cout << "Server is listening on port 32323\n";
    return 0;
}
