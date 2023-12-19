#include "DBHandler.h"
#include "LocationHandler.h"

int main()
{
    DBHandler dbHandler("registry.db");
    std::cout << "Before connection" << std::endl;
    if (!dbHandler.openConnection())
    {
        std::cout << "Failed to connect to database" << std::endl;
        return 1;
    }
    std::cout << "After connection" << std::endl;

    httplib::Server svr;
    LocationHandler locationHandler(dbHandler);

    locationHandler.handleRequests(svr);

    svr.listen("0.0.0.0", 8080);

    dbHandler.closeConnection();
    return 0;
}