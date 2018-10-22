#include <stdio.h>
#include <string.h>

#include "library.h"

#define CLIENT_ID    "Alice"
#define SYSTEM       "channel"
#define TIMEOUT     10000L

int myCallBack( const char* sSessID, int nJob, const char* sJob) {

    printf("%s\n", sSessID);
    printf("%i\n", nJob);
    printf("%s\n", sJob);

    return 42;
}

int main() {

    // initialise driver interface and register a callback function
    const char* retCode = ELICreate("lic", "0.9", myCallBack );
    printf("ELICreate() => %s\n\n", retCode);

    // dump the driver-info to console
    printf("%s\n",ELIDriverInfo());

    // dump product-info to console
    printf("%s\n",ELIProductInfo("productid"));

    // dump system-info to console
    printf("%s\n",ELISystemInfo("users"));

    // call ELIDriverUI
    ELIDriverUI( "sessionID", "SID");

    // open connection to hardware (in this case MQTT broker)
    const char* session = ELIOpen("UserList", SYSTEM, CLIENT_ID);
    printf("Session : %s\n", session);

    int rc = ELIApp2Drv( session, 4711, "Job");


    // close connection to hardware (i.e. MQTT broker disconnect)
    printf("%s\n", ELIClose(session));

    // destroy the driver interface
    ELIDestroy();
    return 0;
}