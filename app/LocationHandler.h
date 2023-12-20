#pragma once
#include "DBHandler.h"

class LocationHandler
{
public:
    explicit LocationHandler(DBHandler &dbHandler);

    void handleRequests(httplib::Server &svr);

private:
    DBHandler &db;

    void listLocations(const httplib::Request &req, httplib::Response &res);
    void addLocation(const httplib::Request &req, httplib::Response &res);
    void updateLocation(const httplib::Request &req, httplib::Response &res);
    void deleteLocation(const httplib::Request &req, httplib::Response &res);
};