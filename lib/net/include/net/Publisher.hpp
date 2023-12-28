#pragma once

#include <mqtt_client.h>

#include <string>
#include <chrono>
#include <atomic>

class Publisher {
public:
    Publisher() = default;

    [[nodiscard]] bool
    connected() const;

    [[nodiscard]] bool
    connect();

    void
    close();

    [[maybe_unused]] int
    publish(const std::string& name, const std::string& value, int qos = 1, bool retain = false);

private:
    void
    onConnect();

    void
    onDisconnect();

    void
    onError(esp_mqtt_error_type_t type, const esp_mqtt_error_codes_t& handle);

    static void
    eventHandler(void* args, esp_event_base_t base, int32_t eventId, void* data);

private:
    std::atomic<bool> _connected{};
    esp_mqtt_client_handle_t _client{};
};