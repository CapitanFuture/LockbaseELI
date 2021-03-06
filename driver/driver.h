#include "MQTTClient.h"
#include "library.h"
#include "session_list.h"

#ifndef DRIVER_H
#define DRIVER_H

#if defined (WIN32)
    #if defined(LbwELI_Demo_EXPORTS)
        #define LBELI_EXPORT __declspec(dllexport)
    #else
        #define  LBELI_EXPORT __declspec(dllimport)
    #endif
#else
    #define LBELI_EXPORT
#endif

typedef struct driver_info {
    ELIDrv2App  callback;
    MQTTClient client;
    node_t * sessions;
    char * config;
    char * host;
    char * productInfo;
    char * systemInfo;
    char * driverInfo;
    long port;
} driver_info_t;

LBELI_EXPORT extern driver_info_t * driverInfo;

driver_info_t * new_driver(ELIDrv2App callBack);
void free_driver(driver_info_t * driver);

void parseProductInfo(const char* json, const char* sProductID, char** productInfo);
void parseSystemInfo(const char* json, char** systemInfo);
void parseDriverInfo(const char* json, char** driverInfo);

#endif //DRIVER_H