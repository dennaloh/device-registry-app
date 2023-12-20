#include "DBHandler.h"
#include "DeviceHandler.h"
#include "LocationHandler.h"

int main()
{
    DBHandler dbHandler("registry.db");
    if (!dbHandler.openConnection())
    {
        std::cout << "Failed to connect to database" << std::endl;
        return 1;
    }
    std::cout << "Connected to database." << std::endl;

    httplib::Server svr;
    DeviceHandler deviceHandler(dbHandler);
    LocationHandler locationHandler(dbHandler);

    deviceHandler.handleRequests(svr);
    locationHandler.handleRequests(svr);

    svr.listen("0.0.0.0", 8080);

    dbHandler.closeConnection();
    return 0;
}