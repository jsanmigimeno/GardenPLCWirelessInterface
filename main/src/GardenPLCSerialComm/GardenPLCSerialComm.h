#ifndef GardenPLCSerialComm_h
#define GardenPLCSerialComm_h

#include <MAX485.h>

const uint8_t  transmissionEnablePin  = D0;
const uint32_t transmissionBaudRate   = 19200;
const uint8_t  transmissionBufferSize = 20;
const uint32_t responseTimeout        = 1000;

struct Payload {
    uint8_t* ptr;
    uint8_t size;
};


class GardenPLCSerialComm
{
    public:
        GardenPLCSerialComm();

        void initNewTransmission(uint8_t code);

        void writeRequestPayload(bool request);
        void writeRequestPayload(uint8_t requestInt);
        void writeRequestPayload(uint16_t requestInt);
        void writeRequestPayload(uint32_t requestInt);
        void writeRequestPayload(const uint8_t* startPtr, uint8_t byteCount);

        bool sendRequest();

        bool getResponse();

        uint32_t readResponsePayloadInt(uint8_t readCount);
        void readResponsePayload(uint8_t* bufferPtr, uint8_t bytesCount);

        bool communicate(
            const uint8_t& instructionAddress,
            const Payload& payload,
            const Payload& response
        );

        template <int payloadCount, int responseCount>
        bool communicate(
            const uint8_t& instructionAddress,
            std::array<const Payload*, payloadCount> payload,
            std::array<const Payload*, responseCount> response
        ) {
            initNewTransmission(instructionAddress);

            for(uint8_t i = 0; i < payloadCount; i++) writeRequestPayload(payload[i]->ptr, payload[i]->size);
            bool success = sendRequest();

            if (!success) return false;
            //TODO clear receive buffer?

            for(uint8_t i = 0; i < responseCount; i++) readResponsePayload(response[i]->ptr, response[i]->size);
            return true;
        }

    private:
        MAX485* max485 = new MAX485(Serial, transmissionEnablePin, transmissionBaudRate, SERIAL_8N1, 50, 50);

        uint8_t  requestCode;
        uint32_t responseStartTimestamp;

        uint8_t txPayloadSize;
        uint8_t rxPayloadSize;

        uint8_t txPayloadBuffer[transmissionBufferSize] = {0};
        uint8_t rxPayloadBuffer[transmissionBufferSize] = {0};

        uint8_t* txPayloadBufferReadPtr  = txPayloadBuffer;
        uint8_t* rxPayloadBufferWritePtr = rxPayloadBuffer;

        bool checkParity(uint8_t* bytePtr);
        bool checkParity(uint8_t* startPtr, uint8_t size);

        bool getTxParity();
        bool getRxParity();
};

#endif