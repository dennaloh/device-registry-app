#include "DBHandler.h"

DBHandler::DBHandler(const std::string &dbPath) : db(nullptr), dbPath(dbPath) {}

DBHandler::~DBHandler()
{
    closeConnection();
}

bool DBHandler::openConnection()
{
    int rc = sqlite3_open(dbPath.c_str(), &db);
    return (rc == SQLITE_OK);
}

void DBHandler::closeConnection()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

// LOCATIONS TABLE OPERATIONS
// 1. List all locations
std::vector<Location> DBHandler::getAllLocations()
{
    std::string sql = "SELECT * FROM locations";

    std::vector<Location> locations;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return locations;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Location location = extractLocationData(stmt);
        locations.push_back(location);
    }

    sqlite3_finalize(stmt);
    return locations;
}

// 2. Add new location
bool DBHandler::addLocation(const Location &location)
{
    std::string sql;
    if (location.id == -1)
    {
        sql = "INSERT INTO locations (name, type) VALUES (?, ?)";
    }
    else
    {
        sql = "INSERT INTO locations (id, name, type) VALUES (?, ?, ?)";
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    bindLocationData(stmt, location);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// 3. Update a location
bool DBHandler::updateLocation(const int id, const std::string &name, const std::string &type)
{
    std::string sql = "UPDATE locations SET ";
    if (!name.empty())
    {
        sql += "name = ?, ";
    }
    if (!type.empty())
    {
        sql += "type = ?, ";
    }
    sql.pop_back();
    sql.pop_back();
    sql += "WHERE id = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    int bind_index = 1;
    if (!name.empty())
    {
        sqlite3_bind_text(stmt, bind_index++, name.c_str(), -1, SQLITE_STATIC);
    }
    if (!type.empty())
    {
        sqlite3_bind_text(stmt, bind_index++, type.c_str(), -1, SQLITE_STATIC);
    }
    sqlite3_bind_int(stmt, bind_index++, id);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// 4. Delete a location: All devices with this location id will be deleted as well.
bool DBHandler::deleteLocation(const int id)
{
    std::string sqlDevices = "DELETE FROM devices WHERE location_id = ?;";
    sqlite3_stmt *stmtDevices;
    int rcDevices = sqlite3_prepare_v2(db, sqlDevices.c_str(), -1, &stmtDevices, NULL);
    if (rcDevices != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement for devices: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmtDevices, 1, id);
    rcDevices = sqlite3_step(stmtDevices);

    sqlite3_finalize(stmtDevices);

    std::string sqlLocation = "DELETE FROM locations WHERE id = ?;";
    sqlite3_stmt *stmtLocation;
    int rcLocation = sqlite3_prepare_v2(db, sqlLocation.c_str(), -1, &stmtLocation, NULL);
    if (rcLocation != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement for locations: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmtLocation, 1, id);
    rcLocation = sqlite3_step(stmtLocation);

    sqlite3_finalize(stmtLocation);

    return (rcDevices == SQLITE_DONE && rcLocation == SQLITE_DONE);
}

// HELPER METHODS
// public methods
bool DBHandler::locationExists(int location_id)
{
    std::string sql = "SELECT COUNT(*) FROM locations WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, location_id);
    rc = sqlite3_step(stmt);
    int count = 0;
    if (rc == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return (count > 0);
}

// private methods
void DBHandler::bindLocationData(sqlite3_stmt *stmt, const Location &location)
{
    int parameterIndex = 1;

    if (location.id != -1)
    {
        sqlite3_bind_int(stmt, parameterIndex++, location.id);
    }

    sqlite3_bind_text(stmt, parameterIndex++, location.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, parameterIndex, location.type.c_str(), -1, SQLITE_STATIC);
}

Location DBHandler::extractLocationData(sqlite3_stmt *stmt)
{
    Location location;
    location.id = sqlite3_column_int(stmt, 0);
    location.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    location.type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    return location;
}