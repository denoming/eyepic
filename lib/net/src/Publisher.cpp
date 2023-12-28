#include "../include/net/Publisher.hpp"

#include "net/Config.hpp"

#include <esp_log.h>
#include <esp_assert.h>

static const char* TAG = "EP<Publisher>";

bool
Publisher::connected() const
{
    return _connected.load();
}

bool
Publisher::connect()
{
    if (_client != nullptr) {
        ESP_LOGW(TAG, "Attempt to setup publisher more than once");
        return true;
    }

    esp_mqtt_client_config_t cfg = {};
    cfg.broker.address.uri = EP_NET_MQTT_URL;
#ifdef EP_NET_MQTT_AUTH
    cfg.credentials.username = EP_NET_MQTT_USER;
    cfg.credentials.authentication.password = EP_NET_MQTT_PASS;
#endif

    if (_client = esp_mqtt_client_init(&cfg); _client == nullptr) {
        ESP_LOGE(TAG, "Unable to init MQTT client");
        return false;
    }
    if (esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, eventHandler, this) != ESP_OK) {
        close();
        ESP_LOGE(TAG, "Unable to register MQTT client");
        return false;
    }
    if (esp_mqtt_client_start(_client) != ESP_OK) {
        close();
        ESP_LOGE(TAG, "Unable to start MQTT client");
        return false;
    }

    return true;
}

void
Publisher::close()
{
    if (_client != nullptr) {
        esp_mqtt_client_stop(_client);
        esp_mqtt_client_disconnect(_client);
        esp_mqtt_client_destroy(_client);
        _client = {};
    }
}

int
Publisher::publish(const std::string& name, const std::string& value, int qos, bool retain)
{
    if (_client == nullptr) {
        return -1;
    }
    if (name.empty() or value.empty()) {
        return -1;
    }
    if (not _connected) {
        return -1;
    }

    const std::string topic = std::string{EP_NET_MQTT_TOPIC} + "/" + name;
    return esp_mqtt_client_publish(
        _client, topic.data(), value.data(), int(value.size()), qos, retain);
}

void
Publisher::onConnect()
{
    _connected.store(true);
}

void
Publisher::onDisconnect()
{
    _connected.store(false);
}

void
Publisher::onError(esp_mqtt_error_type_t type, const esp_mqtt_error_codes_t& handle)
{
    if (handle.error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        const int error = handle.esp_transport_sock_errno;
        ESP_LOGE(TAG, "Transport error: %d, %s", error, strerror(error));
        return;
    }
    if (handle.error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
        ESP_LOGE(TAG, "Connection error: %d", handle.connect_return_code);
        return;
    }

    ESP_LOGE(TAG, "Unknown error: type<%d>", int(handle.error_type));
}

void
Publisher::eventHandler(void* args, esp_event_base_t /*base*/, int32_t eventId, void* data)
{
    const auto event = static_cast<esp_mqtt_event_handle_t>(data);
    if (event == nullptr) {
        ESP_LOGE(TAG, "Invalid event");
        return;
    }

    auto* self = static_cast<Publisher*>(args);
    if (self == nullptr) {
        ESP_LOGE(TAG, "Invalid pointer");
        assert(event != nullptr);
        esp_mqtt_client_unregister_event(event->client, MQTT_EVENT_ANY, &eventHandler);
        return;
    }

    switch (eventId) {
    case MQTT_EVENT_CONNECTED: {
        self->onConnect();
        break;
    }
    case MQTT_EVENT_DISCONNECTED: {
        self->onDisconnect();
        break;
    }
    case MQTT_EVENT_ERROR: {
        assert(event != nullptr);
        self->onError(event->error_handle->error_type, *event->error_handle);
        break;
    }
    default:
        break;
    }
}