#include "LocationHandler.h"

LocationHandler::LocationHandler(DBHandler &dbHandler) : db(dbHandler) {}

void LocationHandler::list_locations(const httplib::Request &req, httplib::Response &res)
{
    json response;
    auto locations = db.get_locations();

    if (locations.empty())
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "No locations found in locations table";
        res.set_content(response.dump(), "application/json");
        return;
    }

    res.status = 200;
    json response_content;
    for (const auto &location : locations)
    {
        json loc_info = {
            {"id", location.id},
            {"name", location.name},
            {"type", location.type}};
        response_content.push_back(loc_info);
    }

    res.set_content(response_content.dump(), "application/json");
}

void LocationHandler::add_location(const httplib::Request &req, httplib::Response &res)
{
    Location newLocation;
    json response;

    if (!(req.has_param("name") && req.has_param("type")))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid request parameters: Must have at least both name and type";
        res.set_content(response.dump(), "application/json");
        return;
    }

    newLocation.name = req.get_param_value("name");
    newLocation.type = req.get_param_value("type");
    newLocation.id = -1; // Default value for ID
    if (req.has_param("id"))
    {
        try
        {
            newLocation.id = std::stoi(req.get_param_value("id"));
            if (db.location_exists(newLocation.id))
            {
                res.status = 409;
                response["status"] = "conflict";
                response["message"] = "Location ID already exists in locations table";
                res.set_content(response.dump(), "application/json");
                return;
            }
        }
        catch (...)
        {
            res.status = 400;
            response["status"] = "invalid";
            response["message"] = "Invalid id";
            res.set_content(response.dump(), "application/json");
            return;
        }
    }

    bool added = db.add_location(newLocation);

    if (added)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "New location created successfully: " + newLocation.name + " | " + newLocation.type;
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to create new location in DBHandler";
    }

    res.set_content(response.dump(), "application/json");
}

void LocationHandler::update_location(const httplib::Request &req, httplib::Response &res)
{
    json response;

    if (!(req.has_param("name") || req.has_param("type")))
    {
        res.status = 400;
        response["status"] = "invalid";
        response["message"] = "Invalid request parameters: Only name &/or type";
        res.set_content(response.dump(), "application/json");
        return;
    }

    std::string name = req.get_param_value("name");
    std::string type = req.get_param_value("type");
    int id = std::stoi(req.matches[1]);
    if (!db.location_exists(id))
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "Location ID does not exist in locations table";
        res.set_content(response.dump(), "application/json");
        return;
    }

    auto updated = db.update_location(id, name, type);

    if (updated)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "Successfully updated location: " + std::to_string(id);
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to update location in DBHandler";
    }

    res.set_content(response.dump(), "application/json");
}

void LocationHandler::delete_location(const httplib::Request &req, httplib::Response &res)
{
    json response;

    int id = std::stoi(req.matches[1]);
    if (!db.location_exists(id))
    {
        res.status = 404;
        response["status"] = "not found";
        response["message"] = "Location ID does not exist in locations table";
        res.set_content(response.dump(), "application/json");
        return;
    }

    auto deleted = db.delete_location(id);

    if (deleted)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "Successfully deleted location and devices with location id " + std::to_string(id);
        res.set_content(response.dump(), "application/json");
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to delete location and devices with this location id in DBHandler";
        res.set_content(response.dump(), "application/json");
    }
}

void LocationHandler::handle_requests(httplib::Server &svr)
{
    svr.Get("/locations", [&](const httplib::Request &req, httplib::Response &res)
            { list_locations(req, res); });

    svr.Post("/locations", [&](const httplib::Request &req, httplib::Response &res)
             { add_location(req, res); });

    svr.Patch(R"(/locations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
              { update_location(req, res); });

    svr.Delete(R"(/locations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               { delete_location(req, res); });
}