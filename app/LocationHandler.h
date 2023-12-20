#pragma once
#include "DBHandler.h"

class LocationHandler
{
public:
    explicit LocationHandler(DBHandler &dbHandler);

    void handle_requests(httplib::Server &svr);

private:
    DBHandler &db;

    void list_locations(const httplib::Request &req, httplib::Response &res);
    void add_location(const httplib::Request &req, httplib::Response &res);
    void update_location(const httplib::Request &req, httplib::Response &res);
    void delete_location(const httplib::Request &req, httplib::Response &res);
};