#pragma once
#include "../Includes/Includes.h"
/*class bf_read
{
public:
	unsigned long		*m_pData;
	int					m_nDataBytes;
	int					m_nDataBits;
	int					m_iCurBit;
	bool				m_bOverflow;
	bool				m_bAssertOnOverflow;
	const char			*m_pDebugName;
};
*/
class bf_write
{
public:
	int StartWriting(void *pData, int nBytes, int iStartBit = 0, int nBits = -1);

public:
	unsigned long	*m_pData;
	int				m_nDataBytes;
	int				m_nDataBits;
	int				m_iCurBit;
	bool			m_bOverflow;
	bool			m_bAssertOnOverflow;
	const char		*m_pDebugName;
};



#ifndef BITBUF_H
#define BITBUF_H

#ifdef _WIN32
#pragma once
#endif

#ifdef _DEBUG
#if !defined( PLAT_COMPILE_TIME_ASSERT )
#define PLAT_COMPILE_TIME_ASSERT( pred )	switch(0){case 0:case pred:;}
#endif
#else
#if !defined( PLAT_COMPILE_TIME_ASSERT )
#define PLAT_COMPILE_TIME_ASSERT(pred)
#endif
#endif

// OVERALL Coordinate Size Limits used in COMMON.C MSG_*BitCoord() Routines (and someday the HUD)
#define	COORD_INTEGER_BITS 14
#define COORD_FRACTIONAL_BITS 5
#define COORD_DENOMINATOR (1<<(COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION (1.0/(COORD_DENOMINATOR))

// Special threshold for networking multiplayer origins
#define COORD_INTEGER_BITS_MP 11
#define COORD_FRACTIONAL_BITS_MP_LOWPRECISION 3
#define COORD_DENOMINATOR_LOWPRECISION (1<<(COORD_FRACTIONAL_BITS_MP_LOWPRECISION))
#define COORD_RESOLUTION_LOWPRECISION (1.0/(COORD_DENOMINATOR_LOWPRECISION))

#define NORMAL_FRACTIONAL_BITS 11
#define NORMAL_DENOMINATOR ( (1<<(NORMAL_FRACTIONAL_BITS)) - 1 )
#define NORMAL_RESOLUTION (1.0/(NORMAL_DENOMINATOR))

#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

#define IsPC() true
#define IsConsole() false
#define BITS_PER_INT 32

#define CHECK_VALID(_v) 0
#define Assert(_exp) ((void)0)
#define FORCEINLINE	__forceinline
typedef Vec3 Vector;
typedef Vector QAngle;

typedef unsigned int uint;
typedef unsigned char uint8;
typedef signed char int8;

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

// intp is an integer that can accomodate a pointer
// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
typedef intptr_t intp;
typedef uintptr_t uintp;
//-----------------------------------------------------------------------------
// Purpose: Standard functions for handling endian-ness
//-----------------------------------------------------------------------------

//-------------------------------------
// Basic swaps
//-------------------------------------

template <typename T>
inline T WordSwapC(T w)
{
    uint16 temp;

    temp = ((*((uint16*)&w) & 0xff00) >> 8);
    temp |= ((*((uint16*)&w) & 0x00ff) << 8);

    return *((T*)&temp);
}

template <typename T>
inline T DWordSwapC(T dw)
{
    uint32 temp;

    temp = *((uint32*)&dw) >> 24;
    temp |= ((*((uint32*)&dw) & 0x00FF0000) >> 8);
    temp |= ((*((uint32*)&dw) & 0x0000FF00) << 8);
    temp |= ((*((uint32*)&dw) & 0x000000FF) << 24);

    return *((T*)&temp);
}

template <typename T>
inline T QWordSwapC(T dw)
{
    // Assert sizes passed to this are already correct, otherwise
    // the cast to uint64 * below is unsafe and may have wrong results 
    // or even crash.
    PLAT_COMPILE_TIME_ASSERT(sizeof(dw) == sizeof(uint64));

    uint64 temp;

    temp = *((uint64*)&dw) >> 56;
    temp |= ((*((uint64*)&dw) & 0x00FF000000000000ull) >> 40);
    temp |= ((*((uint64*)&dw) & 0x0000FF0000000000ull) >> 24);
    temp |= ((*((uint64*)&dw) & 0x000000FF00000000ull) >> 8);
    temp |= ((*((uint64*)&dw) & 0x00000000FF000000ull) << 8);
    temp |= ((*((uint64*)&dw) & 0x0000000000FF0000ull) << 24);
    temp |= ((*((uint64*)&dw) & 0x000000000000FF00ull) << 40);
    temp |= ((*((uint64*)&dw) & 0x00000000000000FFull) << 56);

    return *((T*)&temp);
}

//-------------------------------------
// Fast swaps
//-------------------------------------

#if defined( _MSC_VER ) && !defined( PLATFORM_WINDOWS_PC64 )

#define WordSwap  WordSwapAsm
#define DWordSwap DWordSwapAsm

#pragma warning(push)
#pragma warning (disable:4035) // no return value

template <typename T>
inline T WordSwapAsm(T w)
{
    __asm
    {
        mov ax, w
        xchg al, ah
    }
}

template <typename T>
inline T DWordSwapAsm(T dw)
{
    __asm
    {
        mov eax, dw
        bswap eax
    }
}

#pragma warning(pop)

#endif

#define QWordSwap QWordSwapC

// If a swapped float passes through the fpu, the bytes may get changed.
// Prevent this by swapping floats as DWORDs.
#define SafeSwapFloat( pOut, pIn )	(*((uint*)pOut) = DWordSwap( *((uint*)pIn) ))

#define BigShort( val )				WordSwap( val )
#define BigWord( val )				WordSwap( val )
#define BigLong( val )				DWordSwap( val )
#define BigDWord( val )				DWordSwap( val )
#define LittleShort( val )			( val )
#define LittleWord( val )			( val )
#define LittleLong( val )			( val )
#define LittleDWord( val )			( val )
#define SwapShort( val )			BigShort( val )
#define SwapWord( val )				BigWord( val )
#define SwapLong( val )				BigLong( val )
#define SwapDWord( val )			BigDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )


inline uint32 LoadLittleDWord(uint32* base, unsigned int dwordIndex)
{
    return LittleDWord(base[dwordIndex]);
}

inline void StoreLittleDWord(uint32* base, unsigned int dwordIndex, uint32 dword)
{
    base[dwordIndex] = LittleDWord(dword);
}

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


//-----------------------------------------------------------------------------
// Helpers.
//-----------------------------------------------------------------------------

inline int BitByte(int bits)
{
    // return PAD_NUMBER( bits, 8 ) >> 3;
    return (bits + 7) >> 3;
}

//-----------------------------------------------------------------------------
enum EBitCoordType
{
    kCW_None,
    kCW_LowPrecision,
    kCW_Integral
};

//-----------------------------------------------------------------------------
// Used for unserialization
//-----------------------------------------------------------------------------

class old_bf_read
{
public:
    old_bf_read();

    // nMaxBits can be used as the number of bits in the buffer. 
    // It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
    old_bf_read(const void* pData, int nBytes, int nBits = -1);
    old_bf_read(const char* pDebugName, const void* pData, int nBytes, int nBits = -1);

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
    const char* GetDebugName();
    void			SetDebugName(const char* pName);


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

    FORCEINLINE int TotalBytesAvailable(void) const
    {
        return m_nDataBytes;
    }

    // Read a list of bits in..
    void            ReadBits(void* pOut, int nBits);

    float			ReadBitAngle(int numbits);

    unsigned int	ReadUBitLong(int numbits);
    unsigned int	PeekUBitLong(int numbits);
    int				ReadSBitLong(int numbits);

    // reads an unsigned integer with variable bit length
    unsigned int	ReadUBitVar();

    // You can read signed or unsigned data with this, just cast to 
    // a signed int if necessary.
    unsigned int	ReadBitLong(int numbits, bool bSigned);

    float			ReadBitCoord();
    float			ReadBitCoordMP(EBitCoordType coordType);
    float 			ReadBitCellCoord(int bits, EBitCoordType coordType);
    float			ReadBitFloat();
    float			ReadBitNormal();
    void			ReadBitVec3Coord(Vector& fa);
    void			ReadBitVec3Normal(Vector& fa);
    void			ReadBitAngles(QAngle& fa);


    // Byte functions (these still read data in bit-by-bit).
public:

    int				ReadChar();
    int				ReadByte();
    int				ReadShort();
    int				ReadWord();
    long			ReadLong();
    int64			ReadLongLong();
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
    bool			ReadWString(wchar_t* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL);

    // Reads a string and allocates memory for it. If the string in the buffer
    // is > 2048 bytes, then pOverflow is set to true (if it's not NULL).
    char* ReadAndAllocateString(bool* pOverflow = 0);

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
    inline void		SetOverflowFlag();


public:

    // The current buffer.
    const unsigned char* m_pData;
    int						m_nDataBytes;
    int						m_nDataBits;

    // Where we are in the buffer.
    int				m_iCurBit;


private:
    // used by varbit reads internally
    inline int CountRunOfZeros();

    // Errors?
    bool			m_bOverflow;

    // For debugging..
    bool			m_bAssertOnOverflow;

    const char* m_pDebugName;
};

//-----------------------------------------------------------------------------
// Inlines.
//-----------------------------------------------------------------------------

inline int old_bf_read::GetNumBytesRead()
{
    return BitByte(m_iCurBit);
}

inline int old_bf_read::GetNumBitsLeft()
{
    return m_nDataBits - m_iCurBit;
}

inline int old_bf_read::GetNumBytesLeft()
{
    return GetNumBitsLeft() >> 3;
}

inline int old_bf_read::GetNumBitsRead() const
{
    return m_iCurBit;
}

inline void old_bf_read::SetOverflowFlag()
{
    if (m_bAssertOnOverflow)
    {
        Assert(false);
    }

    m_bOverflow = true;
}

inline bool old_bf_read::Seek(int iBit)
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
inline bool	old_bf_read::SeekRelative(int iBitDelta)
{
    return Seek(m_iCurBit + iBitDelta);
}

inline bool old_bf_read::CheckForOverflow(int nBits)
{
    if (m_iCurBit + nBits > m_nDataBits)
    {
        SetOverflowFlag();
        CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
    }

    return m_bOverflow;
}

inline int old_bf_read::ReadOneBitNoCheck()
{
    int value = m_pData[m_iCurBit >> 3] & (1 << (m_iCurBit & 7));
    ++m_iCurBit;
    return !!value;
}

inline int old_bf_read::ReadOneBit()
{
    return (!CheckForOverflow(1)) ? ReadOneBitNoCheck() : 0;
}

inline float old_bf_read::ReadBitFloat()
{
    long val;

    Assert(sizeof(float) == sizeof(long));
    Assert(sizeof(float) == 4);

    if (CheckForOverflow(32))
        return 0.0f;

    int bit = m_iCurBit & 0x7;
    int byte = m_iCurBit >> 3;
    val = m_pData[byte] >> bit;
    val |= ((int)m_pData[byte + 1]) << (8 - bit);
    val |= ((int)m_pData[byte + 2]) << (16 - bit);
    val |= ((int)m_pData[byte + 3]) << (24 - bit);
    if (bit != 0)
        val |= ((int)m_pData[byte + 4]) << (32 - bit);
    m_iCurBit += 32;
    return *((float*)&val);
}


inline unsigned int old_bf_read::ReadUBitLong(int numbits)
{
    extern unsigned long g_ExtraMasks[32];

    if ((m_iCurBit + numbits) > m_nDataBits)
    {
        m_iCurBit = m_nDataBits;
        SetOverflowFlag();
        return 0;
    }

    Assert(numbits > 0 && numbits <= 32);

    // Read the current dword.
    int idword1 = m_iCurBit >> 5;
    unsigned int dword1 = LoadLittleDWord((uint32*)m_pData, idword1);

    dword1 >>= (m_iCurBit & 31); // Get the bits we're interested in.

    m_iCurBit += numbits;
    unsigned int ret = dword1;

    // Does it span this dword?
    if ((m_iCurBit - 1) >> 5 == idword1)
    {
        if (numbits != 32)
            ret &= g_ExtraMasks[numbits];
    }
    else
    {
        int nExtraBits = m_iCurBit & 31;
        unsigned int dword2 = LoadLittleDWord((uint32*)m_pData, idword1 + 1);

        dword2 &= g_ExtraMasks[nExtraBits];

        // No need to mask since we hit the end of the dword.
        // Shift the second dword's part into the high bits.
        ret |= (dword2 << (numbits - nExtraBits));
    }

    return ret;
}


class CBitBuffer
{
public:
    char const* m_pDebugName;
    bool m_bOverflow;
    int m_nDataBits;
    size_t m_nDataBytes;

    void SetDebugName(char const* pName)
    {
        m_pDebugName = pName;
    }

    CBitBuffer(void)
    {
        m_bOverflow = false;
        m_pDebugName = NULL;
        m_nDataBits = -1;
        m_nDataBytes = 0;
    }

    FORCEINLINE void SetOverflowFlag(void)
    {
        m_bOverflow = true;
    }

    FORCEINLINE bool IsOverflowed(void) const
    {
        return m_bOverflow;
    }

    static const uint32 s_nMaskTable[33];							// 0 1 3 7 15 ..

};

class CBitWrite : public CBitBuffer
{
    uint32 m_nOutBufWord;
    int m_nOutBitsAvail;
    uint32* m_pDataOut;
    uint32* m_pBufferEnd;
    uint32* m_pData;
    bool m_bFlushed;

public:
    void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nMaxBits = -1);


    CBitWrite(void* pData, int nBytes, int nBits = -1)
    {
        m_bFlushed = false;
        StartWriting(pData, nBytes, 0, nBits);
    }

    CBitWrite(const char* pDebugName, void* pData, int nBytes, int nBits = -1)
    {
        m_bFlushed = false;
        SetDebugName(pDebugName);
        StartWriting(pData, nBytes, 0, nBits);
    }

    CBitWrite(void)
    {
        m_bFlushed = false;
    }

    ~CBitWrite(void)
    {
        TempFlush();
        Assert((!m_pData) || m_bFlushed);
    }
    FORCEINLINE int GetNumBitsLeft(void) const
    {
        return m_nOutBitsAvail + (32 * (m_pBufferEnd - m_pDataOut - 1));
    }

    FORCEINLINE void Reset(void)
    {
        m_bOverflow = false;
        m_nOutBitsAvail = 32;
        m_pDataOut = m_pData;
        m_nOutBufWord = 0;

    }

    FORCEINLINE void TempFlush(void)
    {
        // someone wants to know how much data we have written, or the pointer to it, so we'd better make
        // sure we write our data
        if (m_nOutBitsAvail != 32)
        {
            if (m_pDataOut == m_pBufferEnd)
            {
                SetOverflowFlag();
            }
            else
            {
                StoreLittleDWord(m_pDataOut, 0, (LoadLittleDWord(m_pDataOut, 0) & ~s_nMaskTable[32 - m_nOutBitsAvail]) | m_nOutBufWord);
            }
        }
        m_bFlushed = true;
    }

    FORCEINLINE unsigned char* GetBasePointer()
    {
        TempFlush();
        return reinterpret_cast<unsigned char*>(m_pData);
    }

    FORCEINLINE unsigned char* GetData()
    {
        return GetBasePointer();
    }

    FORCEINLINE void Finish();
    FORCEINLINE void Flush();
    FORCEINLINE void FlushNoCheck();
    FORCEINLINE void WriteOneBit(int nValue);
    FORCEINLINE void WriteOneBitNoCheck(int nValue);
    FORCEINLINE void WriteUBitLong(unsigned int data, int numbits, bool bCheckRange = true);
    FORCEINLINE void WriteSBitLong(int data, int numbits);
    FORCEINLINE void WriteUBitVar(unsigned int data);
    FORCEINLINE void WriteBitFloat(float flValue);
    FORCEINLINE void WriteFloat(float flValue);
    bool WriteBits(const void* pInData, int nBits);
    void WriteBytes(const void* pBuf, int nBytes);
    void SeekToBit(int nSeekPos);

    FORCEINLINE int GetNumBitsWritten(void) const
    {
        return (32 - m_nOutBitsAvail) + (32 * (m_pDataOut - m_pData));
    }

    FORCEINLINE int GetNumBytesWritten(void) const
    {
        return (GetNumBitsWritten() + 7) >> 3;
    }


    FORCEINLINE void WriteLong(long val)
    {
        WriteSBitLong(val, 32);
    }



    FORCEINLINE void WriteChar(int val)
    {
        WriteSBitLong(val, sizeof(char) << 3);
    }

    FORCEINLINE void WriteByte(int val)
    {
        WriteUBitLong(val, sizeof(unsigned char) << 3, false);
    }

    FORCEINLINE void WriteShort(int val)
    {
        WriteSBitLong(val, sizeof(short) << 3);
    }

    FORCEINLINE void WriteWord(int val)
    {
        WriteUBitLong(val, sizeof(unsigned short) << 3);
    }

    bool WriteString(const char* pStr);
    bool WriteString(const wchar_t* pStr);

    void WriteLongLong(int64 val);

    void WriteBitAngle(float fAngle, int numbits);
    void WriteBitCoord(const float f);
    void WriteBitCoordMP(const float f, EBitCoordType coordType);
    void WriteBitCellCoord(const float f, int bits, EBitCoordType coordType);
    void WriteBitVec3Coord(const Vector& fa);
    void WriteBitNormal(float f);
    void WriteBitVec3Normal(const Vector& fa);
    void WriteBitAngles(const QAngle& fa);

    // Copy the bits straight out of pIn. This seeks pIn forward by nBits.
    // Returns an error if this buffer or the read buffer overflows.
    bool WriteBitsFromBuffer(class bf_read* pIn, int nBits);

};

void CBitWrite::Finish(void)
{
    if (m_nOutBitsAvail != 32)
    {
        if (m_pDataOut == m_pBufferEnd)
        {
            SetOverflowFlag();
        }
        StoreLittleDWord(m_pDataOut, 0, m_nOutBufWord);
    }
}

void CBitWrite::FlushNoCheck(void)
{
    StoreLittleDWord(m_pDataOut++, 0, m_nOutBufWord);
    m_nOutBitsAvail = 32;
    m_nOutBufWord = 0;										// ugh - I need this because of 32 bit writes. a<<=32 is a nop

}
void CBitWrite::Flush(void)
{
    if (m_pDataOut == m_pBufferEnd)
    {
        SetOverflowFlag();
    }
    else
    {
        StoreLittleDWord(m_pDataOut++, 0, m_nOutBufWord);
    }
    m_nOutBufWord = 0;										// ugh - I need this because of 32 bit writes. a<<=32 is a nop
    m_nOutBitsAvail = 32;

}
void CBitWrite::WriteOneBitNoCheck(int nValue)
{
    m_nOutBufWord |= (nValue & 1) << (32 - m_nOutBitsAvail);
    if (--m_nOutBitsAvail == 0)
    {
        FlushNoCheck();
    }
}

void CBitWrite::WriteOneBit(int nValue)
{
    m_nOutBufWord |= (nValue & 1) << (32 - m_nOutBitsAvail);
    if (--m_nOutBitsAvail == 0)
    {
        Flush();
    }
}

FORCEINLINE void CBitWrite::WriteUBitLong(unsigned int nData, int nNumBits, bool bCheckRange)
{

#ifdef _DEBUG
    // Make sure it doesn't overflow.
    if (bCheckRange && nNumBits < 32)
    {
        Assert(nData <= (unsigned long)(1 << nNumBits));
    }
    Assert(nNumBits >= 0 && nNumBits <= 32);
#endif
    if (nNumBits <= m_nOutBitsAvail)
    {
        if (bCheckRange)
            m_nOutBufWord |= (nData) << (32 - m_nOutBitsAvail);
        else
            m_nOutBufWord |= (nData & s_nMaskTable[nNumBits]) << (32 - m_nOutBitsAvail);
        m_nOutBitsAvail -= nNumBits;
        if (m_nOutBitsAvail == 0)
        {
            Flush();
        }
    }
    else
    {
        // split dwords case
        int nOverflowBits = (nNumBits - m_nOutBitsAvail);
        m_nOutBufWord |= (nData & s_nMaskTable[m_nOutBitsAvail]) << (32 - m_nOutBitsAvail);
        Flush();
        m_nOutBufWord = (nData >> (nNumBits - nOverflowBits));
        m_nOutBitsAvail = 32 - nOverflowBits;
    }
}

FORCEINLINE void CBitWrite::WriteSBitLong(int nData, int nNumBits)
{
    WriteUBitLong((uint32)nData, nNumBits, false);
}

FORCEINLINE void CBitWrite::WriteUBitVar(unsigned int n)
{
    if (n < 16)
        WriteUBitLong(n, 6);
    else
        if (n < 256)
            WriteUBitLong((n & 15) | 16 | ((n & (128 | 64 | 32 | 16)) << 2), 10);
        else
            if (n < 4096)
                WriteUBitLong((n & 15) | 32 | ((n & (2048 | 1024 | 512 | 256 | 128 | 64 | 32 | 16)) << 2), 14);
            else
            {
                WriteUBitLong((n & 15) | 48, 6);
                WriteUBitLong((n >> 4), 32 - 4);
            }
}

FORCEINLINE void CBitWrite::WriteBitFloat(float flValue)
{
    WriteUBitLong(*((uint32*)&flValue), 32);
}

FORCEINLINE void CBitWrite::WriteFloat(float flValue)
{
    // Pre-swap the float, since WriteBits writes raw data
    LittleFloat(&flValue, &flValue);
    WriteUBitLong(*((uint32*)&flValue), 32);
}

class CBitRead : public CBitBuffer
{
    uint32 m_nInBufWord;
    int m_nBitsAvail;
    uint32 const* m_pDataIn;
    uint32 const* m_pBufferEnd;
    uint32 const* m_pData;

public:
    CBitRead(const void* pData, int nBytes, int nBits = -1)
    {
        StartReading(pData, nBytes, 0, nBits);
    }

    CBitRead(const char* pDebugName, const void* pData, int nBytes, int nBits = -1)
    {
        SetDebugName(pDebugName);
        StartReading(pData, nBytes, 0, nBits);
    }

    CBitRead(void) : CBitBuffer()
    {
    }

    FORCEINLINE int Tell(void) const
    {
        return GetNumBitsRead();
    }

    FORCEINLINE size_t TotalBytesAvailable(void) const
    {
        return m_nDataBytes;
    }

    FORCEINLINE int GetNumBitsLeft() const
    {
        return m_nDataBits - Tell();
    }

    FORCEINLINE int GetNumBytesLeft() const
    {
        return GetNumBitsLeft() >> 3;
    }

    bool Seek(int nPosition);

    FORCEINLINE bool SeekRelative(int nOffset)
    {
        return Seek(GetNumBitsRead() + nOffset);
    }

    FORCEINLINE unsigned char const* GetBasePointer()
    {
        return reinterpret_cast<unsigned char const*>(m_pData);
    }

    void StartReading(const void* pData, int nBytes, int iStartBit = 0, int nBits = -1);

    FORCEINLINE int GetNumBitsRead(void) const;

    FORCEINLINE void GrabNextDWord(bool bOverFlowImmediately = false);
    FORCEINLINE void FetchNext(void);
    FORCEINLINE unsigned int ReadUBitLong(int numbits);
    FORCEINLINE int ReadSBitLong(int numbits);
    FORCEINLINE unsigned int ReadUBitVar(void);
    FORCEINLINE unsigned int PeekUBitLong(int numbits);
    FORCEINLINE float ReadBitFloat(void);
    float ReadBitCoord();
    float ReadBitCoordMP(EBitCoordType coordType);
    float ReadBitCellCoord(int bits, EBitCoordType coordType);
    float ReadBitNormal();
    void ReadBitVec3Coord(Vector& fa);
    void ReadBitVec3Normal(Vector& fa);
    void ReadBitAngles(QAngle& fa);
    bool ReadBytes(void* pOut, int nBytes);
    float ReadBitAngle(int numbits);

    // Returns 0 or 1.
    FORCEINLINE int	ReadOneBit(void);
    FORCEINLINE int ReadLong(void);
    FORCEINLINE int ReadChar(void);
    FORCEINLINE int ReadByte(void);
    FORCEINLINE int ReadShort(void);
    FORCEINLINE int ReadWord(void);
    FORCEINLINE float ReadFloat(void);
    void ReadBits(void* pOut, int nBits);

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
    // pOutN<umChars is set to the number of characters left in pStr when the routine is 
    // complete (this will never exceed bufLen-1).
    //
    bool ReadString(char* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL);
    bool ReadWString(wchar_t* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL);
    char* ReadAndAllocateString(bool* pOverflow = 0);

    int64 ReadLongLong(void);

};


FORCEINLINE int CBitRead::GetNumBitsRead(void) const
{
    if (!m_pData)									   // pesky null ptr bitbufs. these happen.
        return 0;
    int nCurOfs = (32 - m_nBitsAvail) + (8 * sizeof(m_pData[0]) * (m_pDataIn - m_pData - 1));
    int nAdjust = 8 * (m_nDataBytes & 3);
    return MIN(nCurOfs + nAdjust, m_nDataBits);

}

FORCEINLINE void CBitRead::GrabNextDWord(bool bOverFlowImmediately)
{
    if (m_pDataIn == m_pBufferEnd)
    {
        m_nBitsAvail = 1;									// so that next read will run out of words
        m_nInBufWord = 0;
        m_pDataIn++;										// so seek count increments like old
        if (bOverFlowImmediately)
            SetOverflowFlag();
    }
    else
        if (m_pDataIn > m_pBufferEnd)
        {
            SetOverflowFlag();
            m_nInBufWord = 0;
        }
        else
        {
            Assert(reinterpret_cast<int>(m_pDataIn) + 3 < reinterpret_cast<int>(m_pBufferEnd));
            m_nInBufWord = LittleDWord(*(m_pDataIn++));
        }
}

FORCEINLINE void CBitRead::FetchNext(void)
{
    m_nBitsAvail = 32;
    GrabNextDWord(false);
}

int CBitRead::ReadOneBit(void)
{
    int nRet = m_nInBufWord & 1;
    if (--m_nBitsAvail == 0)
    {
        FetchNext();
    }
    else
        m_nInBufWord >>= 1;
    return nRet;
}


unsigned int CBitRead::ReadUBitLong(int numbits)
{
    if (m_nBitsAvail >= numbits)
    {
        unsigned int nRet = m_nInBufWord & s_nMaskTable[numbits];
        m_nBitsAvail -= numbits;
        if (m_nBitsAvail)
        {
            m_nInBufWord >>= numbits;
        }
        else
        {
            FetchNext();
        }
        return nRet;
    }
    else
    {
        // need to merge words
        unsigned int nRet = m_nInBufWord;
        numbits -= m_nBitsAvail;
        GrabNextDWord(true);
        if (m_bOverflow)
            return 0;
        nRet |= ((m_nInBufWord & s_nMaskTable[numbits]) << m_nBitsAvail);
        m_nBitsAvail = 32 - numbits;
        m_nInBufWord >>= numbits;
        return nRet;
    }
}

FORCEINLINE unsigned int CBitRead::PeekUBitLong(int numbits)
{
    int nSaveBA = m_nBitsAvail;
    int nSaveW = m_nInBufWord;
    uint32 const* pSaveP = m_pDataIn;
    unsigned int nRet = ReadUBitLong(numbits);
    m_nBitsAvail = nSaveBA;
    m_nInBufWord = nSaveW;
    m_pDataIn = pSaveP;
    return nRet;
}

FORCEINLINE int CBitRead::ReadSBitLong(int numbits)
{
    int nRet = ReadUBitLong(numbits);
    // sign extend
    return (nRet << (32 - numbits)) >> (32 - numbits);
}

FORCEINLINE int CBitRead::ReadLong(void)
{
    return (int)ReadUBitLong(sizeof(long) << 3);
}

FORCEINLINE float CBitRead::ReadFloat(void)
{
    uint32 nUval = ReadUBitLong(sizeof(long) << 3);
    return *((float*)&nUval);
}

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4715)								// disable warning on not all cases
// returning a value. throwing default:
// in measurably reduces perf in bit
// packing benchmark
#endif
FORCEINLINE unsigned int CBitRead::ReadUBitVar(void)
{
    unsigned int ret = ReadUBitLong(6);
    switch (ret & (16 | 32))
    {
    case 16:
        ret = (ret & 15) | (ReadUBitLong(4) << 4);
        Assert(ret >= 16);
        break;

    case 32:
        ret = (ret & 15) | (ReadUBitLong(8) << 4);
        Assert(ret >= 256);
        break;
    case 48:
        ret = (ret & 15) | (ReadUBitLong(32 - 4) << 4);
        Assert(ret >= 4096);
        break;
    }
    return ret;
}
#ifdef _WIN32
#pragma warning(pop)
#endif

FORCEINLINE float CBitRead::ReadBitFloat(void)
{
    uint32 nvalue = ReadUBitLong(32);
    return *((float*)&nvalue);
}

int CBitRead::ReadChar(void)
{
    return ReadSBitLong(sizeof(char) << 3);
}

int CBitRead::ReadByte(void)
{
    return ReadUBitLong(sizeof(unsigned char) << 3);
}

int CBitRead::ReadShort(void)
{
    return ReadSBitLong(sizeof(short) << 3);
}

int CBitRead::ReadWord(void)
{
    return ReadUBitLong(sizeof(unsigned short) << 3);
}

#define WRAP_READ( bc ) 																									  \
    class bf_read : public bc																									  \
    {																															  \
    public:																														  \
        FORCEINLINE bf_read( void ) : bc(  )																								  \
    	{																														  \
    	}																														  \
    																															  \
    	FORCEINLINE bf_read( const void *pData, int nBytes, int nBits = -1 ) : bc( pData, nBytes, nBits )									  \
    	{																														  \
    	}																														  \
    																															  \
    	FORCEINLINE bf_read( const char *pDebugName, const void *pData, int nBytes, int nBits = -1 ) : bc( pDebugName, pData, nBytes, nBits ) \
    	{																														  \
    	}																														  \
    };

#if 0
#define DELEGATE0( t, m )	t m()					\
    {												\
    		Check(); \
    	t nOld = old1.m();						\
    	t nNew = new1.m();						\
    	Assert( nOld == nNew );						\
    	Check();									\
    	return nOld;								\
    }
#define DELEGATE1( t, m, t1 )	t m( t1 x)					\
    {												\
    		Check(); \
    	t nOld = old1.m( x);						\
    	t nNew = new1.m( x );						\
    	Assert( nOld == nNew );						\
    	Check();									\
    	return nOld;								\
    }
#define DELEGATE0I( m )	DELEGATE0( int, m )
#define DELEGATE0LL( m ) DELEGATE0( int64, m )
class bf_read
{
    old_bf_read old1;
    CBitRead new1;
    void Check(void) const
    {
        int n = new1.GetNumBitsRead();
        int o = old1.GetNumBitsRead();
        Assert(n == o);
        Assert(old1.IsOverflowed() == new1.IsOverflowed());
    }
public:
    FORCEINLINE bf_read(void) : old1(), new1()
    {
    }
    FORCEINLINE bf_read(const void* pData, int nBytes, int nBits = -1) : old1(pData, nBytes, nBits), new1(pData, nBytes, nBits)
    {
    }
    FORCEINLINE bf_read(const char* pDebugName, const void* pData, int nBytes, int nBits = -1) : old1(pDebugName, pData, nBytes, nBits), new1(pDebugName, pData, nBytes, nBits)
    {
    }
    FORCEINLINE bool IsOverflowed(void) const
    {
        bool bOld = old1.IsOverflowed();
        bool bNew = new1.IsOverflowed();
        Assert(bOld == bNew);
        Check();
        return bOld;
    }
    void ReadBits(void* pOut, int nBits)
    {
        old1.ReadBits(pOut, nBits);
        void* mem = stackalloc(1 + (nBits / 8));
        new1.ReadBits(mem, nBits);
        Assert(memcmp(mem, pOut, nBits / 8) == 0);
    }
    bool ReadBytes(void* pOut, int nBytes)
    {
        ReadBits(pOut, nBytes << 3);
        return !IsOverflowed();
    }
    unsigned int ReadUBitLong(int numbits)
    {
        unsigned int nOld = old1.ReadUBitLong(numbits);
        unsigned int nNew = new1.ReadUBitLong(numbits);
        Assert(nOld == nNew);
        Check();
        return nOld;
    }
    unsigned const char* GetBasePointer()
    {
        Assert(old1.GetBasePointer() == new1.GetBasePointer());
        Check();
        return old1.GetBasePointer();
    }
    void SetDebugName(const char* pDebugName)
    {
        old1.SetDebugName(pDebugName);
        new1.SetDebugName(pDebugName);
        Check();
    }
    void StartReading(const void* pData, int nBytes, int iStartBit = 0, int nBits = -1)
    {
        old1.StartReading(pData, nBytes, iStartBit, nBits);
        new1.StartReading(pData, nBytes, iStartBit, nBits);
        Check();
    }
    void SetAssertOnOverflow(bool bAssert)
    {
        old1.SetAssertOnOverflow(bAssert);
        //		new1.SetAssertOnOverflow( bAssert );
        Check();
    }
    DELEGATE0I(ReadOneBit);
    DELEGATE0I(ReadByte);
    DELEGATE0I(ReadWord);
    DELEGATE0I(ReadLong);
    DELEGATE0I(GetNumBytesLeft);
    DELEGATE0I(ReadShort);
    DELEGATE1(int, PeekUBitLong, int);
    DELEGATE0I(ReadChar);
    DELEGATE0I(GetNumBitsRead);
    DELEGATE0LL(ReadLongLong);
    DELEGATE0(float, ReadFloat);
    DELEGATE0(unsigned int, ReadUBitVar);
    DELEGATE0(float, ReadBitCoord);
    DELEGATE2(float, ReadBitCoordMP, bool, bool);
    DELEGATE0(float, ReadBitFloat);
    DELEGATE0(float, ReadBitNormal);
    DELEGATE1(bool, Seek, int);
    DELEGATE1(float, ReadBitAngle, int);
    DELEGATE1(bool, SeekRelative, int);
    DELEGATE0I(GetNumBitsLeft);
    DELEGATE0I(TotalBytesAvailable);
    void SetOverflowFlag()
    {
        old1.SetOverflowFlag();
        new1.SetOverflowFlag();
        Check();
    }
    bool ReadString(char* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL)
    {
        Check();
        int oldn, newn;
        bool bOld = old1.ReadString(pStr, bufLen, bLine, &oldn);
        bool bNew = new1.ReadString(pStr, bufLen, bLine, &newn);
        Assert(bOld == bNew);
        Assert(oldn == newn);
        if (pOutNumChars)
            *pOutNumChars = oldn;
        Check();
        return bOld;
    }
    bool ReadWString(wchar_t* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL)
    {
        Check();
        int oldn, newn;
        bool bOld = old1.ReadWString(pStr, bufLen, bLine, &oldn);
        bool bNew = new1.ReadWString(pStr, bufLen, bLine, &newn);
        Assert(bOld == bNew);
        Assert(oldn == newn);
        if (pOutNumChars)
            *pOutNumChars = oldn;
        Check();
        return bOld;
    }
    void ReadBitVec3Coord(Vector& fa)
    {
        Check();
        old1.ReadBitVec3Coord(fa);
        Vector test;
        new1.ReadBitVec3Coord(test);
        Assert(VectorsAreEqual(fa, test));
        Check();
    }
    void ReadBitVec3Normal(Vector& fa)
    {
        Check();
        old1.ReadBitVec3Coord(fa);
        Vector test;
        new1.ReadBitVec3Coord(test);
        Assert(VectorsAreEqual(fa, test));
        Check();
    }

    char* ReadAndAllocateString(bool* pOverflow = NULL)
    {
        Check();
        bool bold, bnew;
        char* pold = old1.ReadAndAllocateString(&bold);
        char* pnew = new1.ReadAndAllocateString(&bnew);
        Assert(bold == bnew);
        Assert(strcmp(pold, pnew) == 0);
        delete[] pnew;
        Check();
        if (pOverflow)
            *pOverflow = bold;
        return pold;
    }
    DELEGATE1(int, ReadSBitLong, int);
};
#endif


WRAP_READ(CBitRead);

#endif