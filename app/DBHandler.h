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
    DBHandler(const std::string &dbPath);
    ~DBHandler();
    bool openConnection();
    void closeConnection();

    // DEVICES TABLE OPERATIONS
    std::vector<Device> getAllDevices();
    std::vector<Device> filterDevices(const std::string &serial_number, const std::string &name, const std::string &type,
                                      const std::string &creation_date, const std::string &location_id, const std::string &start_date,
                                      const std::string &end_date, const std::string &location_name, const std::string &location_type);
    bool addDevice(const Device &device);
    bool updateDevice(const std::string &serial_number, const std::string &name, const std::string &type, const std::string &creation_date,
                      const std::string &location_id);
    bool deleteDevice(const std::string &serial_number);

    // LOCATIONS TABLE OPERATIONS
    std::vector<Location> getAllLocations();
    bool addLocation(const Location &location);
    bool updateLocation(const int id, const std::string &name, const std::string &type);
    bool deleteLocation(const int id);

    // Helper methods
    bool serialNumExists(std::string &serial_num);
    bool locationExists(int location_id);

private:
    sqlite3 *db;
    std::string dbPath;

    // Helper methods
    void bindDeviceData(sqlite3_stmt *stmt, const Device &device);
    Device extractDeviceData(sqlite3_stmt *stmt);
    void bindLocationData(sqlite3_stmt *stmt, const Location &location);
    Location extractLocationData(sqlite3_stmt *stmt);
};