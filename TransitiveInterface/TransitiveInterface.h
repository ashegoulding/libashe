/*
* TransitiveInterface.h
*
* @Maintained
*  Under development since: 2015 Q1
* @Author
*  Ashe David Sterkhus
*  Blame to: ashe.goulding+blame@gmail.com
* @COLOPHON
*  This file is part of libashe, Ashe's C++11/98 utility stuff
*
* @DESCRIPTION
*  - Supreme interface for transitive descriptor like pipe, fifo, socket
*/
#ifndef ASHE_TRANSITIVEINTERFACE_H_
#define ASHE_TRANSITIVEINTERFACE_H_

#include <cstdlib>
#include <vector>
#include <set>

#include "Frelish.h"

namespace ashe
{

class TransitiveInterface
{
public:
	typedef TransitiveInterface thisClass;

	class TransitiveRune : public WeakRune
	{
	/* Inheritable Rune class.
	* Inherit this class to specify exceptions on: socket
	*/
	public:
		typedef WeakRune motherClass;
		typedef TransitiveRune thisClass;

		enum Code
		{
			C_UNKNOWN,
			C_ILLEGAL_STATE, // The instance is not valid or has detached
			C_INTERNAL_ERROR, // Error returned from posix function. Rune errno shall be properly set.
			C_DELAYED, // Transfer delayed whilst non-blocking mode.
			C_NO_DATA_GIVEN, // No data given for send() or to retrieve()
			C_SHORT_LENGTH, // Given data range is shorter than specified length, which would cause SEGFAULT
			C_EMPTY_BUFFER // The buffer of descriptor(s) is empty, no data to read
		};

		static std::string codeToString__(const Code x) noexcept;

	protected:
		// This is unsigned int for a reason
		unsigned int code = (unsigned int)C_UNKNOWN;

		void __construct(const thisClass &src) noexcept;

	public:
		TransitiveRune(const Code code, const std::string msg = "") noexcept;
		TransitiveRune(const thisClass &src) noexcept;
		virtual ~TransitiveRune() noexcept;

		virtual thisClass &operator =(const thisClass &src) noexcept;

		virtual unsigned int getCode() const noexcept;
	};

protected:
	const static unsigned int SB_GOOD/* =		 	0*/;
	// Last transmission failed.
	const static unsigned int SB_FAILED/* = 	0b00000000000000000000000000000001*/;
	// Last transmission delayed.
	const static unsigned int SB_DELAYED/* = 	0b00000000000000000000000000000010*/;
	// End of transmission: like shutdown in TCP socket
	const static unsigned int SB_ENDED/* = 		0b00000000000000000000000000000100*/;
	// The instance has closed (EOF)
	const static unsigned int SB_CLOSED/* = 	0b00000000000000000000000000001000*/;

	/* Transmission statistics.
	* - lastRetrievedSize: Retrieved data length on last transmission
	* - lastSentSize: Sent data length on last transmission
	* - retrievedSize: (Overall)
	* - sentSize: (Overall)
	*/
	size_t lastRetrievedSize = 0, lastSentSize = 0, retrievedSize = 0, sentSize = 0;
	unsigned int stateBits = SB_GOOD;
	// Detached state. Mostly for resource saving.
	bool __detached = false;

	virtual void __setStateBits(const unsigned int &bit, const bool set) noexcept;
	virtual void __accumilateSentSize(const size_t sent) noexcept;
	virtual void __accumilateRetrievedSize(const size_t sent) noexcept;

public:
	virtual ~TransitiveInterface() noexcept;

	virtual thisClass &post(const void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<unsigned char> &data) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<unsigned char> &data, const size_t len) = 0; //@Pure virtual
	// For non-blocking mode support
	virtual thisClass &post() = 0; //@Pure virtual
	virtual thisClass &receive(void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<unsigned char> &data) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<unsigned char> &data, const size_t len) = 0; //@Pure virtual

	// Tests if the instance is on non-blocking mode.
	virtual bool blocking() const = 0; //@Pure virtual
	// Sets the blocking mode.
	virtual thisClass &blocking(const bool blocking) = 0; //@Pure virtual
	// Returns the actual buffer size. Actual buffer size can differ from requested buffer size setting.
	virtual size_t bufferSize() const = 0; //@Pure virtual
	// Sets the buffer size. Actual buffer size can differ from requested buffer size setting.
	virtual thisClass &bufferSize(const size_t size) = 0; //@Pure virtual

	// Tests if the instance is in valid-state
	virtual bool valid() const = 0; //@Pure virtual
	// Detaches the instance for another instance to continue to use.
	// For resource saving: Inserting instances into std containers like std::list
	virtual thisClass &detach() = 0; //@Pure virtual
	// Tests if the instance has detached.
	virtual bool detached() const noexcept;

	// Tests if any bad bits are NOT set.
	virtual bool good() const noexcept;
	// Tests if the fail bit set.
	virtual bool failed() const noexcept;
	// Tests if the delayed bit set.
	virtual bool delayed() const noexcept;
	// Tests if EOT bit set.
	virtual bool ended() const noexcept;
	// Tests if EOF bit set.
	virtual bool closed() const noexcept;

	// Returns actual descriptors that the instance is using.
	// For poll(), select() or overlapped IO wrapping class support.
	virtual thisClass &descriptors(std::set<int>& y) = 0;

	// Returns retrieved data length in bytes.
	virtual size_t retrieved(const bool overall = false) const noexcept;
	// Returns sent data length in bytes.
	virtual size_t sent(const bool overall = false) const noexcept;
	// Sets overall statistic members to zero.
	// NOTE: This does not set 'last' members also to zero. They shall be left intact.
	virtual thisClass &resetStatistics(const bool sent = true, const bool retrieved = true) noexcept;

	// Event handler for 'sentSize' member overflow.
	// NOTE: It is about 16,384PB on 64bits machines, 4GB on 32bits. This may vary by size_t's actual type.
	virtual void onSentSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept;
	// Event handler for 'retrievedSize' member overflow.
	// NOTE: It is about 16,384PB on 64bits machines, 4GB on 32bits. This may vary by size_t's actual type.
	virtual void onRetrievedSizeOverflow(const size_t sizeToAdd, const size_t previousSize) noexcept;
};

} /* namespace ashe */

#endif /* TRANSITIVEINTERFACE_H_ */
