/****************************************************************************
* Bluetooth config wifi connection to ap. You can config ESP32 to connect a softap
* or config ESP32 as a softap to be connected by other device. APP can be downloaded from github 
* android source code: https://github.com/EspressifApp/EspBlufi
* iOS source code: https://github.com/EspressifApp/EspBlufiForiOS
****************************************************************************/

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_bt.h>

#include <esp_blufi_api.h>
#include <esp_bt_defs.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>

#define BLUFI_TAG "BLUFI"
#define BLUFI_INFO(fmt, ...)   ESP_LOGI(BLUFI_TAG, fmt, ##__VA_ARGS__) 
#define BLUFI_ERROR(fmt, ...)  ESP_LOGE(BLUFI_TAG, fmt, ##__VA_ARGS__) 

#include "BluFiSecurity.h"

#define BLUFI_DEVICE_NAME            "BLUFI_PEDALINO"

static uint8_t blufi_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
};

//static uint8_t test_manufacturer[TEST_MANUFACTURER_DATA_LEN] =  {0x12, 0x23, 0x45, 0x56};
static esp_ble_adv_data_t   blufi_adv_data;
static esp_ble_adv_params_t blufi_adv_params;

static wifi_config_t sta_config;
static wifi_config_t ap_config;

static uint8_t  server_if;
static uint16_t conn_id;


static void blufi_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
  switch (event) {
    case ESP_BLUFI_EVENT_INIT_FINISH:
        BLUFI_INFO("ESP_BLUFI_EVENT_INIT_FINISH");

        esp_ble_gap_set_device_name(BLUFI_DEVICE_NAME);
        blufi_adv_data.set_scan_rsp        = false;
        blufi_adv_data.include_name        = true;
        blufi_adv_data.include_txpower     = true;
        blufi_adv_data.min_interval        = 0x0006; //slave connection min interval, Time = min_interval * 1.25 msec
        blufi_adv_data.max_interval        = 0x0010; //slave connection max interval, Time = max_interval * 1.25 msec
        blufi_adv_data.appearance          = 0x00;
        blufi_adv_data.manufacturer_len    = 0;
        blufi_adv_data.p_manufacturer_data =  NULL;
        blufi_adv_data.service_data_len    = 0;
        blufi_adv_data.p_service_data      = NULL;
        blufi_adv_data.service_uuid_len    = 16;
        blufi_adv_data.p_service_uuid      = blufi_service_uuid128;
        blufi_adv_data.flag                = 0x6;
        esp_ble_gap_config_adv_data(&blufi_adv_data);
        break;
    case ESP_BLUFI_EVENT_DEINIT_FINISH:
        BLUFI_INFO("ESP_BLUFI_EVENT_DEINIT_FINISH");
        break;
    case ESP_BLUFI_EVENT_BLE_CONNECT:
        BLUFI_INFO("ESP_BLUFI_EVENT_BLE_CONNECT");
        server_if = param->connect.server_if;
        conn_id = param->connect.conn_id;
        esp_ble_gap_stop_advertising();
        blufi_security_init();
        break;
    case ESP_BLUFI_EVENT_BLE_DISCONNECT:
        BLUFI_INFO("ESP_BLUFI_EVENT_BLE_DISCONNECT");
        blufi_security_deinit();
        esp_ble_gap_start_advertising(&blufi_adv_params);
        break;
    case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
        BLUFI_INFO("ESP_BLUFI_EVENT_SET_WIFI_OPMODE %d", param->wifi_mode.op_mode);
        ESP_ERROR_CHECK(esp_wifi_set_mode(param->wifi_mode.op_mode) );
        break;
    case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
        BLUFI_INFO("ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP");
        /* there is no wifi callback when the device has already connected to this wifi
        so disconnect wifi before connection.
        */
        esp_wifi_disconnect();
        esp_wifi_connect();
        break;
    case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
        BLUFI_INFO("ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP");
        esp_wifi_disconnect();
        break;
    case ESP_BLUFI_EVENT_REPORT_ERROR:
        BLUFI_ERROR("ESP_BLUFI_EVENT_REPORT_ERROR, error code %d", param->report_error.state);
        esp_blufi_send_error_info(param->report_error.state);
        break;
    case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
        BLUFI_INFO("ESP_BLUFI_EVENT_GET_WIFI_STATUS");
        wifi_mode_t mode;
        esp_blufi_extra_info_t info;

        esp_wifi_get_mode(&mode);

        if (WiFi.isConnected()) {  
            memset(&info, 0, sizeof(esp_blufi_extra_info_t));
            memcpy(info.sta_bssid, WiFi.BSSID(), 6);
            info.sta_bssid_set = true;
            info.sta_ssid = (uint8_t*)WiFi.SSID().c_str();
            info.sta_ssid_len = WiFi.SSID().length();
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
        } else {
            esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
        }
        break;
    }
    case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE");
        esp_blufi_close(server_if, conn_id);
        break;
    case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
        BLUFI_INFO("ESP_BLUFI_EVENT_DEAUTHENTICATE_STA");
        /* TODO */
        break;
	case ESP_BLUFI_EVENT_RECV_STA_BSSID:
        memcpy(sta_config.sta.bssid, param->sta_bssid.bssid, 6);
        sta_config.sta.bssid_set = 1;
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_STA_BSSID %s", sta_config.sta.ssid);
        break;
	case ESP_BLUFI_EVENT_RECV_STA_SSID:
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_STA_SSID");
        strncpy((char *)sta_config.sta.ssid, (char *)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
        sta_config.sta.ssid[param->sta_ssid.ssid_len] = '\0';
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        wifiSSID = String((char *)sta_config.sta.ssid);
        DPRINT("SSID        : %s\n", wifiSSID.c_str());
        break;
	case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_STA_PASSWD");
        strncpy((char *)sta_config.sta.password, (char *)param->sta_passwd.passwd, param->sta_passwd.passwd_len);
        sta_config.sta.password[param->sta_passwd.passwd_len] = '\0';
        esp_wifi_set_config(WIFI_IF_STA, &sta_config);
        wifiPassword = String((char *)sta_config.sta.password);
        DPRINT("Password    : %s\n", wifiPassword.c_str());
        eeprom_update_sta_wifi_credentials(wifiSSID, wifiPassword);
        break;
	case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        strncpy((char *)ap_config.ap.ssid, (char *)param->softap_ssid.ssid, param->softap_ssid.ssid_len);
        ap_config.ap.ssid[param->softap_ssid.ssid_len] = '\0';
        ap_config.ap.ssid_len = param->softap_ssid.ssid_len;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SOFTAP_SSID %s", ap_config.ap.ssid);
        ssidSoftAP = String((char *)ap_config.ap.ssid);
        DPRINT("AP SSID     : %s\n", ssidSoftAP.c_str());
        break;
	case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        strncpy((char *)ap_config.ap.password, (char *)param->softap_passwd.passwd, param->softap_passwd.passwd_len);
        ap_config.ap.password[param->softap_passwd.passwd_len] = '\0';
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD %s", ap_config.ap.password);
        break;
	case ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM:
        if (param->softap_max_conn_num.max_conn_num > 4) return;
        ap_config.ap.max_connection = param->softap_max_conn_num.max_conn_num;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM %d", ap_config.ap.max_connection);
        break;
	case ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE:
        if (param->softap_auth_mode.auth_mode >= WIFI_AUTH_MAX) return;
        ap_config.ap.authmode = param->softap_auth_mode.auth_mode;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE %d", ap_config.ap.authmode);
        break;
	case ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL:
        if (param->softap_channel.channel > 13) return;
        ap_config.ap.channel = param->softap_channel.channel;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL %d", ap_config.ap.channel);
        break;
    case ESP_BLUFI_EVENT_GET_WIFI_LIST: {
        BLUFI_INFO("ESP_BLUFI_EVENT_GET_WIFI_LIST");
        wifi_scan_config_t scanConf = {
          .ssid        = NULL,
          .bssid       = NULL,
          .channel     = 0,
          .show_hidden = false
        };
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, true));
        break;
    }
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        BLUFI_INFO("ESP_BLUFI_EVENT_RECV_CUSTOM_DATA %d", param->custom_data.data_len);
        esp_log_buffer_hex("Custom Data", param->custom_data.data, param->custom_data.data_len);
        break;
	case ESP_BLUFI_EVENT_RECV_USERNAME:
        /* Not handle currently */
        break;
	case ESP_BLUFI_EVENT_RECV_CA_CERT:
        /* Not handle currently */
        break;
	case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        /* Not handle currently */
        break;
	case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        /* Not handle currently */
        break;
	case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        /* Not handle currently */
        break;;
	case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        /* Not handle currently */
        break;
    default:
        break;
    }
}

static esp_blufi_callbacks_t blufi_callbacks = {
    .event_cb = blufi_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

static void blufi_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
      blufi_adv_params.adv_int_min        = 0x100,
      blufi_adv_params.adv_int_max        = 0x100,
      blufi_adv_params.adv_type           = ADV_TYPE_IND,
      blufi_adv_params.own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
      //blufi_adv_params.peer_addr            =
      //blufi_adv_params.peer_addr_type       =
      blufi_adv_params.channel_map        = ADV_CHNL_ALL,
      blufi_adv_params.adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
      esp_ble_gap_start_advertising(&blufi_adv_params);
      BLUFI_INFO("BluFi GAP advertising started");
      break;
    default:
      break;
  }
}

void blufi_config(void)
{
    esp_err_t ret;

/*
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bt_cfg.mode = ESP_BT_MODE_BLE;
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        BLUFI_ERROR("%s initialize bt controller failed: %s", __func__, esp_err_to_name(ret));
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        BLUFI_ERROR("%s enable bt controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        BLUFI_ERROR("%s init bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        BLUFI_ERROR("%s init bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
*/
    BLUFI_INFO("BLUEDROID MAC ADDRESS: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(esp_bt_dev_get_address()));
    BLUFI_INFO("BLUFI VERSION %04x", esp_blufi_get_version());
    BLUFI_INFO("BLUFI DEVICE NAME %s", BLUFI_DEVICE_NAME);

    ret = esp_ble_gap_register_callback(blufi_gap_event_handler);
    if (ret) {
        BLUFI_ERROR("%s gap register failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_blufi_register_callbacks(&blufi_callbacks);
    if (ret) {
        BLUFI_ERROR("%s blufi register failed, error code = %x", __func__, ret);
        return;
    }

    esp_blufi_profile_init();
}