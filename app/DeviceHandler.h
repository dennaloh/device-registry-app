#pragma once
#include "DBHandler.h"

class DeviceHandler
{
public:
    explicit DeviceHandler(DBHandler &dbHandler);

    void handleRequests(httplib::Server &svr);

private:
    DBHandler &db;

    void listDevices(const httplib::Request &req, httplib::Response &res);
    void filterDevices(const httplib::Request &req, httplib::Response &res);
    void addDevice(const httplib::Request &req, httplib::Response &res);
    void updateDevice(const httplib::Request &req, httplib::Response &res);
    void deleteDevice(const httplib::Request &req, httplib::Response &res);

    // Helper methods
    std::string getTodayDate();
    bool isValidDate(const std::string &dateStr);
    bool isAlphanumeric(const std::string &str);
};