#pragma once
#include <sqlite3.h>
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;

struct Device
{
    std::string serial_number;
    std::string name;
    std::string type;
    std::string creation_date;
    int location_id;
    std::string location_name;
    std::string location_type;
};

struct Location
{
    int id;
    std::string name;
    std::string type;
};

class DBHandler
{
public:
    DBHandler(const std::string &db_path);
    ~DBHandler();
    bool open_connection();
    void close_connection();

    // DEVICES TABLE OPERATIONS
    std::vector<Device> get_devices();
    std::vector<Device> filter_devices(const std::string &serial_number, const std::string &name, const std::string &type,
                                       const std::string &creation_date, const std::string &location_id, const std::string &start_date,
                                       const std::string &end_date, const std::string &location_name, const std::string &location_type);
    bool add_device(const Device &device);
    bool update_device(const std::string &serial_number, const std::string &name, const std::string &type, const std::string &creation_date,
                       const std::string &location_id);
    bool delete_device(const std::string &serial_number);

    // LOCATIONS TABLE OPERATIONS
    std::vector<Location> get_locations();
    bool add_location(const Location &location);
    bool update_location(const int id, const std::string &name, const std::string &type);
    bool delete_location(const int id);

    // Helper methods
    bool serial_num_exists(std::string &serial_num);
    bool location_exists(int location_id);

private:
    sqlite3 *db;
    std::string dbPath;

    // Helper methods
    void bind_device_data(sqlite3_stmt *stmt, const Device &device);
    Device extract_device_data(sqlite3_stmt *stmt);
    void bind_location_data(sqlite3_stmt *stmt, const Location &location);
    Location extract_location_data(sqlite3_stmt *stmt);
};