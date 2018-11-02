#ifndef UTILITY 
  #define UTILITY
  void encode(char *s, char *enc, char *tb);
  int urlEncode(char *enc, char *url);
  size_t calcBase64DecodeLength(const char *b64input) ;
  int base64Decode(char *b64message, unsigned char **buffer, size_t *length);
  int base64Encode(const char *message, char **buffer);
#endif

