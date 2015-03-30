#ifndef TRANSITIVEINTERFACE_H_
#define TRANSITIVEINTERFACE_H_

#include <cstdlib>
#include <vector>
#include <set>

namespace ashe
{

class TransitiveInterface
{
public:
	typedef TransitiveInterface thisClass;

protected:
	const static unsigned int SB_GOOD/* =		 	0*/;
	const static unsigned int SB_FAILED/* = 	0b00000000000000000000000000000001*/;
	const static unsigned int SB_DELAYED/* = 	0b00000000000000000000000000000010*/;
	const static unsigned int SB_ENDED/* = 		0b00000000000000000000000000000100*/;
	const static unsigned int SB_CLOSED/* = 	0b00000000000000000000000000001000*/;

	size_t lastRetrievedSize = 0, lastSentSize = 0, retrievedSize = 0, sentSize = 0;
	unsigned int stateBits = SB_GOOD;
	bool detached = false;

	virtual void __setStateBits(const unsigned int &bit, const bool set) noexcept;
	virtual void __accumilateSentSize(const size_t sent) noexcept;
	virtual void __accumilateRetrievedSize(const size_t sent) noexcept;

public:
	virtual ~TransitiveInterface() noexcept;

	virtual thisClass &post(const void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<unsigned char> &data) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<unsigned char> &data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &post() = 0; //@Pure virtual
	virtual thisClass &receive(void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<unsigned char> &data) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<unsigned char> &data, const size_t len) = 0; //@Pure virtual

	virtual bool blocking() const = 0; //@Pure virtual
	virtual thisClass &blocking(const bool blocking) = 0; //@Pure virtual
	virtual size_t bufferSize() const = 0; //@Pure virtual
	virtual thisClass &bufferSize(const size_t size) = 0; //@Pure virtual

	virtual bool valid() const = 0; //@Pure virtual
	virtual thisClass &detach() = 0; //@Pure virtual
	virtual bool hasDetached() const noexcept;

	virtual bool good() const noexcept;
	virtual bool failed() const noexcept;
	virtual bool delayed() const noexcept;
	virtual bool ended() const noexcept;
	virtual bool closed() const noexcept;

	virtual thisClass& descriptors(std::set<int>& y) = 0;

	virtual size_t retrieved(const bool overall = false) const noexcept;
	virtual size_t sent(const bool overall = false) const noexcept;

	virtual void onSentSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept;
	virtual void onRetrievedSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept;
};

} /* namespace ashe */

#endif /* TRANSITIVEINTERFACE_H_ */