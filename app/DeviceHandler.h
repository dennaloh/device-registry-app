#pragma once
#include "DBHandler.h"

class DeviceHandler
{
public:
    explicit DeviceHandler(DBHandler &dbHandler);

    void handle_requests(httplib::Server &svr);

private:
    DBHandler &db;

    void list_devices(const httplib::Request &req, httplib::Response &res);
    void filter_devices(const httplib::Request &req, httplib::Response &res);
    void add_device(const httplib::Request &req, httplib::Response &res);
    void update_device(const httplib::Request &req, httplib::Response &res);
    void delete_device(const httplib::Request &req, httplib::Response &res);

    // Helper methods
    std::string get_today_date();
    bool is_valid_date(const std::string &date);
    bool is_alphanumeric(const std::string &date);
};