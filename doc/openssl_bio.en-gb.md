# Buffered in-memory Base64 Coding with OpenSSL.
This example, **openssl_bio.cpp**, shows how to encode/decode base64 with openssl base64 filter.
Many examples on the internet only demonstrates how to encode/decode full memory of data,
not the stream of base64 encoded string or data to encode. By "buffered" I mean coding in
"stream mode", like the one in libxml. The example decodes/encodes a base64 string/binary
data 10 characters/bytes at a time without having the complete data prepared before hand.
This way of conversion will save a huge amount of memory and response time, or can benefit
platform portability since it is entirely in-memory. eg) Encode the data from the user input
or other network enpoint as the program receives it.

## Details
In [OpenSSL example](https://www.openssl.org/docs/manmaster/crypto/BIO_f_base64.html), stdout
is opened as file BIO to print the encoded string. In this example, memory BIO is allocated
instead to receive the result string. `BIO_reset()` is called to clear the previous result
otherwise the result will be appended every `BIO_write()` call.

### Other Considerations
When encoding, if `BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL)` is not called, the outcome
will be delayed since the filter only writes the data when a full line of encoded string is
ready.

The Base64 filter will get confused if it is given any new-line character whilst
`BIO_FLAGS_BASE64_NO_NL` is set. In my opinion, it is best to use the filter by always passing
it one line of base64 encoded string with `BIO_FLAGS_BASE64_NO_NL` set. Especially in this
case. 

## Note
This is(was) a draft for base64 coding classes of LibAshe.

## Author
* ashe.goulding@gmail.com

## Legal Note
NO RIGHTS claimed on this example.

## Links
* <http://doctrina.org/Base64-With-OpenSSL-C-API.html>
* OpenSSL example: <https://www.openssl.org/docs/manmaster/crypto/BIO_f_base64.html>
