/*
__________           .___      .__  .__                 _____  .__       .__     ___ ________________    ___
\______   \ ____   __| _/____  |  | |__| ____   ____   /     \ |__| ____ |__|   /  / \__    ___/     \   \  \
 |     ___// __ \ / __ |\__  \ |  | |  |/    \ /  _ \ /  \ /  \|  |/    \|  |  /  /    |    | /  \ /  \   \  \
 |    |   \  ___// /_/ | / __ \|  |_|  |   |  (  <_> )    Y    \  |   |  \  | (  (     |    |/    Y    \   )  )
 |____|    \___  >____ |(____  /____/__|___|  /\____/\____|__  /__|___|  /__|  \  \    |____|\____|__  /  /  /
               \/     \/     \/             \/               \/        \/       \__\                 \/  /__/
                                                                                   (c) 2018-2023 alf45star
                                                                       https://github.com/alf45tar/PedalinoMini
*/

//    Source code based on https://github.com/esphome/esphome/tree/dev/esphome/components/improv_serial

#pragma once

#include <string>
#include <vector>
#include <improv.h>
#include <NimBLEDevice.h>
#include <WiFi.h>

namespace improv_ble {

class ESP32ImprovComponent {
  public:
    ESP32ImprovComponent();

    void setup(String &device_name);
    void start();
    bool loop(bool timeout = false);
    void stop();
    void release();

    bool is_active() const { return this->state_ != improv::STATE_STOPPED; }
    void on_client_disconnect();
  
    void set_authorizer(bool authorizer) { this->authorizer_ = authorizer; }
    void set_status_indicator(bool status_indicator) { this->status_indicator_ = status_indicator; }
    void set_authorized_duration(uint32_t authorized_duration) { this->authorized_duration_ = authorized_duration; }

    improv::State get_state();

    String get_ssid();
    String get_password();

  protected:
    bool setup_complete_{false};
    bool should_start_{false};
    bool service_is_running_{false};

    uint32_t identify_start_{0};
    uint32_t identify_duration_;
    uint32_t authorized_start_{0};
    uint32_t authorized_duration_;

    NimBLEServer         *server_;
    NimBLEService        *service_;
    NimBLECharacteristic *status_;
    NimBLECharacteristic *error_;
    NimBLECharacteristic *rpc_;
    NimBLECharacteristic *rpc_response_;
    NimBLECharacteristic *capabilities_;

    bool authorizer_{false};
    bool status_indicator_{false};

    improv::ImprovCommand command_{improv::Command::UNKNOWN, "", ""};
    improv::State state_{improv::STATE_STOPPED};
    improv::Error error_state_{improv::ERROR_NONE};
  
    std::vector<uint8_t> incoming_data_;

    void set_state_(improv::State state);
    void set_error_(improv::Error error);
    void send_response_(std::vector<uint8_t> &response);
    void process_incoming_data_();
    void on_wifi_connect_timeout_();
    bool check_identify_();

  private:
    friend class MyServerCallbacks;
    friend class MyCharacteristicCallbacks;
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern ESP32ImprovComponent global_improv_ble;

}