#include "LocationHandler.h"

LocationHandler::LocationHandler(DBHandler &dbHandler) : db(dbHandler) {}

void LocationHandler::listLocations(const httplib::Request &req, httplib::Response &res)
{
    json response;
    auto locations = db.getAllLocations();

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

void LocationHandler::addLocation(const httplib::Request &req, httplib::Response &res)
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
            if (db.locationExists(newLocation.id))
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

    bool added = db.addLocation(newLocation);

    if (added)
    {
        res.status = 200;
        response["status"] = "success";
        response["message"] = "New location created successfully";
    }
    else
    {
        res.status = 500;
        response["status"] = "error";
        response["message"] = "Failed to create new location in DBHandler";
    }

    res.set_content(response.dump(), "application/json");
}

void LocationHandler::handleRequests(httplib::Server &svr)
{
    svr.Get("/locations", [&](const httplib::Request &req, httplib::Response &res)
            { listLocations(req, res); });

    svr.Post("/locations/new", [&](const httplib::Request &req, httplib::Response &res)
             { addLocation(req, res); });
}