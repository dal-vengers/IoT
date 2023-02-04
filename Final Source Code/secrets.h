#include <pgmspace.h>

#define SECRET
#define THINGNAME "MyNewESP32"

const char WIFI_SSID[] = "*****";          // ****
const char WIFI_PASSWORD[] = "*****";        // ****
const char AWS_IOT_ENDPOINT[] = "*****";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = 
"-----BEGIN CERTIFICATE-----\n\
*****
-----END CERTIFICATE-----\n";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = 
"-----BEGIN CERTIFICATE-----\n\
*****
-----END CERTIFICATE-----\n";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = 
"-----BEGIN RSA PRIVATE KEY-----\n\
*****
-----END RSA PRIVATE KEY-----\n";
