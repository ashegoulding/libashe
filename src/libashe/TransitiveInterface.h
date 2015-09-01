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

#include "Frelish.h"
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <set>


namespace ashe
{

class ASHE_DECL_EXT TransitiveInterface
{
public:
	typedef TransitiveInterface thisClass;

	class ASHE_DECL_EXT TransitiveRune : public WeakRune
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

		static std::string codeToString__(const Code x) ASHE_NOEXCEPT;

	protected:
		// This is uint32_t for a reason
		uint32_t code = (uint32_t)C_UNKNOWN;

		void __construct(const thisClass &src) ASHE_NOEXCEPT;

	public:
		TransitiveRune(const Code code, const std::string msg = "") ASHE_NOEXCEPT;
		TransitiveRune(const thisClass &src) ASHE_NOEXCEPT;
		virtual ~TransitiveRune() ASHE_NOEXCEPT;

		virtual thisClass &operator =(const thisClass &src) ASHE_NOEXCEPT;

		virtual uint32_t getCode() const ASHE_NOEXCEPT;
	};

protected:
	const static uint32_t SB_GOOD/* =		 	0*/;
	// Last transmission failed.
	const static uint32_t SB_FAILED/* = 	0b00000000000000000000000000000001*/;
	// Last transmission delayed.
	const static uint32_t SB_DELAYED/* = 	0b00000000000000000000000000000010*/;
	// End of transmission: like shutdown in TCP socket
	const static uint32_t SB_ENDED/* = 		0b00000000000000000000000000000100*/;
	// The instance has closed (EOF)
	const static uint32_t SB_CLOSED/* = 	0b00000000000000000000000000001000*/;

	/* Transmission statistics.
	* - lastRetrievedSize: Retrieved data length on last transmission
	* - lastSentSize: Sent data length on last transmission
	* - retrievedSize: (Overall)
	* - sentSize: (Overall)
	*/
	uint64_t lastRetrievedSize = 0, lastSentSize = 0, retrievedSize = 0, sentSize = 0;
	uint32_t stateBits = SB_GOOD;
	// Detached state. Mostly for resource saving.
	bool __detached = false;

	virtual void __setStateBits(const uint32_t &bit, const bool set) ASHE_NOEXCEPT;
	virtual void __accumilateSentSize(const uint64_t sent) ASHE_NOEXCEPT;
	virtual void __accumilateRetrievedSize(const uint64_t sent) ASHE_NOEXCEPT;

public:
	virtual ~TransitiveInterface() ASHE_NOEXCEPT;

	virtual thisClass &post(const void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<uint8_t> &data) = 0; //@Pure virtual
	virtual thisClass &post(const std::vector<uint8_t> &data, const size_t len) = 0; //@Pure virtual
	// For non-blocking mode support
	virtual thisClass &post() = 0; //@Pure virtual
	virtual thisClass &receive(void *data, const size_t len) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<uint8_t> &data) = 0; //@Pure virtual
	virtual thisClass &receive(std::vector<uint8_t> &data, const size_t len) = 0; //@Pure virtual

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
	virtual bool detached() const ASHE_NOEXCEPT;

	// Tests if any bad bits are NOT set.
	virtual bool good() const ASHE_NOEXCEPT;
	// Tests if the fail bit set.
	virtual bool failed() const ASHE_NOEXCEPT;
	// Tests if the delayed bit set.
	virtual bool delayed() const ASHE_NOEXCEPT;
	// Tests if EOT bit set.
	virtual bool ended() const ASHE_NOEXCEPT;
	// Tests if EOF bit set.
	virtual bool closed() const ASHE_NOEXCEPT;

	// Returns actual descriptors that the instance is using.
	// For poll(), select() or overlapped IO wrapping class support.
	virtual thisClass &descriptors(std::set<int>& y) = 0;

	// Returns retrieved data length in bytes.
	virtual uint64_t retrieved(const bool overall = false) const ASHE_NOEXCEPT;
	// Returns sent data length in bytes.
	virtual uint64_t sent(const bool overall = false) const ASHE_NOEXCEPT;
	// Sets overall statistic members to zero.
	// NOTE: This does not set 'last' members also to zero. They shall be left intact.
	virtual thisClass &resetStatistics(const bool sent = true, const bool retrieved = true) ASHE_NOEXCEPT;

	// Event handler for 'sentSize' member overflow.
	// NOTE: It is about 16,384PB
	virtual void onSentSizeOverflow(const uint64_t sizeToAdd, const uint64_t previousSize) ASHE_NOEXCEPT;
	// Event handler for 'retrievedSize' member overflow.
	// NOTE: It is about 16,384PB
	virtual void onRetrievedSizeOverflow(const uint64_t sizeToAdd, const uint64_t previousSize) ASHE_NOEXCEPT;
};

} /* namespace ashe */

#endif /* TRANSITIVEINTERFACE_H_ */
