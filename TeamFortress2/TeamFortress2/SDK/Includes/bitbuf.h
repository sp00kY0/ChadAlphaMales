//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

// NOTE: bf_read is guaranteed to return zeros if it overflows.


#pragma once


#include <stdint.h>
#include <cassert>
#include "../../Utils/Math/Math.h"

#define Assert assert

#if _DEBUG
#define BITBUF_INLINE inline
#else
#define BITBUF_INLINE FORCEINLINE
#endif

#define RESTRICT

//-----------------------------------------------------------------------------
// You can define a handler function that will be called in case of 
// out-of-range values and overruns here.
//
// NOTE: the handler is only called in debug mode.
//
// Call SetBitBufErrorHandler to install a handler.
//-----------------------------------------------------------------------------

typedef enum
{
	BITBUFERROR_VALUE_OUT_OF_RANGE = 0,		// Tried to write a value with too few bits.
	BITBUFERROR_BUFFER_OVERRUN,				// Was about to overrun a buffer.

	BITBUFERROR_NUM_ERRORS
} BitBufErrorType;


typedef void(*BitBufErrorHandler)(BitBufErrorType errorType, const char* pDebugName);


#if defined( _DEBUG )
extern void InternalBitBufErrorHandler(BitBufErrorType errorType, const char* pDebugName);
#define CallErrorHandler( errorType, pDebugName ) InternalBitBufErrorHandler( errorType, pDebugName );
#else
#define CallErrorHandler( errorType, pDebugName )
#endif


// Use this to install the error handler. Call with NULL to uninstall your error handler.
void SetBitBufErrorHandler(BitBufErrorHandler fn);

#define BITS_PER_INT 32

//-----------------------------------------------------------------------------
// Helpers.
//-----------------------------------------------------------------------------
template <typename T>
inline T WordSwapC(T w)
{
	uint16_t temp;

	temp = ((*((uint16_t*)&w) & 0xff00) >> 8);
	temp |= ((*((uint16_t*)&w) & 0x00ff) << 8);

	return *((T*)&temp);
}

template <typename T>
inline T DWordSwapC(T dw)
{
	uint32_t temp;

	temp = *((uint32_t*)&dw) >> 24;
	temp |= ((*((uint32_t*)&dw) & 0x00FF0000) >> 8);
	temp |= ((*((uint32_t*)&dw) & 0x0000FF00) << 8);
	temp |= ((*((uint32_t*)&dw) & 0x000000FF) << 24);

	return *((T*)&temp);
}

template <typename T>
inline T QWordSwapC(T dw)
{
	// Assert sizes passed to this are already correct, otherwise
	// the cast to uint64_t * below is unsafe and may have wrong results 
	// or even crash.
	//PLAT_COMPILE_TIME_ASSERT(sizeof(dw) == sizeof(uint64_t));

	uint64_t temp;

	temp = *((uint64_t*)&dw) >> 56;
	temp |= ((*((uint64_t*)&dw) & 0x00FF000000000000ull) >> 40);
	temp |= ((*((uint64_t*)&dw) & 0x0000FF0000000000ull) >> 24);
	temp |= ((*((uint64_t*)&dw) & 0x000000FF00000000ull) >> 8);
	temp |= ((*((uint64_t*)&dw) & 0x00000000FF000000ull) << 8);
	temp |= ((*((uint64_t*)&dw) & 0x0000000000FF0000ull) << 24);
	temp |= ((*((uint64_t*)&dw) & 0x000000000000FF00ull) << 40);
	temp |= ((*((uint64_t*)&dw) & 0x00000000000000FFull) << 56);

	return *((T*)&temp);
}

inline int BitByte(int bits)
{
	// return PAD_NUMBER( bits, 8 ) >> 3;
	return (bits + 7) >> 3;
}

inline unsigned GetEndMask(int numBits)
{
	static unsigned bitStringEndMasks[] =
	{
		0xffffffff,
		0x00000001,
		0x00000003,
		0x00000007,
		0x0000000f,
		0x0000001f,
		0x0000003f,
		0x0000007f,
		0x000000ff,
		0x000001ff,
		0x000003ff,
		0x000007ff,
		0x00000fff,
		0x00001fff,
		0x00003fff,
		0x00007fff,
		0x0000ffff,
		0x0001ffff,
		0x0003ffff,
		0x0007ffff,
		0x000fffff,
		0x001fffff,
		0x003fffff,
		0x007fffff,
		0x00ffffff,
		0x01ffffff,
		0x03ffffff,
		0x07ffffff,
		0x0fffffff,
		0x1fffffff,
		0x3fffffff,
		0x7fffffff,
	};

	return bitStringEndMasks[numBits % BITS_PER_INT];
}


inline int GetBitForBitnum(int bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
		(1 << 8),
		(1 << 9),
		(1 << 10),
		(1 << 11),
		(1 << 12),
		(1 << 13),
		(1 << 14),
		(1 << 15),
		(1 << 16),
		(1 << 17),
		(1 << 18),
		(1 << 19),
		(1 << 20),
		(1 << 21),
		(1 << 22),
		(1 << 23),
		(1 << 24),
		(1 << 25),
		(1 << 26),
		(1 << 27),
		(1 << 28),
		(1 << 29),
		(1 << 30),
		(1 << 31),
	};

	return bitsForBitnum[(bitNum) & (BITS_PER_INT - 1)];
}

inline int GetBitForBitnumByte(int bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
	};

	return bitsForBitnum[bitNum & 7];
}

inline int CalcNumIntsForBits(int numBits) { return (numBits + (BITS_PER_INT - 1)) / BITS_PER_INT; }



#define WordSwap  WordSwapC
#define DWordSwap DWordSwapC
#define QWordSwap QWordSwapC


#define SafeSwapFloat( pOut, pIn )	(*((unsigned int*)pOut) = DWordSwap( *((unsigned int*)pIn) ))
inline short BigShort(short val) { int test = 1; return (*(char*)&test == 1) ? WordSwap(val) : val; }
inline uint16_t BigWord(uint16_t val) { int test = 1; return (*(char*)&test == 1) ? WordSwap(val) : val; }
inline long BigLong(long val) { int test = 1; return (*(char*)&test == 1) ? DWordSwap(val) : val; }
inline uint32_t BigDWord(uint32_t val) { int test = 1; return (*(char*)&test == 1) ? DWordSwap(val) : val; }
inline short LittleShort(short val) { int test = 1; return (*(char*)&test == 1) ? val : WordSwap(val); }
inline uint16_t LittleWord(uint16_t val) { int test = 1; return (*(char*)&test == 1) ? val : WordSwap(val); }
inline long LittleLong(long val) { int test = 1; return (*(char*)&test == 1) ? val : DWordSwap(val); }
inline uint32_t LittleDWord(uint32_t val) { int test = 1; return (*(char*)&test == 1) ? val : DWordSwap(val); }
inline uint64_t LittleQWord(uint64_t val) { int test = 1; return (*(char*)&test == 1) ? val : QWordSwap(val); }
inline short SwapShort(short val) { return WordSwap(val); }
inline uint16_t SwapWord(uint16_t val) { return WordSwap(val); }
inline long SwapLong(long val) { return DWordSwap(val); }
inline uint32_t SwapDWord(uint32_t val) { return DWordSwap(val); }

// Pass floats by pointer for swapping to avoid truncation in the fpu
inline void BigFloat(float* pOut, const float* pIn) { int test = 1; (*(char*)&test == 1) ? SafeSwapFloat(pOut, pIn) : (*pOut = *pIn); }
inline void LittleFloat(float* pOut, const float* pIn) { int test = 1; (*(char*)&test == 1) ? (*pOut = *pIn) : SafeSwapFloat(pOut, pIn); }
inline void SwapFloat(float* pOut, const float* pIn) { SafeSwapFloat(pOut, pIn); }


#define FORCEINLINE __forceinline

FORCEINLINE unsigned long LoadLittleDWord(const unsigned long* base, unsigned int dwordIndex)
{
	return LittleDWord(base[dwordIndex]);
}

FORCEINLINE void StoreLittleDWord(unsigned long* base, unsigned int dwordIndex, unsigned long dword)
{
	base[dwordIndex] = LittleDWord(dword);
}



//-----------------------------------------------------------------------------
// namespaced helpers
//-----------------------------------------------------------------------------
namespace bitbuf
{
	// ZigZag Transform:  Encodes signed integers so that they can be
	// effectively used with varint encoding.
	//
	// varint operates on unsigned integers, encoding smaller numbers into
	// fewer bytes.  If you try to use it on a signed integer, it will treat
	// this number as a very large unsigned integer, which means that even
	// small signed numbers like -1 will take the maximum number of bytes
	// (10) to encode.  ZigZagEncode() maps signed integers to unsigned
	// in such a way that those with a small absolute value will have smaller
	// encoded values, making them appropriate for encoding using varint.
	//
	//       int32_t ->     uint32_t
	// -------------------------
	//           0 ->          0
	//          -1 ->          1
	//           1 ->          2
	//          -2 ->          3
	//         ... ->        ...
	//  2147483647 -> 4294967294
	// -2147483648 -> 4294967295
	//
	//        >> encode >>
	//        << decode <<

	inline uint32_t ZigZagEncode32(int32_t n)
	{
		// Note:  the right-shift must be arithmetic
		return(n << 1) ^ (n >> 31);
	}

	inline int32_t ZigZagDecode32(uint32_t n)
	{
		return(n >> 1) ^ -static_cast<int32_t>(n & 1);
	}

	inline uint64_t ZigZagEncode64(int64_t n)
	{
		// Note:  the right-shift must be arithmetic
		return(n << 1) ^ (n >> 63);
	}

	inline int64_t ZigZagDecode64(uint64_t n)
	{
		return(n >> 1) ^ -static_cast<int64_t>(n & 1);
	}

	const int kMaxVarintBytes = 10;
	const int kMaxVarint32Bytes = 5;
}

//-----------------------------------------------------------------------------
// Used for serialization
//-----------------------------------------------------------------------------

class bf_write
{
public:
	bf_write();

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nMaxBits = -1);
	bf_write(const char* pDebugName, void* pData, int nBytes, int nMaxBits = -1);

	// Start writing to the specified buffer.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void			StartWriting(void* pData, int nBytes, int iStartBit = 0, int nMaxBits = -1);

	// Restart buffer writing.
	void			Reset();

	// Get the base pointer.
	unsigned char* GetBasePointer() { return (unsigned char*)m_pData; }

	// Enable or disable assertion on overflow. 99% of the time, it's a bug that we need to catch,
	// but there may be the occasional buffer that is allowed to overflow gracefully.
	void			SetAssertOnOverflow(bool bAssert);

	// This can be set to assign a name that gets output if the buffer overflows.
	const char* GetDebugName();
	void			SetDebugName(const char* pDebugName);


	// Seek to a specific position.
public:

	void			SeekToBit(int bitPos);


	// Bit functions.
public:

	void			WriteOneBit(int nValue);
	void			WriteOneBitNoCheck(int nValue);
	void			WriteOneBitAt(int iBit, int nValue);

	// Write signed or unsigned. Range is only checked in debug.
	void			WriteUBitLong(unsigned int data, int numbits, bool bCheckRange = true);
	void			WriteSBitLong(int data, int numbits);

	// Tell it whether or not the data is unsigned. If it's signed,
	// cast to unsigned before passing in (it will cast back inside).
	void			WriteBitLong(unsigned int data, int numbits, bool bSigned);

	// Write a list of bits in.
	bool			WriteBits(const void* pIn, int nBits);

	// writes an unsigned integer with variable bit length
	void			WriteUBitVar(unsigned int data);

	// writes a varint encoded integer
	void			WriteVarInt32(uint32_t data);
	void			WriteVarInt64(uint64_t data);
	void			WriteSignedVarInt32(int32_t data);
	void			WriteSignedVarInt64(int64_t data);
	int				ByteSizeVarInt32(uint32_t data);
	int				ByteSizeVarInt64(uint64_t data);
	int				ByteSizeSignedVarInt32(int32_t data);
	int				ByteSizeSignedVarInt64(int64_t data);

	// Copy the bits straight out of pIn. This seeks pIn forward by nBits.
	// Returns an error if this buffer or the read buffer overflows.
	bool			WriteBitsFromBuffer(class bf_read* pIn, int nBits);

	void			WriteBitAngle(float fAngle, int numbits);
	void			WriteBitCoord(const float f);
	void			WriteBitCoordMP(const float f, bool bIntegral, bool bLowPrecision);
	void			WriteBitFloat(float val);
	void			WriteBitVec3Coord(const Vec3& fa);
	void			WriteBitNormal(float f);
	void			WriteBitVec3Normal(const Vec3& fa);
	void			WriteBitAngles(const Vec3& fa);


	// Byte functions.
public:

	void			WriteChar(int val);
	void			WriteByte(int val);
	void			WriteShort(int val);
	void			WriteWord(int val);
	void			WriteLong(long val);
	void			WriteLongLong(int64_t val);
	void			WriteFloat(float val);
	bool			WriteBytes(const void* pBuf, int nBytes);

	// Returns false if it overflows the buffer.
	bool			WriteString(const char* pStr);


	// Status.
public:

	// How many bytes are filled in?
	int				GetNumBytesWritten() const;
	int				GetNumBitsWritten() const;
	int				GetMaxNumBits();
	int				GetNumBitsLeft();
	int				GetNumBytesLeft();
	unsigned char* GetData();
	const unsigned char* GetData() const;

	// Has the buffer overflowed?
	bool			CheckForOverflow(int nBits);
	inline bool		IsOverflowed() const { return m_bOverflow; }

	void			SetOverflowFlag();


public:
	// The current buffer.
	unsigned long* RESTRICT m_pData;
	int				m_nDataBytes;
	int				m_nDataBits;

	// Where we are in the buffer.
	int				m_iCurBit;

private:

	// Errors?
	bool			m_bOverflow;

	bool			m_bAssertOnOverflow;
	const char* m_pDebugName;
};


//-----------------------------------------------------------------------------
// Inlined methods
//-----------------------------------------------------------------------------

// How many bytes are filled in?
inline int bf_write::GetNumBytesWritten() const
{
	return BitByte(m_iCurBit);
}

inline int bf_write::GetNumBitsWritten() const
{
	return m_iCurBit;
}

inline int bf_write::GetMaxNumBits()
{
	return m_nDataBits;
}

inline int bf_write::GetNumBitsLeft()
{
	return m_nDataBits - m_iCurBit;
}

inline int bf_write::GetNumBytesLeft()
{
	return GetNumBitsLeft() >> 3;
}

inline unsigned char* bf_write::GetData()
{
	return (unsigned char*)m_pData;
}

inline const unsigned char* bf_write::GetData()	const
{
	return (unsigned char*)m_pData;
}

BITBUF_INLINE bool bf_write::CheckForOverflow(int nBits)
{
	if (m_iCurBit + nBits > m_nDataBits)
	{
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
	}

	return m_bOverflow;
}

BITBUF_INLINE void bf_write::SetOverflowFlag()
{
#ifdef DBGFLAG_ASSERT
	if (m_bAssertOnOverflow)
	{
		Assert(false);
	}
#endif
	m_bOverflow = true;
}

BITBUF_INLINE void bf_write::WriteOneBitNoCheck(int nValue)
{
#if __i386__
	if (nValue)
		m_pData[m_iCurBit >> 5] |= 1u << (m_iCurBit & 31);
	else
		m_pData[m_iCurBit >> 5] &= ~(1u << (m_iCurBit & 31));
#else
	extern unsigned long g_LittleBits[32];
	if (nValue)
		m_pData[m_iCurBit >> 5] |= g_LittleBits[m_iCurBit & 31];
	else
		m_pData[m_iCurBit >> 5] &= ~g_LittleBits[m_iCurBit & 31];
#endif

	++m_iCurBit;
}

inline void bf_write::WriteOneBit(int nValue)
{
	if (m_iCurBit >= m_nDataBits)
	{
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return;
	}
	WriteOneBitNoCheck(nValue);
}


inline void	bf_write::WriteOneBitAt(int iBit, int nValue)
{
	if (iBit >= m_nDataBits)
	{
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return;
	}

#if __i386__
	if (nValue)
		m_pData[iBit >> 5] |= 1u << (iBit & 31);
	else
		m_pData[iBit >> 5] &= ~(1u << (iBit & 31));
#else
	extern unsigned long g_LittleBits[32];
	if (nValue)
		m_pData[iBit >> 5] |= g_LittleBits[iBit & 31];
	else
		m_pData[iBit >> 5] &= ~g_LittleBits[iBit & 31];
#endif
}

BITBUF_INLINE void bf_write::WriteUBitLong(unsigned int curData, int numbits, bool bCheckRange) RESTRICT
{
#ifdef _DEBUG
	// Make sure it doesn't overflow.
	if (bCheckRange && numbits < 32)
	{
		if (curData >= (unsigned long)(1 << numbits))
		{
			CallErrorHandler(BITBUFERROR_VALUE_OUT_OF_RANGE, GetDebugName());
		}
	}
	Assert(numbits >= 0 && numbits <= 32);
#endif

	if (GetNumBitsLeft() < numbits)
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return;
	}

	int iCurBitMasked = m_iCurBit & 31;
	int iDWord = m_iCurBit >> 5;
	m_iCurBit += numbits;

	// Mask in a dword.
	Assert((iDWord * 4 + sizeof(long)) <= (unsigned int)m_nDataBytes);
	unsigned long* RESTRICT pOut = &m_pData[iDWord];

	// Rotate data into dword alignment
	curData = (curData << iCurBitMasked) | (curData >> (32 - iCurBitMasked));

	// Calculate bitmasks for first and second word
	unsigned int temp = 1 << (numbits - 1);
	unsigned int mask1 = (temp * 2 - 1) << iCurBitMasked;
	unsigned int mask2 = (temp - 1) >> (31 - iCurBitMasked);

	// Only look beyond current word if necessary (avoid access violation)
	int i = mask2 & 1;
	unsigned long dword1 = LoadLittleDWord(pOut, 0);
	unsigned long dword2 = LoadLittleDWord(pOut, i);

	// Drop bits into place
	dword1 ^= (mask1 & (curData ^ dword1));
	dword2 ^= (mask2 & (curData ^ dword2));

	// Note reversed order of writes so that dword1 wins if mask2 == 0 && i == 0
	StoreLittleDWord(pOut, i, dword2);
	StoreLittleDWord(pOut, 0, dword1);
}

// writes an unsigned integer with variable bit length
BITBUF_INLINE void bf_write::WriteUBitVar(unsigned int data)
{
	/* Reference:
	if ( data < 0x10u )
		WriteUBitLong( 0, 2 ), WriteUBitLong( data, 4 );
	else if ( data < 0x100u )
		WriteUBitLong( 1, 2 ), WriteUBitLong( data, 8 );
	else if ( data < 0x1000u )
		WriteUBitLong( 2, 2 ), WriteUBitLong( data, 12 );
	else
		WriteUBitLong( 3, 2 ), WriteUBitLong( data, 32 );
	*/
	// a < b ? -1 : 0 translates into a CMP, SBB instruction pair
	// with no flow control. should also be branchless on consoles.
	int n = (data < 0x10u ? -1 : 0) + (data < 0x100u ? -1 : 0) + (data < 0x1000u ? -1 : 0);
	WriteUBitLong(data * 4 + n + 3, 6 + n * 4 + 12);
	if (data >= 0x1000u)
	{
		WriteUBitLong(data >> 16, 16);
	}
}

// write raw IEEE float bits in little endian form
BITBUF_INLINE void bf_write::WriteBitFloat(float val)
{
	long intVal;

	Assert(sizeof(long) == sizeof(float));
	Assert(sizeof(float) == 4);

	intVal = *((long*)&val);
	WriteUBitLong(intVal, 32);
}

//-----------------------------------------------------------------------------
// This is useful if you just want a buffer to write into on the stack.
//-----------------------------------------------------------------------------

template<int SIZE>
class old_bf_write_static : public bf_write
{
public:
	inline old_bf_write_static() : bf_write(m_StaticData, SIZE) {}

	char	m_StaticData[SIZE];
};



//-----------------------------------------------------------------------------
// Used for unserialization
//-----------------------------------------------------------------------------

class bf_read
{
public:
	bf_read();

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_read(const void* pData, int nBytes, int nBits = -1);
	bf_read(const char* pDebugName, const void* pData, int nBytes, int nBits = -1);

	// Start reading from the specified buffer.
	// pData's start address must be dword-aligned.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void			StartReading(const void* pData, int nBytes, int iStartBit = 0, int nBits = -1);

	// Restart buffer reading.
	void			Reset();

	// Enable or disable assertion on overflow. 99% of the time, it's a bug that we need to catch,
	// but there may be the occasional buffer that is allowed to overflow gracefully.
	void			SetAssertOnOverflow(bool bAssert);

	// This can be set to assign a name that gets output if the buffer overflows.
	const char* GetDebugName() const { return m_pDebugName; }
	void			SetDebugName(const char* pName);

	void			ExciseBits(int startbit, int bitstoremove);


	// Bit functions.
public:

	// Returns 0 or 1.
	int				ReadOneBit();


protected:

	unsigned int	CheckReadUBitLong(int numbits);		// For debugging.
	int				ReadOneBitNoCheck();				// Faster version, doesn't check bounds and is inlined.
	bool			CheckForOverflow(int nBits);


public:

	// Get the base pointer.
	const unsigned char* GetBasePointer() { return m_pData; }

	BITBUF_INLINE int TotalBytesAvailable(void) const
	{
		return m_nDataBytes;
	}

	// Read a list of bits in.
	void            ReadBits(void* pOut, int nBits);
	// Read a list of bits in, but don't overrun the destination buffer.
	// Returns the number of bits read into the buffer. The remaining
	// bits are skipped over.
	int             ReadBitsClamped_ptr(void* pOut, size_t outSizeBytes, size_t nBits);
	// Helper 'safe' template function that infers the size of the destination
	// array. This version of the function should be preferred.
	// Usage: char databuffer[100];
	//        ReadBitsClamped( dataBuffer, msg->m_nLength );
	template <typename T, size_t N>
	int             ReadBitsClamped(T(&pOut)[N], size_t nBits)
	{
		return ReadBitsClamped_ptr(pOut, N * sizeof(T), nBits);
	}

	float			ReadBitAngle(int numbits);

	unsigned int	ReadUBitLong(int numbits) RESTRICT;
	unsigned int	ReadUBitLongNoInline(int numbits) RESTRICT;
	unsigned int	PeekUBitLong(int numbits);
	int				ReadSBitLong(int numbits);

	// reads an unsigned integer with variable bit length
	unsigned int	ReadUBitVar();
	unsigned int	ReadUBitVarInternal(int encodingType);

	// reads a varint encoded integer
	uint32_t			ReadVarInt32();
	uint64_t			ReadVarInt64();
	int32_t			ReadSignedVarInt32();
	int64_t			ReadSignedVarInt64();

	// You can read signed or unsigned data with this, just cast to 
	// a signed int if necessary.
	unsigned int	ReadBitLong(int numbits, bool bSigned);

	float			ReadBitCoord();
	float			ReadBitCoordMP(bool bIntegral, bool bLowPrecision);
	float			ReadBitFloat();
	float			ReadBitNormal();
	void			ReadBitVec3Coord(Vec3& fa);
	void			ReadBitVec3Normal(Vec3& fa);
	void			ReadBitAngles(Vec3& fa);

	// Faster for comparisons but do not fully decode float values
	unsigned int	ReadBitCoordBits();
	unsigned int	ReadBitCoordMPBits(bool bIntegral, bool bLowPrecision);

	// Byte functions (these still read data in bit-by-bit).
public:

	BITBUF_INLINE int	ReadChar() { return (char)ReadUBitLong(8); }
	BITBUF_INLINE int	ReadByte() { return ReadUBitLong(8); }
	BITBUF_INLINE int	ReadShort() { return (short)ReadUBitLong(16); }
	BITBUF_INLINE int	ReadWord() { return ReadUBitLong(16); }
	BITBUF_INLINE long ReadLong() { return ReadUBitLong(32); }
	int64_t			ReadLongLong();
	float			ReadFloat();
	bool			ReadBytes(void* pOut, int nBytes);

	// Returns false if bufLen isn't large enough to hold the
	// string in the buffer.
	//
	// Always reads to the end of the string (so you can read the
	// next piece of data waiting).
	//
	// If bLine is true, it stops when it reaches a '\n' or a null-terminator.
	//
	// pStr is always null-terminated (unless bufLen is 0).
	//
	// pOutNumChars is set to the number of characters left in pStr when the routine is 
	// complete (this will never exceed bufLen-1).
	//
	bool			ReadString(char* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL);

	// Reads a string and allocates memory for it. If the string in the buffer
	// is > 2048 bytes, then pOverflow is set to true (if it's not NULL).
	char* ReadAndAllocateString(bool* pOverflow = 0);

	// Returns nonzero if any bits differ
	int				CompareBits(bf_read* RESTRICT other, int bits) RESTRICT;
	int				CompareBitsAt(int offset, bf_read* RESTRICT other, int otherOffset, int bits) RESTRICT;

	// Status.
public:
	int				GetNumBytesLeft();
	int				GetNumBytesRead();
	int				GetNumBitsLeft();
	int				GetNumBitsRead() const;

	// Has the buffer overflowed?
	inline bool		IsOverflowed() const { return m_bOverflow; }

	inline bool		Seek(int iBit);					// Seek to a specific bit.
	inline bool		SeekRelative(int iBitDelta);	// Seek to an offset from the current position.

	// Called when the buffer is overflowed.
	void			SetOverflowFlag();


public:

	// The current buffer.
	const unsigned char* RESTRICT m_pData;
	int						m_nDataBytes;
	int						m_nDataBits;

	// Where we are in the buffer.
	int				m_iCurBit;


private:
	// Errors?
	bool			m_bOverflow;

	// For debugging..
	bool			m_bAssertOnOverflow;

	const char* m_pDebugName;
};

//-----------------------------------------------------------------------------
// Inlines.
//-----------------------------------------------------------------------------

inline int bf_read::GetNumBytesRead()
{
	return BitByte(m_iCurBit);
}

inline int bf_read::GetNumBitsLeft()
{
	return m_nDataBits - m_iCurBit;
}

inline int bf_read::GetNumBytesLeft()
{
	return GetNumBitsLeft() >> 3;
}

inline int bf_read::GetNumBitsRead() const
{
	return m_iCurBit;
}

inline bool bf_read::Seek(int iBit)
{
	if (iBit < 0 || iBit > m_nDataBits)
	{
		SetOverflowFlag();
		m_iCurBit = m_nDataBits;
		return false;
	}
	else
	{
		m_iCurBit = iBit;
		return true;
	}
}

// Seek to an offset from the current position.
inline bool	bf_read::SeekRelative(int iBitDelta)
{
	return Seek(m_iCurBit + iBitDelta);
}

inline bool bf_read::CheckForOverflow(int nBits)
{
	if (m_iCurBit + nBits > m_nDataBits)
	{
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
	}

	return m_bOverflow;
}

inline int bf_read::ReadOneBitNoCheck()
{
#if VALVE_LITTLE_ENDIAN
	unsigned int value = ((unsigned long* RESTRICT)m_pData)[m_iCurBit >> 5] >> (m_iCurBit & 31);
#else
	unsigned char value = m_pData[m_iCurBit >> 3] >> (m_iCurBit & 7);
#endif
	++m_iCurBit;
	return value & 1;
}

inline int bf_read::ReadOneBit()
{
	if (GetNumBitsLeft() <= 0)
	{
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return 0;
	}
	return ReadOneBitNoCheck();
}

inline float bf_read::ReadBitFloat()
{
	union { uint32_t u; float f; } c = { ReadUBitLong(32) };
	return c.f;
}

BITBUF_INLINE unsigned int bf_read::ReadUBitVar()
{
	// six bits: low 2 bits for encoding + first 4 bits of value
	unsigned int sixbits = ReadUBitLong(6);
	unsigned int encoding = sixbits & 3;
	if (encoding)
	{
		// this function will seek back four bits and read the full value
		return ReadUBitVarInternal(encoding);
	}
	return sixbits >> 2;
}

BITBUF_INLINE unsigned int bf_read::ReadUBitLong(int numbits) RESTRICT
{
	Assert(numbits > 0 && numbits <= 32);

	if (GetNumBitsLeft() < numbits)
	{
		m_iCurBit = m_nDataBits;
		SetOverflowFlag();
		CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
		return 0;
	}

	unsigned int iStartBit = m_iCurBit & 31u;
	int iLastBit = m_iCurBit + numbits - 1;
	unsigned int iWordOffset1 = m_iCurBit >> 5;
	unsigned int iWordOffset2 = iLastBit >> 5;
	m_iCurBit += numbits;

#if __i386__
	unsigned int bitmask = (2 << (numbits - 1)) - 1;
#else
	extern unsigned long g_ExtraMasks[33];
	unsigned int bitmask = g_ExtraMasks[numbits];
#endif

	unsigned int dw1 = LoadLittleDWord((unsigned long* RESTRICT)m_pData, iWordOffset1) >> iStartBit;
	unsigned int dw2 = LoadLittleDWord((unsigned long* RESTRICT)m_pData, iWordOffset2) << (32 - iStartBit);

	return (dw1 | dw2) & bitmask;
}

BITBUF_INLINE int bf_read::CompareBits(bf_read* RESTRICT other, int numbits) RESTRICT
{
	return (ReadUBitLong(numbits) != other->ReadUBitLong(numbits));
}

