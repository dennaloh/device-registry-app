#pragma once
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>

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

    // LOCATIONS TABLE OPERATIONS
    std::vector<Location> getAllLocations();
    bool addLocation(const Location &location);
    bool updateLocation(const int id, const std::string &name, const std::string &type);
    bool deleteLocation(const int id);

    // Helper methods
    bool locationExists(int location_id);

private:
    sqlite3 *db;
    std::string dbPath;

    // Helper methods
    void bindLocationData(sqlite3_stmt *stmt, const Location &location);
    Location extractLocationData(sqlite3_stmt *stmt);
};