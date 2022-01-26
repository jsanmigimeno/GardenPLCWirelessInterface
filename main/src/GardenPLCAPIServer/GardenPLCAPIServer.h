#ifndef GardenPLCAPIServer_h
#define GardenPLCAPIServer_h

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "../GardenPLCSerialComm/GardenPLCSerialComm.h"

#define SERVER_PORT 80

static const uint8_t IRRIGATION_GROUP_NAME_LENGTH = 16;
static const uint8_t IRRIGATION_GROUP_COUNT       = 10;

enum ContentType { TEXT, JSON };

enum ResponseType { BOOL, INT, CSTR };

struct Request {
    const char     responseKeyName[30];
    const uint8_t  instructionAddress;
    const uint8_t* requestPtr;
    const uint8_t  requestSize;
    const uint8_t  responseSize;
    const ResponseType responseType;
};

class GardenPLCAPIServer
{
    public:
        GardenPLCAPIServer();
        void startServer();
        void handleClient();

    private:
        ESP8266WebServer server = SERVER_PORT;
        GardenPLCSerialComm communicator;

        void setRoutes();

        // Generic request handling functions
        void handleGetBoolRequest(uint8_t instructionAddress);
        void handlePostBoolRequest(uint8_t instructionAddress);
        void handleGetIntRequest(uint8_t instructionAddress, uint8_t size);
        void handlePostIntRequest(uint8_t instructionAddress, uint8_t size);

        void handleGetIrrGroupBoolRequest(uint8_t instructionAddress);
        void handlePostIrrGroupBoolRequest(uint8_t instructionAddress);
        void handleGetIrrGroupIntRequest(uint8_t instructionAddress, uint8_t size);
        void handlePostIrrGroupIntRequest(uint8_t instructionAddress, uint8_t size);
        void handleGetIrrGroupCStrRequest(uint8_t instructionAddress, uint8_t size);
        void handlePostIrrGroupCStrRequest(uint8_t instructionAddress, uint8_t size);
        
        // Specific request handling functions
        void handleRootRequest();
        void handlePLCStateRequest();
        void handlePLCChangesRequest();
        void handleSwimmingPoolStateRequest();
        void handleIrrigationStateRequest();
        void handleIrrigationGroupsStateRequest();
        void handleIrrigationGroupStateRequest();

        void getJSONString(const Request* requestArrPtr, const uint8_t& requestCount, String& output);
        void handleJSONResponseRequest(const Request* requestArrPtr, const uint8_t& requestCount);

        // HTTP response helpers
        void respondSuccessString(const String& successString);
        void respondSuccessJson(const String& jsonString);
        void respondSuccess(const String& string, const ContentType& responseType);
        void respondBadRequest();
        void respondNotFound();
        void respondServerError();
};
#endif