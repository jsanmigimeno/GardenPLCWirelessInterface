#include "GardenPLCSerialComm.h"


GardenPLCSerialComm::GardenPLCSerialComm() {
    max485->begin();
}

void GardenPLCSerialComm::initNewTransmission(uint8_t code) {
    requestCode   = code;
    txPayloadSize = 0;
    rxPayloadSize = 0;
    txPayloadBufferReadPtr  = txPayloadBuffer;
    rxPayloadBufferWritePtr = rxPayloadBuffer;
    memset(rxPayloadBuffer, 0, transmissionBufferSize);
}

void GardenPLCSerialComm::writeRequestPayload(bool request) {
    writeRequestPayload((uint8_t) (request ? 1 : 0));
}

void GardenPLCSerialComm::writeRequestPayload(uint8_t requestInt) {
    writeRequestPayload((uint8_t*) &requestInt, 1);
}

void GardenPLCSerialComm::writeRequestPayload(uint16_t requestInt) {
    writeRequestPayload((uint8_t*) &requestInt, 2);
}

void GardenPLCSerialComm::writeRequestPayload(uint32_t requestInt) {
    writeRequestPayload((uint8_t*) &requestInt, 4);
}

void GardenPLCSerialComm::writeRequestPayload(const uint8_t* startPtr, uint8_t byteCount) {
    for (int8_t i = 0; i < byteCount; i++) {
        txPayloadSize++;
        *txPayloadBufferReadPtr++ = startPtr[i];
    }
}

bool GardenPLCSerialComm::sendRequest() {
    txPayloadBufferReadPtr  = txPayloadBuffer;

    // Clear received data
    while (max485->available() != 0) max485->read();

    max485->beginTransmission();

    max485->write(requestCode);

    uint8_t parityAndPayloadSize = txPayloadSize | ((getTxParity() ? 0 : 1) << 7); // Make number of 1s in the request even 
    max485->write(parityAndPayloadSize);

    for (int8_t i = 0; i < txPayloadSize; i++) {
        max485->write(*(txPayloadBufferReadPtr++));
    }

    // Serial.available() treats 0xFF as EOL and will skip it if it's the last byte on the buffer.
    // Always transmit 0x0 at the end of the response as a workaround
    max485->write(0x0);

    max485->flush();
    max485->endTransmission();
    
    return getResponse();
}

bool GardenPLCSerialComm::getResponse() {
    responseStartTimestamp = millis();

    // Wait for response
    while(max485->available() < 2) {    // Expect 2 packets - request code + payload size/parity bit
        if (millis() >= responseStartTimestamp + responseTimeout) {
            // TODO NOTE FAIL?
            return false;
        }
    }

    // Check request code matches
    uint8_t receivedRequestCode = max485->read();
    if (receivedRequestCode != requestCode) return false;

    uint8_t parityAndPayloadSize = max485->read();
    bool parityBit = (0x80 & parityAndPayloadSize) != 0;
    rxPayloadSize  = 0x7F & parityAndPayloadSize;

    // Wait for payload
    while(max485->available() < rxPayloadSize + 1) { // Expect an extra null packet at the end of the response
        if (millis() >= responseStartTimestamp + responseTimeout) {
            // TODO NOTE FAIL?
            return false;
        }
    }

    // Read response
    for (uint i = 0; i < rxPayloadSize; i++) {
        *(rxPayloadBufferWritePtr++) = max485->read();
    }

    // Expect an extra null byte
    if (max485->read() != 0) return false; //TODO NOTE ERROR?

    // Check parity
    if (getRxParity() == parityBit) return false; // getRxParity is true if the number of 1s in the response is even, hence the parity bit should be 0
    
    rxPayloadBufferWritePtr = rxPayloadBuffer;

    return true;
}

uint32_t GardenPLCSerialComm::readResponsePayloadInt(uint8_t readCount)  {
    uint32_t response = 0;

    uint8_t* responsePtr = (uint8_t*) &response;

    for (int8_t i = 0; i < readCount; i++) {
        responsePtr[i] = *(rxPayloadBufferWritePtr++);
        //TODO CHECK BUFFER OVERFLOW
    }

    return response;

}

void GardenPLCSerialComm::readResponsePayload(uint8_t* bufferPtr, uint8_t bytesCount)  {
    for (int8_t i = 0; i < bytesCount; i++) {
        bufferPtr[i] = *(rxPayloadBufferWritePtr++);
        //TODO CHECK BUFFER OVERFLOW
    }
}

bool GardenPLCSerialComm::communicate(
    const uint8_t& instructionAddress,
    const Payload& payload,
    const Payload& response
) {
    return communicate<1, 1>(instructionAddress, {&payload}, {&response});
}


bool GardenPLCSerialComm::checkParity(uint8_t* bytePtr) {
    // Returns true if the number of 1s in the byte is even
    uint8_t t = *bytePtr ^ (*bytePtr >> 1);
    t ^= t >> 2;
    t ^= t >> 4;
    return (~t) & 1;
}

bool GardenPLCSerialComm::checkParity(uint8_t* startPtr, uint8_t size) {
    // Returns true if the number of 1s in the buffer is even
    bool parity = true;
    for (uint8_t i = 0; i < size; i++) {
        parity = parity == checkParity(startPtr + i);
    }
    return parity;
}

bool GardenPLCSerialComm::getTxParity() {
    bool parity = checkParity(&requestCode);
    parity      = parity == checkParity(&txPayloadSize);
    parity      = parity == checkParity(txPayloadBuffer, txPayloadSize);

    return parity;
}
bool GardenPLCSerialComm::getRxParity() {
    bool parity = checkParity(&requestCode);
    parity      = parity == checkParity(&rxPayloadSize);
    parity      = parity == checkParity(rxPayloadBuffer, rxPayloadSize);

    return parity;
}
