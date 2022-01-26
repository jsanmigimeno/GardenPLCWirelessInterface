#include "GardenPLCAPIServer.h"
#include "ProtocolDefinition.h"

GardenPLCAPIServer::GardenPLCAPIServer() {
    setRoutes();
}

void GardenPLCAPIServer::startServer() {
    server.begin();
}

void GardenPLCAPIServer::handleClient() {
    server.handleClient();
}

void GardenPLCAPIServer::setRoutes() {

    server.on("/",                              HTTP_GET,  [this](){handleRootRequest();});

    // PLC Global
    server.on("/state",                         HTTP_GET,  [this](){handlePLCStateRequest();});
    server.on("/changes",                       HTTP_GET,  [this](){handlePLCChangesRequest();});
    server.on("/auto-value",                    HTTP_GET,  [this](){handleGetBoolRequest(GET_AUTO_VALUE_ADDR);});

    server.on("/clock",                         HTTP_GET,  [this](){handleGetIntRequest(GET_CLOCK_ADDR, 4);});
    server.on("/clock",                         HTTP_POST, [this](){handlePostIntRequest(SET_CLOCK_ADDR, 4);});


    // Swimming Pool
    server.on("/sp-state" ,                     HTTP_GET,  [this](){handleSwimmingPoolStateRequest();});
    server.on("/sp-last-change" ,               HTTP_GET,  [this](){handleGetIntRequest(SP_GET_LAST_CHANGE_ADDR, 4);});
    server.on("/sp-controller-state" ,          HTTP_GET,  [this](){handleGetIntRequest(SP_GET_CONTROLLER_STATE_ADDR, 1);});
    server.on("/sp-pump-state",                 HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_PUMP_STATE_ADDR);});
    server.on("/sp-uv-state",                   HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_UV_STATE_ADDR);});
    server.on("/sp-pump-manual-value",          HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_PUMP_MANUAL_VALUE_ADDR);});
    server.on("/sp-uv-enable-value",            HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_UV_ENABLE_VALUE_ADDR);});
    server.on("/sp-flow-sensor-value",          HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_FLOW_SENSOR_VALUE_ADDR);});
    server.on("/sp-pump-manual-disable",        HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_PUMP_MANUAL_DISABLE_ADDR);});

    server.on("/sp-schedule-enable",            HTTP_GET,  [this](){handleGetBoolRequest(SP_GET_SCHEDULE_ENABLE_ADDR);});
    server.on("/sp-schedule-enable",            HTTP_POST, [this](){handlePostBoolRequest(SP_SET_SCHEDULE_ENABLE_ADDR);});
    server.on("/sp-schedule-next",              HTTP_GET,  [this](){handleGetIntRequest(SP_GET_SCHEDULE_NEXT_ADDR, 4);});
    server.on("/sp-schedule-next",              HTTP_POST, [this](){handlePostIntRequest(SP_SET_SCHEDULE_NEXT_ADDR, 4);});
    server.on("/sp-schedule-duration",          HTTP_GET,  [this](){handleGetIntRequest(SP_GET_SCHEDULE_DURATION_ADDR, 2);});
    server.on("/sp-schedule-duration",          HTTP_POST, [this](){handlePostIntRequest(SP_SET_SCHEDULE_DURATION_ADDR, 2);});
    server.on("/sp-schedule-period",            HTTP_GET,  [this](){handleGetIntRequest(SP_GET_SCHEDULE_PERIOD_ADDR, 1);});
    server.on("/sp-schedule-period",            HTTP_POST, [this](){handlePostIntRequest(SP_SET_SCHEDULE_PERIOD_ADDR, 1);});
    server.on("/sp-schedule-reset",             HTTP_POST, [this](){handlePostIntRequest(SP_REQ_SCHEDULE_RESET_ADDR, 2);});


    // Irrigation
    server.on("/irr-state" ,                    HTTP_GET,  [this](){handleIrrigationStateRequest();});
    server.on("/irr-last-change" ,              HTTP_GET,  [this](){handleGetIntRequest(IRR_GET_LAST_CHANGE_ADDR, 4);});
    server.on("/irr-controller-state" ,         HTTP_GET,  [this](){handleGetIntRequest(IRR_GET_CONTROLLER_STATE_ADDR, 1);});
    server.on("/irr-pump-state",                HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_PUMP_STATE_ADDR);});
    server.on("/irr-mains-inlet-state",         HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_MAINS_INLET_STATE_ADDR);});
    server.on("/irr-manual-value",              HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_MANUAL_VALUE_ADDR);});
    server.on("/irr-pressure-sensor-value",     HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_PRESSURE_SENSOR_VALUE_ADDR);});
    server.on("/irr-manual-disable",            HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_MANUAL_DISABLE_STATE_ADDR);});
    server.on("/irr-zones-state",               HTTP_GET,  [this](){handlePostIntRequest(IRR_GET_ZONES_STATE_ADDR, 2);});

    server.on("/irr-manual-zones",              HTTP_GET,  [this](){handleGetIntRequest(IRR_GET_MANUAL_ZONES_ADDR, 2);});
    server.on("/irr-manual-zones",              HTTP_POST, [this](){handlePostIntRequest(IRR_SET_MANUAL_ZONES_ADDR, 2);});
    server.on("/irr-manual-source",             HTTP_GET,  [this](){handleGetIntRequest(IRR_GET_MANUAL_SOURCE_ADDR, 1);});
    server.on("/irr-manual-source",             HTTP_POST, [this](){handlePostIntRequest(IRR_SET_MANUAL_SOURCE_ADDR, 1);});

    server.on("/irr-schedule-enable",           HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_SCHEDULE_ENABLE_ADDR);});
    server.on("/irr-schedule-enable",           HTTP_POST, [this](){handlePostBoolRequest(IRR_SET_SCHEDULE_ENABLE_ADDR);});
    server.on("/irr-schedule-paused-state",     HTTP_GET,  [this](){handleGetBoolRequest(IRR_GET_SCHEDULE_PAUSED_STATE_ADDR);});
    server.on("/irr-schedule-pause-timestamp",  HTTP_POST, [this](){handlePostIntRequest(IRR_SET_SCHEDULE_PAUSE_TIMESTAMP_ADDR, 4);});
    server.on("/irr-schedule-resume",           HTTP_POST, [this](){handlePostIntRequest(IRR_REQ_SCHEDULE_RESUME_ADDR, 1);});
    server.on("/irr-schedule-next-irr-time",    HTTP_POST, [this](){handlePostIntRequest(IRR_GET_NEXT_IRRIGATION_TIME_ADDR, 4);});

    server.on("/irr-groups-state",              HTTP_GET,  [this](){handleIrrigationGroupsStateRequest();});
    server.on("/irr-group-state" ,              HTTP_GET,  [this](){handleIrrigationGroupStateRequest();});
    server.on("/irr-schedule-groups-state",     HTTP_GET,  [this](){handleGetIntRequest(IRR_GET_SCHEDULE_GROUPS_STATE_ADDR, 2);});
    server.on("/irr-schedule-group-state",      HTTP_GET,  [this](){handleGetIrrGroupBoolRequest(IRR_GET_SCHEDULE_GROUP_STATE_ADDR);});
    server.on("/irr-schedule-group-state",      HTTP_POST, [this](){handlePostIrrGroupBoolRequest(IRR_SET_SCHEDULE_GROUP_STATE_ADDR);});
    server.on("/irr-schedule-group-name",       HTTP_GET,  [this](){handleGetIrrGroupCStrRequest(IRR_GET_SCHEDULE_GROUP_NAME_ADDR, IRRIGATION_GROUP_NAME_LENGTH);});
    server.on("/irr-schedule-group-name",       HTTP_POST, [this](){handlePostIrrGroupCStrRequest(IRR_SET_SCHEDULE_GROUP_NAME_ADDR, IRRIGATION_GROUP_NAME_LENGTH);});
    server.on("/irr-schedule-group-zones",      HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_ZONES_ADDR, 2);});
    server.on("/irr-schedule-group-zones",      HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_SET_SCHEDULE_GROUP_ZONES_ADDR, 2);});
    server.on("/irr-schedule-group-source",     HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_SOURCE_ADDR, 1);});
    server.on("/irr-schedule-group-source",     HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_SET_SCHEDULE_GROUP_SOURCE_ADDR, 1);});
    server.on("/irr-schedule-group-period",     HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_PERIOD_ADDR, 1);});
    server.on("/irr-schedule-group-period",     HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_SET_SCHEDULE_GROUP_PERIOD_ADDR, 1);});
    server.on("/irr-schedule-group-duration",   HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_DURATION_ADDR, 2);});
    server.on("/irr-schedule-group-duration",   HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_SET_SCHEDULE_GROUP_DURATION_ADDR, 2);});
    server.on("/irr-schedule-group-init-time",  HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_INIT_TIME_ADDR, 2);});
    server.on("/irr-schedule-group-init-time",  HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_SET_SCHEDULE_GROUP_INIT_TIME_ADDR, 2);});
    server.on("/irr-schedule-group-next-time",  HTTP_GET,  [this](){handleGetIrrGroupIntRequest(IRR_GET_SCHEDULE_GROUP_NEXT_TIME_ADDR, 4);});
    server.on("/irr-schedule-group-reset",      HTTP_POST, [this](){handlePostIrrGroupIntRequest(IRR_REQ_SCHEDULE_GROUP_RESET_ADDR, 2);});

    server.on("/irr-schedule-reset",            HTTP_POST, [this](){handlePostIntRequest(IRR_REQ_SCHEDULE_RESET_ADDR, 2);});

    server.on("/irr-schedule-group-now",        HTTP_POST, [this](){handlePostIntRequest(IRR_REQ_SCHEDULE_GROUP_NOW_ADDR, 1);});

    server.on("/irr-cancel-current",            HTTP_POST, [this](){handlePostIntRequest(IRR_REQ_CANCEL_CURRENT_JOB_ADDR, 1);});
    server.on("/irr-cancel-all",                HTTP_POST, [this](){handlePostIntRequest(IRR_REQ_CANCEL_ALL_JOBS_ADDR, 1);});

    server.onNotFound([this](){respondNotFound();});

    server.enableCORS(true);
}



// Generic request handling functions *******************************************************************************************
void GardenPLCAPIServer::handleGetBoolRequest(uint8_t instructionAddress) {
    handleGetIntRequest(instructionAddress, 1);
}

void GardenPLCAPIServer::handlePostBoolRequest(uint8_t instructionAddress) {
    handlePostIntRequest(instructionAddress, 1);
}

void GardenPLCAPIServer::handleGetIntRequest(uint8_t instructionAddress, uint8_t size) {
    uint32_t response;
    const bool success = communicator.communicate(instructionAddress, {NULL, 0}, {(uint8_t*) &response, size});

    if (success) respondSuccessString(String(response));
    else         respondServerError();
}

void GardenPLCAPIServer::handlePostIntRequest(uint8_t instructionAddress, uint8_t size) {
    if (!server.hasArg("value")) return respondBadRequest();

    const uint32_t value = strtoul(server.arg("value").c_str(), NULL, 10);
    const bool sucess = communicator.communicate(instructionAddress, {(uint8_t*) &value, size}, {NULL, 0});

    if (sucess) respondSuccessString(String(""));
    else        respondServerError();
}


void GardenPLCAPIServer::handleGetIrrGroupBoolRequest(uint8_t instructionAddress) {
    handleGetIrrGroupIntRequest(instructionAddress, 1);
}

void GardenPLCAPIServer::handlePostIrrGroupBoolRequest(uint8_t instructionAddress) {
    handlePostIrrGroupIntRequest(instructionAddress, 1);
}

void GardenPLCAPIServer::handleGetIrrGroupIntRequest(uint8_t instructionAddress, uint8_t size) {
    if (!server.hasArg("groupIdx")) return respondBadRequest();


    uint8_t groupIdx = strtoul(server.arg("groupIdx").c_str(), NULL, 10);
    uint32_t response;
    const bool success = communicator.communicate(instructionAddress, {&groupIdx, 1}, {(uint8_t*) &response, size});

    if (success) respondSuccessString(String(response));
    else         respondServerError();
}

void GardenPLCAPIServer::handlePostIrrGroupIntRequest(uint8_t instructionAddress, uint8_t size) {
    if (!server.hasArg("groupIdx") || !server.hasArg("value")) return respondBadRequest();

    const uint8_t  groupIdx = strtoul(server.arg("groupIdx").c_str(), NULL, 10);
    const uint32_t value    = strtoul(server.arg("value").c_str(), NULL, 10);

    Payload requestPayload1 = {(uint8_t*) &groupIdx, 1};
    Payload requestPayload2 = {(uint8_t*) &value, size};
    const bool success = communicator.communicate<2, 0>(instructionAddress, {&requestPayload1, &requestPayload2}, {});

    if (success) respondSuccessString(String(""));
    else         respondServerError();
}

void GardenPLCAPIServer::handleGetIrrGroupCStrRequest(uint8_t instructionAddress, uint8_t size) {
    if (!server.hasArg("groupIdx")) return respondBadRequest();

    uint8_t groupIdx = strtoul(server.arg("groupIdx").c_str(), NULL, 10);
    char response[size+1] = {0}; // Make sure last element is the null character

    const bool success = communicator.communicate(instructionAddress, {&groupIdx, 1}, {(uint8_t*) &response, size});

    if (success) respondSuccessString(String(response));
    else         respondServerError();
}

void GardenPLCAPIServer::handlePostIrrGroupCStrRequest(uint8_t instructionAddress, uint8_t size) {
    if (!server.hasArg("groupIdx") || !server.hasArg("value")) return respondBadRequest();

    uint8_t groupIdx    = strtoul(server.arg("groupIdx").c_str(), NULL, 10);
    Payload requestPayload1 = {(uint8_t*) &groupIdx, 1};

    const char * value  = server.arg("value").c_str();
    uint8_t valueLength = strlen(value);
    char buffer[size] = {0};
    memcpy(&buffer, value, min(valueLength, size));
    Payload requestPayload2 = {(uint8_t*) &buffer, size};

    const bool success = communicator.communicate<2, 0>(instructionAddress, {&requestPayload1, &requestPayload2}, {});

    if (success) respondSuccessString(String(""));
    else         respondServerError();
}



// Specific request handling functions ******************************************************************************************

void GardenPLCAPIServer::handleRootRequest() {
    if (server.method() == HTTP_OPTIONS) {
        // CORS headers (preflight)
        server.sendHeader("Access-Control-Allow-Headers", "*");
        server.sendHeader("Access-Control-Max-Age", "10000");
        server.sendHeader("Access-Control-Allow-Methods", "POST,GET");
        server.send(204);
    }
    else server.send(200, "text/plain", "GardenPLC");
}

void GardenPLCAPIServer::handlePLCStateRequest() {
    const Request request[] = {
        {"auto",    GET_AUTO_VALUE_ADDR, NULL, 0, 1, ResponseType::BOOL},
        {"clock",   GET_CLOCK_ADDR,      NULL, 0, 4, ResponseType::INT }
    };

    handleJSONResponseRequest(request, 2);
}

void GardenPLCAPIServer::handlePLCChangesRequest() {
    const Request request[] = {
        {"clock",           GET_CLOCK_ADDR,             NULL, 0, 4, ResponseType::INT},
        {"global",          GET_PLC_LAST_CHANGE_ADDR,   NULL, 0, 4, ResponseType::INT},
        {"swimmingPool",    SP_GET_LAST_CHANGE_ADDR,    NULL, 0, 4, ResponseType::INT},
        {"irrigation",      IRR_GET_LAST_CHANGE_ADDR,   NULL, 0, 4, ResponseType::INT}
    };

    handleJSONResponseRequest(request, 4);
}

void GardenPLCAPIServer::getJSONString(const Request* requestArrPtr, const uint8_t& requestCount, String& output) {
    StaticJsonDocument<300> doc;

    char stringResponseBuffer[IRRIGATION_GROUP_NAME_LENGTH];

    for (uint8_t i = 0; i < requestCount; i++) {
        const Request& request = requestArrPtr[i];
        communicator.initNewTransmission(request.instructionAddress);
        if (request.requestSize != 0) communicator.writeRequestPayload(request.requestPtr, request.requestSize);
        
        if(!communicator.sendRequest()) return respondServerError();

        switch (request.responseType) {
            case ResponseType::BOOL:
                doc[request.responseKeyName] = communicator.readResponsePayloadInt(1);
                break;
            case ResponseType::INT:
                doc[request.responseKeyName] = communicator.readResponsePayloadInt(request.responseSize);
                break;
            case ResponseType::CSTR:
                communicator.readResponsePayload((uint8_t*) stringResponseBuffer, request.responseSize);
                stringResponseBuffer[request.responseSize-1] = 0; // Make sure last character is the null character
                doc[request.responseKeyName] = stringResponseBuffer;
                break;
        }
    }

    serializeJson(doc, output);
}

void GardenPLCAPIServer::handleJSONResponseRequest(const Request* requestArrPtr, const uint8_t& requestCount) {
    String output = "";
    getJSONString(requestArrPtr, requestCount, output);
    respondSuccessString(output);
}

void GardenPLCAPIServer::handleSwimmingPoolStateRequest() {
    const Request request[] = {
        {"changeTimestamp",     SP_GET_LAST_CHANGE_ADDR,         NULL, 0, 4, ResponseType::INT },
        {"state",               SP_GET_CONTROLLER_STATE_ADDR,    NULL, 0, 1, ResponseType::BOOL},
        {"pump",                SP_GET_PUMP_STATE_ADDR,          NULL, 0, 1, ResponseType::BOOL},
        {"uv",                  SP_GET_UV_STATE_ADDR,            NULL, 0, 1, ResponseType::BOOL},
        {"pumpManual",          SP_GET_PUMP_MANUAL_VALUE_ADDR,   NULL, 0, 1, ResponseType::BOOL},
        {"uvEnable",            SP_GET_UV_ENABLE_VALUE_ADDR,     NULL, 0, 1, ResponseType::BOOL},
        {"flowSensor",          SP_GET_FLOW_SENSOR_VALUE_ADDR,   NULL, 0, 1, ResponseType::BOOL},
        {"manualDisable",       SP_GET_PUMP_MANUAL_DISABLE_ADDR, NULL, 0, 1, ResponseType::BOOL},
        {"scheduleEnable",      SP_GET_SCHEDULE_ENABLE_ADDR,     NULL, 0, 1, ResponseType::BOOL},
        {"nextSchedule",        SP_GET_SCHEDULE_NEXT_ADDR,       NULL, 0, 4, ResponseType::INT },
        {"scheduleDuration",    SP_GET_SCHEDULE_DURATION_ADDR,   NULL, 0, 2, ResponseType::INT },
        {"schedulePeriod",      SP_GET_SCHEDULE_PERIOD_ADDR,     NULL, 0, 1, ResponseType::INT }
    };

    handleJSONResponseRequest(request, 12);
}


void GardenPLCAPIServer::handleIrrigationStateRequest() {
    const Request request[] = {
        {"changeTimestamp", IRR_GET_LAST_CHANGE_ADDR,               NULL, 0, 4, ResponseType::INT },
        {"state",           IRR_GET_CONTROLLER_STATE_ADDR,          NULL, 0, 1, ResponseType::INT },
        {"manualDisable",   IRR_GET_MANUAL_DISABLE_STATE_ADDR,      NULL, 0, 1, ResponseType::BOOL},
        {"pump",            IRR_GET_PUMP_STATE_ADDR,                NULL, 0, 1, ResponseType::BOOL},
        {"mains",           IRR_GET_MAINS_INLET_STATE_ADDR,         NULL, 0, 1, ResponseType::BOOL},
        {"manualSource",    IRR_GET_MANUAL_SOURCE_ADDR,             NULL, 0, 1, ResponseType::INT },
        {"manualZones",     IRR_GET_MANUAL_ZONES_ADDR,              NULL, 0, 2, ResponseType::INT },
        {"activeZones",     IRR_GET_ZONES_STATE_ADDR,               NULL, 0, 2, ResponseType::INT },
        {"scheduleEnable",  IRR_GET_SCHEDULE_ENABLE_ADDR,           NULL, 0, 1, ResponseType::BOOL},
        {"nextIrrigation",  IRR_GET_NEXT_IRRIGATION_TIME_ADDR,      NULL, 0, 4, ResponseType::INT },
        {"resumeTimestamp", IRR_GET_SCHEDULE_RESUME_TIME_ADDR,      NULL, 0, 4, ResponseType::INT }
    };

    handleJSONResponseRequest(request, 11);
}

void GardenPLCAPIServer::handleIrrigationGroupsStateRequest() {    
    String output = "{";
    
    for (uint8_t groupIdx = 0; groupIdx < IRRIGATION_GROUP_COUNT; groupIdx++) {
        output += "\"" + String(groupIdx) + "\":";
        const Request request[] = {
            {"name",    IRR_GET_SCHEDULE_GROUP_NAME_ADDR,   &groupIdx, 1, IRRIGATION_GROUP_NAME_LENGTH, ResponseType::CSTR},
            {"enabled", IRR_GET_SCHEDULE_GROUP_STATE_ADDR,  &groupIdx, 1,                            1, ResponseType::BOOL}
        };

        getJSONString(request, 2, output);
        if (groupIdx != IRRIGATION_GROUP_COUNT-1) output += ",";
    }
    output += "}";
    respondSuccessString(output);
}


void GardenPLCAPIServer::handleIrrigationGroupStateRequest() {
    if (!server.hasArg("groupIdx")) return respondBadRequest();
    uint8_t groupIdx = strtoul(server.arg("groupIdx").c_str(), NULL, 10);

    const Request request[] = {
        {"enabled",         IRR_GET_SCHEDULE_GROUP_STATE_ADDR,      &groupIdx, 1,                            1, ResponseType::BOOL},
        {"name",            IRR_GET_SCHEDULE_GROUP_NAME_ADDR,       &groupIdx, 1, IRRIGATION_GROUP_NAME_LENGTH, ResponseType::CSTR},
        {"zones",           IRR_GET_SCHEDULE_GROUP_ZONES_ADDR,      &groupIdx, 1,                            2, ResponseType::INT },
        {"source",          IRR_GET_SCHEDULE_GROUP_SOURCE_ADDR,     &groupIdx, 1,                            1, ResponseType::INT },
        {"period",          IRR_GET_SCHEDULE_GROUP_PERIOD_ADDR,     &groupIdx, 1,                            1, ResponseType::INT },
        {"duration",        IRR_GET_SCHEDULE_GROUP_DURATION_ADDR,   &groupIdx, 1,                            2, ResponseType::INT },
        {"initTime",        IRR_GET_SCHEDULE_GROUP_INIT_TIME_ADDR,  &groupIdx, 1,                            2, ResponseType::INT },
        {"nextTimestamp",   IRR_GET_SCHEDULE_GROUP_NEXT_TIME_ADDR,  &groupIdx, 1,                            4, ResponseType::INT }
    };

    handleJSONResponseRequest(request, 8);
}


// HTTP response helpers *********************************************************************************************************

void GardenPLCAPIServer::respondSuccessString(const String& successString) {
    respondSuccess(successString, ContentType::TEXT);
}

void GardenPLCAPIServer::respondSuccessJson(const String& jsonString) {
    respondSuccess(jsonString, ContentType::JSON);
}

void GardenPLCAPIServer::respondSuccess(const String& string, const ContentType& responseType) {
    server.sendHeader("Allow", "GET,POST");
    server.sendHeader("Access-Control-Allow-Methods", "GET,POST");
    server.sendHeader("Access-Control-Allow-Headers", "X-Requested-With, X-HTTP-Method-Override, Content-Type, Cache-Control, Accept");
    
    server.send(200, responseType == ContentType::TEXT ? "text/plain" : "application/json", string);
}

void GardenPLCAPIServer::respondNotFound(){
    if (server.method() == HTTP_OPTIONS) {
        // CORS headers (preflight)
        server.sendHeader("Access-Control-Allow-Headers", "*");
        server.sendHeader("Access-Control-Max-Age", "10000");
        server.sendHeader("Access-Control-Allow-Methods", "POST,GET");
        server.send(204);
    }
    else server.send(404, "text/plain", "404: Not found");
}

void GardenPLCAPIServer::respondBadRequest(){
    server.send(400, "text/plain", "400: Bad Request");
}

void GardenPLCAPIServer::respondServerError() {
    server.send(500, "text/plain", "500: Server error.");
}
