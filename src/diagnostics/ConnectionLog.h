#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <cstdarg>
#include <cstring>
#include <memory>
#include <new>

// Low-rate connection events only: no packet capture, flash writes or serial I/O.
// Formatting/allocation stays outside the lock shared by BLE/task/web contexts.
class ConnectionLog {
public:
    void add(const char* format, ...) {
        Entry entry{};
        va_list args;
        va_start(args, format);
        vsnprintf(entry.text, sizeof(entry.text), format, args);
        va_end(args);
        for (char* p = entry.text; *p; ++p) {
            if ((unsigned char)*p < 32 || (unsigned char)*p == 127) *p = ' ';
        }
        portENTER_CRITICAL(&mux);
        entry.ms = millis();
        entry.sequence = ++total;
        entries[next] = entry;
        next = (next + 1) % CAPACITY;
        if (count < CAPACITY) ++count;
        portEXIT_CRITICAL(&mux);
    }

    String snapshot() {
        std::unique_ptr<Entry[]> copy(new (std::nothrow) Entry[CAPACITY]);
        if (!copy) return "Connection log unavailable: insufficient memory.\n";
        size_t savedNext, savedCount;
        uint32_t savedTotal;
        portENTER_CRITICAL(&mux);
        memcpy(copy.get(), entries, sizeof(entries));
        savedNext = next;
        savedCount = count;
        savedTotal = total;
        portEXIT_CRITICAL(&mux);
        String out;
        if (!out.reserve(CAPACITY * 176 + 160)) return "Connection log unavailable: insufficient memory.\n";
        out += "Connection events since boot; latest 64 retained. Reboot clears this log.\n";
        out += "Older events overwritten: ";
        out += String(savedTotal - savedCount);
        out += '\n';
        const size_t start = (savedNext + CAPACITY - savedCount) % CAPACITY;
        for (size_t i = 0; i < savedCount; ++i) {
            const Entry& entry = copy[(start + i) % CAPACITY];
            char prefix[48];
            snprintf(prefix, sizeof(prefix), "#%lu [%lu ms] ", (unsigned long)entry.sequence, (unsigned long)entry.ms);
            out += prefix;
            out += entry.text;
            out += '\n';
        }
        if (!savedCount) out += "No connection events recorded for this backend.\n";
        return out;
    }
private:
    static constexpr size_t CAPACITY = 64;
    struct Entry { uint32_t sequence; uint32_t ms; char text[128]; };
    Entry entries[CAPACITY]{};
    size_t next = 0, count = 0;
    uint32_t total = 0;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};
