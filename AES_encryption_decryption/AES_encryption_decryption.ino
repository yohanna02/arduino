#include <Arduino.h>
#include "mbedtls/aes.h"

// 16-byte AES-128 key (no null terminator)
const unsigned char key[16] = { 
  '1','2','3','4','5','6','7','8','9','0','a','b','c','d','e','f' 
};

// 16-byte IV
const unsigned char iv[16]  = { 
  'a','b','c','d','e','f','1','2','3','4','5','6','7','8','9','0' 
};

// Example plaintext (must be multiple of 16 bytes)
unsigned char plaintext[16] = { 
  'E','S','P','3','2',' ','A','E','S',' ','D','E','M','O','!','!' 
};

unsigned char encrypted[16];
unsigned char decrypted[16];

void encryptAES(const unsigned char *input, unsigned char *output, int length) {
  mbedtls_aes_context aes;
  unsigned char iv_copy[16];
  memcpy(iv_copy, iv, 16);

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, key, 128);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, length, iv_copy, input, output);
  mbedtls_aes_free(&aes);
}

void decryptAES(const unsigned char *input, unsigned char *output, int length) {
  mbedtls_aes_context aes;
  unsigned char iv_copy[16];
  memcpy(iv_copy, iv, 16);

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, key, 128);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, length, iv_copy, input, output);
  mbedtls_aes_free(&aes);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== AES Encryption on ESP32 ===");

  // Encrypt
  encryptAES(plaintext, encrypted, sizeof(plaintext));

  Serial.print("Encrypted: ");
  for (int i = 0; i < sizeof(encrypted); i++) {
    Serial.printf("%02X ", encrypted[i]);
  }
  Serial.println();

  // Decrypt
  decryptAES(encrypted, decrypted, sizeof(encrypted));

  Serial.print("Decrypted: ");
  for (int i = 0; i < sizeof(decrypted); i++) {
    Serial.print((char)decrypted[i]);
  }
  Serial.println();
}

void loop() {
}
