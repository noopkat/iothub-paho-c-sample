#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/crypto.h>
#include <string.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>

char rfc3986[256] = {0};
char html5[256] = {0};
 
void encode(char *s, char *enc, char *tb)
{
	for (; *s; s++) {
		if (tb[*s]) sprintf(enc, "%c", tb[*s]);
		else sprintf(enc, "%%%02X", *s);
		while (*++enc);
	}
}
 
int urlEncode(char *enc, char *url)
{
	for (int i = 0; i < 256; i++) {
		rfc3986[i] = isalnum(i) || i == '~' || i == '-' || i == '.'|| i == '_' ? i : 0;
		html5[i] = isalnum(i) || i == '*' || i == '-' || i == '.' || i == '_' ? i : (i == ' ') ? '+' : 0;
	}
 
	encode(url, enc, rfc3986);
 
	return 0;
}

size_t calcBase64DecodeLength(const char *b64input) 
{
  size_t len = strlen(b64input),
  padding = 0;
  
  // check if string ends with '=' or '=='
  if (b64input[len-1] == '=' && b64input[len-2] == '=')
    padding = 2;
  else if (b64input[len-1] == '=')
    padding = 1;

  return (len * 3) / 4 - padding;
}

int base64Decode(char *b64message, unsigned char **buffer, size_t *length) 
{ 
  BIO *bio, *b64;

  int decodeLen = calcBase64DecodeLength(b64message);
  *buffer = (unsigned char*)malloc(decodeLen + 1);
  (*buffer)[decodeLen] = '\0';

  bio = BIO_new_mem_buf(b64message, -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  *length = BIO_read(bio, *buffer, strlen(b64message));
  BIO_free_all(bio);

  return 0;
}

int base64Encode(const char *message, char **buffer) 
{
  BIO *bio, *b64;
  FILE *stream;
  int encodedSize = 4 * ceil((double)strlen(message) / 3);
  *buffer = (char *)malloc(encodedSize + 1);

  stream = fmemopen(*buffer, encodedSize + 1, "w");
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, message, strlen(message));
  BIO_flush(bio);
  BIO_free_all(bio);
  fclose(stream);

  return 0;
}

