#pragma once
#include "DBHandler.h"
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

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
    bool isValidDate(const std::string &date);
};