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

//    Source code based on https://github.com/esphome/esphome/tree/dev/esphome/components/esp32_improv

#include "ImprovBLE.h"

namespace improv_ble {

static const char *const TAG = "improv_ble";

class MyServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        ESP_LOGI(TAG, "Client connected");
        ESP_LOGI(TAG, "Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
        ESP_LOGI(TAG, "Client address: %s", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
    };
    void onDisconnect(NimBLEServer* pServer) {
        ESP_LOGI(TAG, "Client disconnected - start advertising");
        global_improv_ble.on_client_disconnect();
        NimBLEDevice::startAdvertising();
    };
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc) {
        ESP_LOGI(TAG, "MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest(){
        ESP_LOGI(TAG, "Server Passkey Request");
        /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key){
        ESP_LOGI(TAG, "The passkey YES/NO number: %d", pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc* desc){
        /** Check that encryption was successful, if not we disconnect the client */
        if(!desc->sec_state.encrypted) {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            ESP_LOGW(TAG, "Encrypt connection failed - disconnecting client");
            return;
        }
        ESP_LOGI(TAG, "Starting BLE work!");
    };
};

class MyCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic){
        ESP_LOGI(TAG, "%s%s%s", pCharacteristic->getUUID().toString().c_str(), ": onRead(), value: ", pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
        ESP_LOGI(TAG, "%s%s%s", pCharacteristic->getUUID().toString().c_str(), ": onWrite(), value: ", pCharacteristic->getValue().c_str());
        std::string data(pCharacteristic->getValue().c_str());
        if (!data.empty()) {
          global_improv_ble.incoming_data_.insert(global_improv_ble.incoming_data_.end(), data.begin(), data.end());
        }
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic) {
        ESP_LOGI(TAG, "Sending notification to clients");
    };


    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        ESP_LOGI(TAG, "%s", str.c_str());
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if(subValue == 0) {
            str += " Unsubscribed to ";
        }else if(subValue == 1) {
            str += " Subscribed to notifications for ";
        } else if(subValue == 2) {
            str += " Subscribed to indications for ";
        } else if(subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();
        ESP_LOGI(TAG, "%s", str.c_str());
    };
};


class MyDescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor* pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        ESP_LOGI(TAG, "%s %s", "Descriptor witten value: ", dscVal.c_str());
    };

    void onRead(NimBLEDescriptor* pDescriptor) {
        ESP_LOGI(TAG, "%s %s", pDescriptor->getUUID().toString().c_str(), " Descriptor read");
    };
};

/** Define callback instances globally to use for multiple Charateristics \ Descriptors */
static MyDescriptorCallbacks      dscCallbacks;
static MyCharacteristicCallbacks  chrCallbacks;

ESP32ImprovComponent::ESP32ImprovComponent() {}

void ESP32ImprovComponent::setup(String &device_name) {
  if (!NimBLEDevice::getInitialized()) NimBLEDevice::init(device_name.c_str());
  //NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Set the transmit power +9db, default is 3db
  server_ = NimBLEDevice::getServer();
  if (server_ == nullptr) server_ = NimBLEDevice::createServer();
  server_->setCallbacks(new MyServerCallbacks());
  server_->advertiseOnDisconnect(true);
  this->service_ = server_->createService(improv::SERVICE_UUID);

  this->status_ = this->service_->createCharacteristic(improv::STATUS_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  NimBLEDescriptor *status_descriptor = this->status_->createDescriptor(improv::STATUS_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  this->status_->setCallbacks(&chrCallbacks);
  status_descriptor->setCallbacks(&dscCallbacks);

  this->error_ = this->service_->createCharacteristic(improv::ERROR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  NimBLEDescriptor *error_descriptor = this->error_->createDescriptor(improv::ERROR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  this->error_->setCallbacks(&chrCallbacks);
  error_descriptor->setCallbacks(&dscCallbacks);

  this->rpc_ = this->service_->createCharacteristic(improv::RPC_COMMAND_UUID, NIMBLE_PROPERTY::WRITE);
  NimBLEDescriptor *rpc_descriptor = this->rpc_->createDescriptor(improv::RPC_COMMAND_UUID, NIMBLE_PROPERTY::WRITE);
  this->rpc_->setCallbacks(&chrCallbacks);
  rpc_descriptor->setCallbacks(&dscCallbacks);

  this->rpc_response_ = this->service_->createCharacteristic(improv::RPC_RESULT_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  NimBLEDescriptor *rpc_response_descriptor = this->rpc_response_->createDescriptor(improv::RPC_RESULT_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  this->rpc_response_->setCallbacks(&chrCallbacks);
  rpc_response_descriptor->setCallbacks(&dscCallbacks);

  this->capabilities_ = this->service_->createCharacteristic(improv::CAPABILITIES_UUID, NIMBLE_PROPERTY::READ);
  NimBLEDescriptor *capabilities_descriptor = this->capabilities_->createDescriptor(improv::CAPABILITIES_UUID, NIMBLE_PROPERTY::READ);
  this->capabilities_->setCallbacks(&chrCallbacks);
  capabilities_descriptor->setCallbacks(&dscCallbacks);

  uint8_t capabilities = 0x00;
  if (this->status_indicator_)
    capabilities |= improv::CAPABILITY_IDENTIFY;
  this->capabilities_->setValue(capabilities);
  this->setup_complete_ = true;
}

void ESP32ImprovComponent::start() {
  if (this->should_start_ || this->state_ != improv::STATE_STOPPED)
    return;

  ESP_LOGI(TAG, "Setting Improv to start");
  this->should_start_ = true;
}

void ESP32ImprovComponent::stop() {
  this->server_->removeService(this->service_);
  this->set_state_(improv::STATE_STOPPED);
}

void ESP32ImprovComponent::release() {
  NimBLEDevice::deinit(true);
}

bool ESP32ImprovComponent::loop(bool timeout) {
  if (!this->incoming_data_.empty())
    this->process_incoming_data_();
  uint32_t now = millis();

  switch (this->state_) {

    case improv::STATE_STOPPED:
      if (this->status_indicator_)
        ;//this->status_indicator_->turn_off();

      if (this->server_->getServiceByUUID(improv::SERVICE_UUID) && this->should_start_ && this->setup_complete_) {
        if (this->service_is_running_) {
          NimBLEAdvertising *_advertising = server_->getAdvertising();
          _advertising->addServiceUUID(this->service_->getUUID());
          _advertising->setScanResponse(true);      // Setting scan response to false it will extend battery life at the expense of less data sent
          _advertising->setAppearance(0x00);
          _advertising->start();

          this->set_state_(improv::STATE_AWAITING_AUTHORIZATION);
          this->set_error_(improv::ERROR_NONE);
          this->should_start_ = false;
          ESP_LOGI(TAG, "Improv WiFi via BLE started");
          ESP_LOGI(TAG, "Service started!");
        } else {
          this->service_->start();
          this->service_is_running_ = true;
        }
      }
      break;

    case improv::STATE_AWAITING_AUTHORIZATION: {
      if (!this->authorizer_) {
        this->set_state_(improv::STATE_AUTHORIZED);
        this->authorized_start_ = now;
      }
      break;
    }

    case improv::STATE_AUTHORIZED: {
      if (!this->authorizer_) {
        if (now - this->authorized_start_ > this->authorized_duration_) {
          ESP_LOGI(TAG, "Authorization timeout");
          this->set_state_(improv::STATE_AWAITING_AUTHORIZATION);
          return false;
        }
      }
      break;
    }

    case improv::STATE_PROVISIONING: {
      if (this->status_indicator_) {
        if ((now % 200) < 100) {
          //this->status_indicator_->turn_on();
        } else {
          //this->status_indicator_->turn_off();
        }
      }
      if (WiFi.getMode() == WIFI_AP || (WiFi.getMode() == WIFI_STA && WiFi.isConnected())) {
        this->set_state_(improv::STATE_PROVISIONED);

        std::vector<String> urls;
#ifndef NOWEBCONFIG
        String webserver_url = String("http://") + String(WiFi.getHostname()) + String(".local");
        urls.push_back(webserver_url);
#endif
        std::vector<uint8_t> data = improv::build_rpc_response(improv::WIFI_SETTINGS, urls);
        this->send_response_(data);
        //this->set_timeout("end-service", 1000, [this] {
          //this->service_->stop();
          //this->set_state_(improv::STATE_STOPPED);
        //});
        return true;
      }
      else if (timeout) this->on_wifi_connect_timeout_();
      break;
    }
    case improv::STATE_PROVISIONED: {
      this->incoming_data_.clear();
      if (this->status_indicator_)
        ;//this->status_indicator_->turn_off();
      break;
    }
  }
  return false;
}

improv::State ESP32ImprovComponent::get_state() {
    return this->state_;
  }

  String ESP32ImprovComponent::get_ssid() {
    return String(this->command_.ssid.c_str());
  }

  String ESP32ImprovComponent::get_password() {
    return String(this->command_.password.c_str());
  }


void ESP32ImprovComponent::set_state_(improv::State state) {
  ESP_LOGV(TAG, "Setting state: %d", state);
  this->state_ = state;
  if (this->status_->getValue().length() == 0 || this->status_->getValue()[0] != state) {
    uint8_t data[1]{state};
    this->status_->setValue(data, 1);
    if (state != improv::STATE_STOPPED)
      this->status_->notify();
  }
}

void ESP32ImprovComponent::set_error_(improv::Error error) {
  if (error != improv::ERROR_NONE) {
    ESP_LOGE(TAG, "Error: %d", error);
  }
  if (this->error_->getValue().length() == 0 || this->error_->getValue()[0] != error) {
    uint8_t data[1]{error};
    this->error_->setValue(data, 1);
    if (this->state_ != improv::STATE_STOPPED)
      this->error_->notify();
  }
}

void ESP32ImprovComponent::send_response_(std::vector<uint8_t> &response) {
  this->rpc_response_->setValue(response);
  if (this->state_ != improv::STATE_STOPPED)
    this->rpc_response_->notify();
}

void ESP32ImprovComponent::process_incoming_data_() {
  uint8_t length = this->incoming_data_[1];

  ESP_LOGI(TAG, "Processing %d bytes", length);
  if (this->incoming_data_.size() - 3 == length) {
    this->set_error_(improv::ERROR_NONE);
    command_ = improv::parse_improv_data(this->incoming_data_);
    switch (command_.command) {
      case improv::BAD_CHECKSUM:
        ESP_LOGW(TAG, "Error decoding Improv payload");
        this->set_error_(improv::ERROR_INVALID_RPC);
        this->incoming_data_.clear();
        break;
      case improv::WIFI_SETTINGS: {
        if (this->state_ != improv::STATE_AUTHORIZED) {
          ESP_LOGW(TAG, "Settings received, but not authorized");
          this->set_error_(improv::ERROR_NOT_AUTHORIZED);
          this->incoming_data_.clear();
          return;
        }
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        WiFi.begin(command_.ssid.c_str(), command_.password.c_str());
        this->set_state_(improv::STATE_PROVISIONING);
        ESP_LOGI(TAG, "Received Improv wifi settings ssid=%s, password=%s", command_.ssid.c_str(), command_.password.c_str());
        this->incoming_data_.clear();
        break;
      }
      case improv::IDENTIFY:
        this->incoming_data_.clear();
        this->identify_start_ = millis();
        break;
      default:
        ESP_LOGW(TAG, "Unknown Improv payload");
        this->set_error_(improv::ERROR_UNKNOWN_RPC);
        this->incoming_data_.clear();
    }
  } else if (this->incoming_data_.size() - 2 > length) {
    ESP_LOGV(TAG, "Too much data came in, or malformed resetting buffer...");
    this->incoming_data_.clear();
  } else {
    ESP_LOGV(TAG, "Waiting for split data packets...");
  }
}

void ESP32ImprovComponent::on_wifi_connect_timeout_() {
  this->set_error_(improv::ERROR_UNABLE_TO_CONNECT);
  this->set_state_(improv::STATE_AUTHORIZED);
  if (!this->authorizer_)
    this->authorized_start_ = millis();
  ESP_LOGW(TAG, "Timed out trying to connect to given WiFi network");
  WiFi.disconnect();
}

void ESP32ImprovComponent::on_client_disconnect() { this->set_error_(improv::ERROR_NONE); };

ESP32ImprovComponent global_improv_ble;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}