#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/crypto.h>
#include <string.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>
#include "../include/utility.h" 

int generateSasToken(char *token, int tokenLen, char *iotHubName, char *deviceId, char *key, int expiry) 
{
  int uriLen = 28 + strlen(iotHubName) + strlen(deviceId);
  char uri[uriLen];
  snprintf(uri, uriLen, "%s.azure-devices.net/devices/%s", iotHubName, deviceId);

  int encodedUriLen = strlen(uri) * 3 + 1;
  char encodedUri[encodedUriLen];
  urlEncode(encodedUri, uri);

  size_t keyLen;
  unsigned char *decodedKey;
  base64Decode(key, &decodedKey, &keyLen);

  int toSignLen = strlen(encodedUri) + 12;
  unsigned char toSign[toSignLen];
  snprintf(toSign, toSignLen, "%s\n%d", encodedUri, expiry); 

  unsigned char *result; 
  unsigned int resultLen = 32;
  result = (unsigned char*) malloc(sizeof(char) *resultLen);
  result = HMAC(EVP_sha256(), decodedKey, strlen(decodedKey), toSign, strlen(toSign), NULL, NULL);

  char *base64EncodedSignature;
  base64Encode(result, &base64EncodedSignature); 
  
  int uriEncodedSignatureLen = strlen(base64EncodedSignature) * 3 + 1;
  char uriEncodedSignature[uriEncodedSignatureLen];
  urlEncode(uriEncodedSignature, base64EncodedSignature);
  
  snprintf(token, tokenLen, "SharedAccessSignature sr=%s&sig=%s&se=%d", encodedUri, uriEncodedSignature, expiry);

 return 0;
}

