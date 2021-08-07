/* ----------------- General config -------------------------------- */
/* WiFi */
const char* ssid                  = "Boganville";     // Your WiFi SSID
const char* password              = "potsticker";     // Your WiFi password

/* MQTT */
const char* mqtt_broker           = "192.168.86.87"; // IP address of your MQTT broker
const char* mqtt_username         = "mosquitto";              // Your MQTT username
const char* mqtt_password         = "%0mosquitto";              // Your MQTT password
#define     REPORT_MQTT_SEPARATE  true               // Report each value to its own topic
#define     REPORT_MQTT_JSON      true               // Report all values in a JSON message
const char* status_topic          = "events";        // MQTT topic to report startup

/* Serial */
#define     SERIAL_BAUD_RATE    115200               // Speed for USB serial console

/* ----------------- Hardware-specific config ---------------------- */
/* Multimeter interface */
#define     UT61E_RX_PIN              D5             // Rx from UT61e (== UT61e Tx)
#define     UT61E_BAUD_RATE        19200             // PMS5003 uses 9600bps

/* Status LED */
#define     STATUS_LED_PIN            D4
