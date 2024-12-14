# include "Functions.hpp"
# include <QSqlDatabase>
# include <QSqlError>
# include <QSqlQuery>
# include <iostream>
# include <sstream>

std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> MainMap;
std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> AnswerMap;
std::map<std::pair<API_PATH, API_METHOD>, std::function<void(std::map<std::string, std::string>)>> FunctionMap;

void NewDifferentiate(std::string& Path, std::map<std::string, std::string>& Params, std::string& Method,
                      std::string& ReturnMessage)
{
    const auto& Iterator = MainMap.find(Path);
    if (Iterator == MainMap.end())
    {
        ReturnMessage = "No such path";
        return;
    }
    if (Iterator->second.find(Method) == Iterator->second.end())
    {
        ReturnMessage = "No such method";
        return;
    }
    if (Params.size() != Iterator->second.size())
    {
        ReturnMessage = "Wrong amount of params";
        return;
    }
    for (std::string const & vIterator : Iterator->second[Method])
    {
        if (Params.find(vIterator) == Params.end())
        {
            ReturnMessage = "Wrong params";
            return;
        }
    }
    FunctionMap[{Path, Method}](std::ref(Params));
}

void newAddUser(std::map<std::string, std::string>& Params, std::string &ReturnMessage)
{
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
        ReturnMessage = lExec.lastError().text().toStdString();
        std::cout << ReturnMessage;
        return;
    }
    ReturnMessage = "0\n";
    Query.str("");
    Query << "select UUID from Users where Email = '" << Params["Email"] << "';";
    std::cout << Query.str() << " - this is a query\n";
    lExec.exec(Query.str().c_str());
    lExec.next();
    ReturnMessage += lExec.value(0).toString().toStdString();
    std::cout << "\nUSER ADDED\n";
    return;
}

void newDeleteUser(std::map<std::string, std::string>& Params, std::string &ReturnMessage)
{
    return;
}

void newAuthorizeUser(std::map<std::string, std::string>& Params, std::string &ReturnMessage)
{
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lSelect;
    std::stringstream Query;
    Query << "select * from Users where Email = '"
          << Params.at("Email") << "' and Password = '" << Params.at("Password") << "';";
    std::cout << Query.str();

    lSelect.exec(Query.str().c_str());
    if (lSelect.next() == true)
    {
        std::cout << "\nUSER AUTHORIZED\n";
        ReturnMessage = "0";
        return;
    }
    else
    {
        std::cout << "\nNOT AUTHORIZED\n";
        ReturnMessage = "1";
        return;
    }
    return;
}

