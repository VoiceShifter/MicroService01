# include "Functions.hpp"
# include <QSqlDatabase>
# include <QSqlError>
# include <QSqlQuery>
# include <iostream>
# include <sstream>
#include <QHttpServerResponse>


std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> MainMap;
std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> AnswerMap;
std::map<std::pair<API_PATH, API_METHOD>, std::function<signed int
        (std::map<std::string, std::string>&, std::vector<std::string>&)>> FunctionMap;

void NewDifferentiate(std::string& Path, std::map<std::string, std::string>& Params,
                      std::string& Method, std::string& ReturnMessage)
{
    signed int Error{0};
    if (MainMap.find(Path) == MainMap.end())
    {
        ReturnMessage = "No such path";
        Error = 1;
        goto End;
    }

    if (MainMap[Path].find(Method) == MainMap[Path].end())
    {
        ReturnMessage = "No such method";
        Error = 2;
        goto End;
    }
    if (MainMap[Path][Method].size() != Params.size())
    {
        ReturnMessage = "Wrong amount of params";
        Error = 3;
        goto End;
    }
    for (std::string const & vIterator : MainMap[Path][Method])
    {
        if (Params.find(vIterator) == Params.end())
        {
            ReturnMessage = "Wrong params";
            Error = 4;
        }
    }

    End:
    std::vector<std::string> Returns;
    Returns.reserve(5);
    if (Error != 0)
    {
        Returns.push_back("-1");
        FalseReturn(ReturnMessage, Error);
    }
    else
    {
        signed int ReturnCode = FunctionMap[std::make_pair(Path, Method)](Params, Returns);
        CreateReturn(Returns, Path, Method, ReturnMessage,
                     ReturnCode);
    }

}

signed int newAddUser(std::map<std::string, std::string>& Params, std::vector<std::string>& ReturnMessage)
{
    std::cout << "\nAdding new user\n";
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lExec;
    std::stringstream Query;
    Query << "INSERT INTO Users (Email, PhoneNumber, Password) values ('" <<
        Params["Email"] << "', '" << Params["PhoneNumber"] << "', '" << Params["Password"] << "');";
    std::cout << '\n' << Query.str() << '\n';

    if (!lExec.exec(Query.str().c_str()))
    {
        //lExec.next();
        ReturnMessage.push_back(""); //lExec.lastError().text().toStdString()
        std::cout << ReturnMessage[0];
        return 400;
    }
    ReturnMessage.push_back("0");
    Query.str("");
    Query << "select UUID from Users where Email = '" << Params["Email"] << "';";
    std::cout << Query.str() << " - this is a query\n";
    lExec.exec(Query.str().c_str());
    lExec.next();
    ReturnMessage.push_back(lExec.value(0).toString().toStdString());
    std::cout << "\nUSER ADDED\n";
    return 201;
}

signed int newDeleteUser(std::map<std::string, std::string>& Params, std::vector<std::string>& ReturnMessage)
{
    std::cout << "\nDeleting user\n";
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lExec;
    std::stringstream Query;
    Query << "update Users set isDeleted = 1 where Email = '" << Params["Email"]
          << "' and Password = '" << Params["Password"] << "';";
    std::cout << '\n' << Query.str() << '\n';

    if (!lExec.exec(Query.str().c_str()))
    {
        //lExec.next();
        ReturnMessage.push_back("");
        std::cout << ReturnMessage[0];
        return 400;

    }
    ReturnMessage.push_back("0");
    return 202;
}

signed int newAuthorizeUser(std::map<std::string, std::string>& Params, std::vector<std::string>& ReturnMessage)
{
    std::cout << "\nAuthorizing user\n";
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lSelect;
    std::stringstream Query;
    Query << "select * from Users where Email = '"
          << Params.at("Email") << "' and Password = '" << Params.at("Password") << "'" << "and isDeleted = 0" << ";";
    std::cout << Query.str();

    lSelect.exec(Query.str().c_str());
    if (lSelect.next() == true)
    {
        std::cout << "\nUSER AUTHORIZED\n";
        ReturnMessage.push_back("0");
        return 202;
    }
    else
    {
        std::cout << "\nNOT AUTHORIZED\n";
        ReturnMessage.push_back("1");
        return 401;
    }
    return 202;
}

signed int newAddSubscription(std::map<std::string, std::string> &Params, std::vector<std::string> &ReturnMessage)
{
    std::cout << "\nAdding subscription user\n";
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lSelect;
    std::stringstream Query;
    Query << "UPDATE Users SET StartDate = date('now'), EndDate = date('now', '+1 month')  WHERE Email='"
          << Params.at("Email") << "' and Password = '" << Params.at("Password") << "'"
          << "and isDeleted = 0 and StartDate == 0" << ";";
    std::cout << Query.str();

    lSelect.exec(Query.str().c_str());
    ReturnMessage.push_back("0");
    Query.str("");
    Query << "select StartDate, EndDate from Users where Email = '" << Params["Email"]
          << "' and Password = '" << Params.at("Password") << "'" << "and isDeleted = 0" << ";";
    std::cout << '\n' << Query.str() << " - this is a query\n";
    lSelect.exec(Query.str().c_str());
    lSelect.next();
    ReturnMessage.push_back(lSelect.value(0).toString().toStdString());
    ReturnMessage.push_back(lSelect.value(1).toString().toStdString());
    return 202;
}

void CreateReturn(std::vector<std::string> &Answers, std::string& Path,
                  std::string& Method, std::string& ReturnMessage, signed int Code)
{
    std::stringstream Response{};
    // QHttpServerResponse qResponse{};
    // qResponse.addHeader("OK", 200);
    Response << "HTTP/1.0 " << Code << " OK\nContent-Type: application/json\n{\n";


    for (size_t Index{}; Index < Answers.size(); ++Index)
    {
        Response << '"' << AnswerMap[Path][Method][Index] << '"' << " : "
             << '"' << Answers[Index] << '"' << '\n';
    }
    Response << "}\n";
    std::cout << Response.str() << '\n'; //remove later
    ReturnMessage = Response.str();
}

void FalseReturn(std::string &ReturnMessage, signed int State)
{
    std::stringstream Response{};
    Response << "{\n";
    switch (State) {
    case 1:
        Response << "\"Error\" : \"Wrong path\"\n" ;
        break;
    case 2:
        Response << "\"Error\" : \"Wrong Method\"\n" ;
        break;
    case 3:
        Response << "\"Error\" : \"Wrong Params\"\n" ;
        break;
    default:
        Response << "\"Error\" : \"How did you get here\"\n" ;
        break;
    }
    Response << '}';
    std::cout << Response.str(); //remove later
    ReturnMessage = Response.str();
}


