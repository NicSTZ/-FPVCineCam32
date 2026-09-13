#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>
#include <functional>

class MspClient {
public:
    using RcCallback = std::function<void(const uint16_t*, size_t)>;

    explicit MspClient(HardwareSerial& serial) : port(serial) {}
    void begin(int rxPin, int txPin, uint32_t baud);
    void loop();
    void requestRc();
    void requestApiVersion();
    bool setCustomText(uint8_t slot, const String& text);
    bool connected() const { return lastValidFrameMs && (millis() - lastValidFrameMs < 2500); }
    uint8_t apiMajor() const { return apiMaj; }
    uint8_t apiMinor() const { return apiMin; }
    void onRc(RcCallback cb) { rcCallback = cb; }

private:
    HardwareSerial& port;
    RcCallback rcCallback;
    uint32_t lastValidFrameMs = 0;
    uint8_t apiMaj = 0, apiMin = 0;

    enum class ParseState { IDLE, V1_DIR, V1_SIZE, V1_CMD, V1_PAYLOAD, V1_CSUM,
                            V2_DIR, V2_FLAGS, V2_CMD1, V2_CMD2, V2_SIZE1, V2_SIZE2, V2_PAYLOAD, V2_CRC };
    ParseState state = ParseState::IDLE;
    uint8_t proto = 0, dir = 0, flags = 0, cmd8 = 0, checksum = 0, crc = 0;
    uint16_t cmd16 = 0, expected = 0, index = 0;
    uint8_t payload[128]{};

    void resetParser();
    void parseByte(uint8_t b);
    void handleFrame(uint16_t cmd, const uint8_t* data, uint16_t len);
    void sendV1(uint8_t cmd, const uint8_t* data = nullptr, uint8_t len = 0);
    void sendV2(uint16_t cmd, const uint8_t* data, uint16_t len);
    static uint8_t crc8DvbS2(uint8_t crc, uint8_t a);
};
