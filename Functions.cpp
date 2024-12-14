# include "Functions.hpp"
# include <QSqlDatabase>
# include <QSqlError>
# include <QSqlQuery>
# include <iostream>
# include <sstream>

std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> MainMap;
std::unordered_map<API_PATH, std::unordered_map<API_METHOD, std::vector<API_PARAM>>> AnswerMap;
std::map<std::pair<API_PATH, API_METHOD>, std::function<void(std::map<std::string, std::string>)>> FunctionMap;

void newAddUser(std::map<std::string, std::string>& Params, std::string &ReturnMessage)
{

}

void newDeleteUser(std::map<std::string, std::string>& Params, std::string &ReturnMessage)
{

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

bool AuthorizeUser(std::string& Email, std::string& Password)
{
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lSelect;
    std::stringstream Query;
    Query << "select * from Users where Email = '" << Email << "' and Password = '" << Password
          << "';";
    std::cout << Query.str();

    lSelect.exec(Query.str().c_str());
    if (lSelect.next() == true)
    {
        std::cout << "\nUSER AUTHORIZED\n";
        return true;
    }
    else
    {
        std::cout << "\nNOT AUTHORIZED\n";
        return false;
    }
    return false;
}



void AddUser(std::string& Email, std::string& Password, std::string& PhoneNumber)
{
    QSqlDatabase Users{QSqlDatabase::addDatabase("QSQLITE")};
    Users.setDatabaseName("Users.sqlite");
    if (!Users.open()) {
        qDebug() << Users.lastError().text();
    }
    QSqlQuery lInsert;
    std::stringstream Query;
    Query << "INSERT INTO Users (Email, PhoneNumber, Password) values ('" <<
        Email << "', '" << PhoneNumber << "', '" << Password << "');";
    std::cout << '\n' << Query.str() << '\n';
    //lInsert.exec(Query.str().c_str());
    std::cout << "\nUSER ADDED\n";
}

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

void Differentiate(std::string& Path, std::map<std::string, std::string>& Params, std::string& Method,
                   std::string& ReturnMessage)
{
    if (Path == "Api/User/Auth/")
    {
        if (Params.find("Email") == Params.end() || Params.find("Password") == Params.end())
        {
            ReturnMessage = "Wrong params";
        }
        else if (AuthorizeUser(Params["Email"], Params["Password"]))
        {
            ReturnMessage = "You authorized\n";
        }
        else
        {
            ReturnMessage = "Access denied\n";
        }
    }
    else if (Path =="Api/User/Register/")
    {
        if (Params.find("Email") == Params.end() || Params.find("Password") == Params.end() ||
            Params.find("PhoneNumber") == Params.end())
        {
            ReturnMessage = "Wrong params";
        }
        else
        {
            ReturnMessage = "You registered user\n";
            AddUser(Params["Email"], Params["Password"], Params["PhoneNumber"]);
        }

    }
    else if (Path == "Api/User/Delete/")
    {
        ReturnMessage = "You deleted user\n";
    }
    else
    {
        ReturnMessage = "No such method";
    }
}



