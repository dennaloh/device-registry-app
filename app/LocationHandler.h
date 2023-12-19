#pragma once
#include "DBHandler.h"
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

class LocationHandler
{
public:
    explicit LocationHandler(DBHandler &dbHandler);

    void handleRequests(httplib::Server &svr);

private:
    DBHandler &db;

    void listLocations(const httplib::Request &req, httplib::Response &res);
    void addLocation(const httplib::Request &req, httplib::Response &res);
};