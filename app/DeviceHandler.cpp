#include "DeviceHandler.h"

DeviceHandler::DeviceHandler(DBHandler &dbHandler) : db(dbHandler) {}

void DeviceHandler::listDevices(const httplib::Request &req, httplib::Response &res)
{
    json response;
    auto devices = db.getAllDevices();

    if (devices.empty())
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "No devices found in devices table";
        res.set_content(response.dump(), "application/json");
        return;
    }

    res.status = 200;
    json response_content;
    for (const auto &device : devices)
    {
        json dev_info = {
            {"serial_number", device.serial_number},
            {"name", device.name},
            {"type", device.type},
            {"creation_date", device.creation_date},
            {"location_id", device.location_id},
            {"location_name", device.location_name},
            {"location_type", device.location_type}};
        response_content.push_back(dev_info);
    }
    res.set_content(response_content.dump(), "application/json");
}

void DeviceHandler::filterDevices(const httplib::Request &req, httplib::Response &res)
{
    json response;

    const std::set<std::string> validFilters = {"serial_number", "name", "type", "creation_date", "location_id",
                                                "start_date", "end_date", "location_name", "location_type"};

    bool hasValidFilter = std::any_of(validFilters.begin(), validFilters.end(),
                                      [&req](const std::string &param)
                                      { return req.has_param(param); });

    if (!hasValidFilter)
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid request parameters: Only serial_number, name, type, creation_date, location_id, start_date, end_date, location_name, location_type";
        res.set_content(response.dump(), "application/json");
        return;
    }

    std::string serial_number = req.get_param_value("serial_number");
    std::string name = req.get_param_value("name");
    std::string type = req.get_param_value("type");
    std::string creation_date = req.get_param_value("creation_date");
    std::string location_id_str = req.get_param_value("location_id");
    std::string start_date = req.get_param_value("start_date");
    std::string end_date = req.get_param_value("end_date");
    std::string location_name = req.get_param_value("location_name");
    std::string location_type = req.get_param_value("location_type");

    auto filtered_devices = db.filterDevices(serial_number, name, type, creation_date, location_id_str, start_date, end_date, location_name, location_type);

    if (filtered_devices.empty())
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "No devices match filters";
    }
    else
    {
        res.status = 200;
        for (const auto &device : filtered_devices)
        {
            json device_info = {
                {"serial_number", device.serial_number},
                {"name", device.name},
                {"type", device.type},
                {"creation_date", device.creation_date},
                {"location_id", device.location_id},
                {"location_name", device.location_name},
                {"location_type", device.location_type}};
            response.push_back(device_info);
        }
    }

    res.set_content(response.dump(), "application/json");
}

void DeviceHandler::addDevice(const httplib::Request &req, httplib::Response &res)
{
    Device newDevice;
    json response;

    if (!(req.has_param("serial_number") && req.has_param("name") && req.has_param("type") && req.has_param("creation_date") && req.has_param("location_id")))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid request parameters: Must have serial_number, name, type, creation_date and location_id";
        res.set_content(response.dump(), "application/json");
        return;
    }

    newDevice.name = req.get_param_value("name");
    newDevice.type = req.get_param_value("type");
    try
    {
        newDevice.serial_number = std::stoi(req.get_param_value("serial_number"));
        if (db.serialNumExists(newDevice.serial_number))
        {
            res.status = 409;
            response["status"] = "conflict";
            response["message"] = "Serial Number already exists in devices table";
            res.set_content(response.dump(), "application/json");
            return;
        }
    }
    catch (...)
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid serial_number";
        res.set_content(response.dump(), "application/json");
        return;
    }
    newDevice.creation_date = req.get_param_value("creation_date");
    if (!isValidDate(newDevice.creation_date))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid creation_date";
        res.set_content(response.dump(), "application/json");
        return;
    }
    try
    {
        newDevice.location_id = std::stoi(req.get_param_value("location_id"));
        if (!db.locationExists(newDevice.location_id))
        {
            res.status = 404;
            response["status"] = "not found";
            response["message"] = "Location ID does not exist in locations table";
            res.set_content(response.dump(), "application/json");
            return;
        }
    }
    catch (...)
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid location_id";
        res.set_content(response.dump(), "application/json");
        return;
    }

    auto added = db.addDevice(newDevice);

    if (added)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "New device created successfully";
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to create new device in DBHandler";
    }

    res.set_content(response.dump(), "application/json");
}

void DeviceHandler::updateDevice(const httplib::Request &req, httplib::Response &res)
{
    json response;

    if (!(req.has_param("name") || req.has_param("type") || req.has_param("creation_date") || req.has_param("location_id")))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid request parameters: Only name &/or type &/or creation_date &/or location_id";
        res.set_content(response.dump(), "application/json");
        return;
    }

    std::string name = req.get_param_value("name");
    std::string type = req.get_param_value("type");
    int serial_number = std::stoi(req.matches[1]);
    if (!db.serialNumExists(serial_number))
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "Serial Number does not exist in devices table";
        res.set_content(response.dump(), "application/json");
        return;
    }
    std::string creation_date = req.get_param_value("creation_date");
    if (!creation_date.empty() && !isValidDate(creation_date))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid creation_date";
        res.set_content(response.dump(), "application/json");
        return;
    }
    std::string location_id = req.get_param_value("location_id");
    try
    {
        if (!location_id.empty() && !db.locationExists(std::stoi(location_id)))
        {
            res.status = 404;
            response["status"] = "not found";
            response["message"] = "Location ID does not exist in locations table";
            res.set_content(response.dump(), "application/json");
            return;
        }
    }
    catch (...)
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid location_id";
        res.set_content(response.dump(), "application/json");
        return;
    }

    auto updated = db.updateDevice(serial_number, name, type, creation_date, location_id);

    if (updated)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "Updated device successfully";
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to update device in DBHandler";
    }

    res.set_content(response.dump(), "application/json");
}

void DeviceHandler::deleteDevice(const httplib::Request &req, httplib::Response &res)
{
    json response;

    int serial_number = std::stoi(req.matches[1]);
    if (!db.serialNumExists(serial_number))
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "Serial Number does not exist in devices table";
        res.set_content(response.dump(), "application/json");
        return;
    }

    auto deleted = db.deleteDevice(serial_number);

    if (deleted)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "Deleted device successfully";
        res.set_content(response.dump(), "application/json");
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to delete device in DBHandler";
        res.set_content(response.dump(), "application/json");
    }
}

void DeviceHandler::handleRequests(httplib::Server &svr)
{
    svr.Get("/devices", [&](const httplib::Request &req, httplib::Response &res)
            { listDevices(req, res); });

    svr.Get("/devices/filter", [&](const httplib::Request &req, httplib::Response &res)
            { filterDevices(req, res); });

    svr.Post("/devices/new", [&](const httplib::Request &req, httplib::Response &res)
             { addDevice(req, res); });

    svr.Put(R"(/devices/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            { updateDevice(req, res); });

    svr.Delete(R"(/devices/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               { deleteDevice(req, res); });
}

// Helper methods
bool DeviceHandler::isValidDate(const std::string &dateStr)
{
    std::tm timeStruct = {0};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&timeStruct, "%Y-%m-%d");

    if (ss.fail())
    {
        return false;
    }

    time_t result = mktime(&timeStruct);
    return result != -1;
}