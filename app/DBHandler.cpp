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

// DEVICES TABLE OPERATIONS
// 1. List all devices
std::vector<Device> DBHandler::getAllDevices()
{
    std::string sql = "SELECT devices.serial_number, devices.name, devices.type, devices.creation_date, devices.location_id, locations.name, locations.type"
                      " FROM devices INNER JOIN locations ON devices.location_id = locations.id ";

    std::vector<Device> devices;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return devices;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Device device = extractDeviceData(stmt);
        devices.push_back(device);
    }

    sqlite3_finalize(stmt);
    return devices;
}

// 2. Filter by metadata: Only serial_number, name, type, creation_date, location_id, start_date, end_date, location_name, location_type.
std::vector<Device> DBHandler::filterDevices(const std::string &serial_number, const std::string &name, const std::string &type,
                                             const std::string &creation_date, const std::string &location_id, const std::string &start_date,
                                             const std::string &end_date, const std::string &location_name, const std::string &location_type)
{
    std::string sql = "SELECT devices.serial_number, devices.name, devices.type, devices.creation_date, devices.location_id, locations.name, locations.type"
                      " FROM devices INNER JOIN locations ON devices.location_id = locations.id "
                      "WHERE 1=1";
    if (!serial_number.empty())
    {
        sql += " AND devices.serial_number = '" + serial_number + "'";
    }
    if (!type.empty())
    {
        sql += " AND devices.type = '" + type + "'";
    }
    if (!start_date.empty())
    {
        sql += " AND devices.creation_date >= '" + start_date + "'";
    }
    if (!end_date.empty())
    {
        sql += " AND devices.creation_date <= '" + end_date + "'";
    }
    if (!location_name.empty())
    {
        sql += " AND locations.name = '" + location_name + "'";
    }
    if (!location_type.empty())
    {
        sql += " AND locations.type = '" + location_type + "'";
    }
    if (!location_id.empty())
    {
        sql += " AND location_id =  " + location_id;
    }

    std::vector<Device> filtered_devices;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return filtered_devices;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Device device = extractDeviceData(stmt);
        filtered_devices.push_back(device);
    }

    sqlite3_finalize(stmt);
    return filtered_devices;
}

// 3. Add new device
bool DBHandler::addDevice(const Device &device)
{
    std::string sql = "INSERT INTO devices (serial_number, name, type, creation_date, location_id) VALUES (?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    bindDeviceData(stmt, device);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// 4. Update a device: Must be integer serial number. To update serial number, need to delete and post again.
bool DBHandler::updateDevice(const std::string &serial_number, const std::string &name, const std::string &type, const std::string &creation_date, const std::string &location_id)
{
    std::string sql = "UPDATE devices SET ";
    if (!name.empty())
    {
        sql += "name = ?, ";
    }
    if (!type.empty())
    {
        sql += "type = ?, ";
    }
    if (!creation_date.empty())
    {
        sql += "creation_date = ?, ";
    }
    if (!location_id.empty())
    {
        sql += "location_id = ?, ";
    }
    sql.pop_back();
    sql.pop_back();
    sql += "WHERE serial_number = ?";

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
    if (!creation_date.empty())
    {
        sqlite3_bind_text(stmt, bind_index++, creation_date.c_str(), -1, SQLITE_STATIC);
    }
    if (!location_id.empty())
    {
        sqlite3_bind_text(stmt, bind_index++, location_id.c_str(), -1, SQLITE_STATIC);
    }
    sqlite3_bind_text(stmt, bind_index++, serial_number.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
}

// 5. Delete a device
bool DBHandler::deleteDevice(const std::string &serial_number)
{
    std::string sql = "DELETE FROM devices WHERE serial_number = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, serial_number.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE);
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
bool DBHandler::serialNumExists(std::string &serial_num)
{
    std::string sql = "SELECT COUNT(*) FROM devices WHERE serial_number = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, serial_num.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    int count = 0;
    if (rc == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return (count > 0);
}

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
void DBHandler::bindDeviceData(sqlite3_stmt *stmt, const Device &device)
{
    sqlite3_bind_text(stmt, 1, device.serial_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, device.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, device.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, device.creation_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, device.location_id);
}

Device DBHandler::extractDeviceData(sqlite3_stmt *stmt)
{
    Device device;
    device.serial_number = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    device.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    device.type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    device.creation_date = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    device.location_id = sqlite3_column_int(stmt, 4);
    device.location_name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
    device.location_type = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));
    return device;
}

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