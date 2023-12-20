#include "DBHandler.h"
#include "DeviceHandler.h"
#include "LocationHandler.h"

int main()
{
    DBHandler dbHandler("registry.db");
    if (!dbHandler.open_connection())
    {
        std::cout << "Failed to connect to database" << std::endl;
        return 1;
    }
    std::cout << "Connected to database." << std::endl;

    httplib::Server svr;
    DeviceHandler deviceHandler(dbHandler);
    LocationHandler locationHandler(dbHandler);

    deviceHandler.handle_requests(svr);
    locationHandler.handle_requests(svr);

    svr.listen("0.0.0.0", 8080);

    dbHandler.close_connection();
    return 0;
}