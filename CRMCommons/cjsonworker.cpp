#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "cjsonworker.h"
#include "customer.h"

CJsonWorker::CJsonWorker()
{
    networkAccessManager = new QNetworkAccessManager();
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CJsonWorker::onDataLoaded);
}

CJsonWorker *CJsonWorker::getInstance()
{
    if (!_cJsonWorker) {
        _cJsonWorker = new CJsonWorker();
    }
    return _cJsonWorker;
}

void CJsonWorker::authenticate(QString login, QString password)
{
    qDebug() << "Trying to authenticate...";

    QString loginUrl = API_URL;
    loginUrl.append(AUTHENTICATE_URL);

    QUrl loginQUrl(loginUrl);
    QNetworkRequest request(loginQUrl);
    request.setAttribute(QNetworkRequest::User, CRequestType::AUTHENTICATION);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonObject;
    jsonObject.insert("username", login);
    jsonObject.insert("password", password);

    networkAccessManager->post(request, QJsonDocument(jsonObject).toJson());
}

void CJsonWorker::getAllMenus()
{
    QString menuUrl = API_URL;
    menuUrl.append(MENUS_URL);
    QUrl menuQUrl(menuUrl);

    QNetworkRequest request(menuQUrl);
    request.setAttribute(QNetworkRequest::User, CRequestType::MENUS);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(AUTHORIZATION_HEADER, getTokenBearer().toLocal8Bit());

    networkAccessManager->get(request);
}

void CJsonWorker::getAllCustomers()
{
    QString localUrl = API_URL;
    localUrl.append(CUSTOMERS_URL);
    QUrl localQUrl(localUrl);

    QNetworkRequest request(localQUrl);
    request.setAttribute(QNetworkRequest::User, CRequestType::CUSTOMERS);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(AUTHORIZATION_HEADER, getTokenBearer().toLocal8Bit());

    networkAccessManager->get(request);
}

void CJsonWorker::saveCustomer(CModels::Customer *customer)
{
    QString localUrl = API_URL;
    localUrl.append(SAVE_CUSTOMER_URL);
    QUrl localQUrl(localUrl);

    QNetworkRequest request(localQUrl);
    request.setAttribute(QNetworkRequest::User, CRequestType::SAVE_CUSTOMER);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(AUTHORIZATION_HEADER, getTokenBearer().toLocal8Bit());

    QJsonObject jsonObject;
    jsonObject.insert(JSON_CSTMR_FULLNAME, customer->getFullName());
    jsonObject.insert(JSON_CSTMR_COMMENT, customer->getComment());
    jsonObject.insert(JSON_CSTMR_EMAIL, customer->getEmail());
    jsonObject.insert(JSON_CSTMR_FAX, customer->getFax());
//    jsonObject.insert(JSON_CSTMR_ID, customer->getId());
    jsonObject.insert(JSON_CSTMR_PHONE, customer->getPhone());
    jsonObject.insert(JSON_CSTMR_SOURCE, customer->getSource());
//    jsonObject.insert(JSON_CSTMR_TYPEID, customer->getTypeId());

    qDebug() << "Post Customer JSON object to the server.";
    networkAccessManager->post(request, QJsonDocument(jsonObject).toJson());
}

void CJsonWorker::getAllTasks()
{
    QString localUrl = API_URL;
    localUrl.append(TASKS_URL);
    QUrl localQUrl(localUrl);

    QNetworkRequest request(localQUrl);
    request.setAttribute(QNetworkRequest::User, CRequestType::TASKS);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(AUTHORIZATION_HEADER, getTokenBearer().toLocal8Bit());

    networkAccessManager->get(request);
}

QString CJsonWorker::getTokenBearer()
{
    QString token(BEARER_PART);
    token.append(globalObject->getToken());
    return token;
}

void CJsonWorker::onDataLoaded(QNetworkReply *reply) {
    CRequestType requestType = static_cast<CRequestType>
            (reply->request().attribute(QNetworkRequest::User).toInt());

    switch (requestType)
    {
        case CRequestType::AUTHENTICATION:
            onAuthenticateDataLoaded(reply);
            break;
        case CRequestType::MENUS:
            onMenuDataLoaded(reply);
            break;
        case CRequestType::CUSTOMERS:
            onCustomersDataLoaded(reply);
            break;
        case CRequestType::TASKS:
            onTasksDataLoaded(reply);
            break;
        default:
            break;
    }
    reply->deleteLater();
}

void CJsonWorker::onAuthenticateDataLoaded(QNetworkReply *reply)
{
    qDebug() << "Server sent back the data.";
    emit onAuthenticateFinished(reply);
}

void CJsonWorker::onMenuDataLoaded(QNetworkReply *reply)
{
    qDebug() << "Menu was loaded from server...";
    emit onMenuLoadFinished(reply);
}

void CJsonWorker::onCustomersDataLoaded(QNetworkReply *reply)
{
    qDebug() << "Customers was loaded from server...";
    emit onCustomersLoadFinished(reply);
}

void CJsonWorker::onTasksDataLoaded(QNetworkReply *reply)
{
    qDebug() << "Customers was loaded from server...";
    emit onTasksLoadFinished(reply);
}
