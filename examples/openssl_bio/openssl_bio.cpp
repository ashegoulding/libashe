#include <iostream>

#include <string>
#include <vector>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cstring>


const char BUF[] = "Messsssssssssssssssssssssssssssssssaggggggggggggggggggggggeeeeeeeeee";
const size_t BUF_SIZE = sizeof(BUF)/sizeof(char) - 1;
std::string encoded;
std::vector<char> decoded;

void encode()
{
	static const size_t PUSH_PAGE = 10; // Make it bigger in real impl.

	BIO *b64, *bio;
	size_t pushed, rem, sizeToPush;
	long readSize;
	char *p;
	bool flag;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // param it.
	BIO_push(b64, bio);

	pushed = 0;
	flag = true;
	do
	{
		rem = ::BUF_SIZE - pushed;
		if(rem > PUSH_PAGE)
			sizeToPush = PUSH_PAGE;
		else if(0 == rem)
			break;
		else
			sizeToPush = rem;
		BIO_reset(bio);
		BIO_write(b64, ::BUF + pushed, sizeToPush);
		pushed += sizeToPush;
		if(pushed == ::BUF_SIZE)
		{
			BIO_flush(b64);
			flag = false;
		}

		readSize = BIO_get_mem_data(bio, &p);
		if(readSize)
			::encoded.append(p, readSize);
	}
	while(flag);

	BIO_free_all(b64);
	b64 = bio = nullptr;
}

void decode()
{
	const size_t PUSH_PAGE = 10; // Make it bigger in real impl.

	BIO *b64, *bio;
	size_t pushed, rem, sizeToPush;
	long readSize;
	std::vector<char> buf(PUSH_PAGE);

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	BIO_push(b64, bio);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Required.

	/* TODO: Validate the encoded string format
	 * RFC 4648
	 *	ex) "^[A-Za-z0-9\\+\\/\\s]+(={0,2})?(\\s+)?$"
	 *	ex) "^[A-Za-z0-9\\-_\\s]+(={0,2})?(\\s+)?$"
	 *		(URL and filename. Replace '-' and '_' to '+' and '/' before decoding.)
	 */
	// TODO: Strip non-base64 characters.

	pushed = 0;
	while(true)
	{
		rem = ::encoded.size() - pushed;
		if(rem > PUSH_PAGE)
			sizeToPush = PUSH_PAGE;
		else if(rem == 0)
			break;
		else
			sizeToPush = rem;
		BIO_reset(bio);
		BIO_write(bio, ::encoded.data() + pushed, sizeToPush);
		pushed += sizeToPush;

		readSize = BIO_read(b64, buf.data(), buf.size());
		if(readSize > 0)
			::decoded.insert(::decoded.end(), buf.begin(), buf.begin() + readSize);
	}

	BIO_free_all(b64);
	b64 = bio = nullptr;
}

int main()
{
	int ret;

	::encode();
	std::cerr << ::encoded << std::endl;

	std::cerr << ::BUF << std::endl;

	::decode();
	::decoded.push_back(0);
	std::cerr << ::decoded.data() << std::endl;

	if(0 == ::strcmp(BUF, ::decoded.data()))
	{
		std::cout << "A - ok" << std::endl;
		ret = 0;
	}
	else
	{
		std::cout << "** Does not match!" << std::endl;
		ret = 2;
	}

	return ret;
}

