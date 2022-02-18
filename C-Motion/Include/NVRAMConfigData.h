/*
	NVRAMData.h: interface for the NVRAMSegment classes.


NVRAMSegment, NVRAMData
    |
	- NVRAMSegmentProductData
	- NVRAMSegmentCommands

//////////////////////////////////////////////////////////////////////
*/

#define NVRAM_SIZE_BYTES	8192
#define NVRAM_SIZE_WORDS	NVRAM_SIZE_BYTES / 2


enum NVRAMError
{
	NVRAMError_None,
	NVRAMError_Erased,
	NVRAMError_SegmentNotFound,
	NVRAMError_ProductDataNotFound,
	NVRAMError_Checksum,
	NVRAMError_InvalidHeader,
	NVRAMError_InvalidSegmentType,
	NVRAMError_InvalidDataCount,
	NVRAMError_ZeroDataCount,
	NVRAMError_UnexpectedDataType,
};

/*
0x2000 0000 Customer NVRAM
0x2000 03FF
0x2100 0000 Customer OTP
0x2100 03FF
0x2200 0000 Manufacturer NVRAM
0x2200 0FFF
*/

enum NVRAMStart
{													
	NVRAMStart_UserNVRAM			= 0x20000000,	
	NVRAMStart_UserOTP				= 0x21000000,	
	NVRAMStart_ManufacturerNVRAM	= 0x22000000,	
};

enum NVRAMSize
{									// size in words
	NVRAMSize_UserNVRAM				= 0x0400,
	NVRAMSize_UserOTP				= 0x0400,
	NVRAMSize_ManufacturerNVRAM		= 0x1000
};

enum NVRAMSegmentType : char
{
	NVRAMSegmentType_Unknown					= 0x00,
	NVRAMSegmentType_ProductData				= 0x10,
	NVRAMSegmentType_InitializationCommands		= 0x12
};

// NVRAMData is the entire block of data containing multiple resources
class NVRAMData
{
private:
	short NVRAMbuffer[NVRAM_SIZE_WORDS];
	int WordCount;			// raw data count in words.


protected:
	short Version;
	int  BufIndex;			// current buffer index in words;
	char ComputeChecksum(int start, int wordcount);
	void CopyBufferFrom(NVRAMData &ExistingConfigData);
	bool IsEndOfData(int index);
	int  GotoEndOfData();

	int  GetBufIndex();
	void SetBufIndex(int index);
	int  GetByteCount();
	void AddBytes(void* s8Data, int bytecount);
	void AddString(CString str);
	void Addf32(float fData);
	void Adds16(short s16Data);
	void Adds32(long s32Data);
	void Sets32(int index, long data);
	void Sets16(int index, short data);

	char* GetBytes(int index);
	void  GetString(int index, int count, CString& str);
	float Getf32(int index);
	short Gets16(int index);
	long  Gets32(int index);

public:
	NVRAMData(int version);
	~NVRAMData(void);

//	NVRAMData& operator= (const NVRAMData& );

	// Set the NVRAM buffer in 16-bit words that was read from NVRAM.
	void CopyNVRAMDataBufferForRead(CWordArray& data);
	// Get the NVRAM buffer to be written to NVRAM.
	void GetNVRAMDataForWrite(CWordArray& data);
	static const char* GetErrorString(NVRAMError error);
	void SetHeaderVersion(short version);
};

// NVRAMSegment is a subsection of NVRAMData containing a data resource
class NVRAMSegment : public NVRAMData 
{
public:
	NVRAMSegment(int version, int identifier);
	~NVRAMSegment(void);

private:
	BOOL m_bValid;

protected:
	int  HeaderWords;		// Size of header in words. 
	int  Identifier;		// optional identifier used for identifing segments of the same type
	int  Checksum;

public:
	void CopyFrom(NVRAMSegment& ExistingSegment);

	int HeaderStart;	// word position in NVRAM data where the resource header starts (checksum byte and resource type byte).
	int DataIndex;		// word position in NVRAM data where the resource data starts.
	int DataCount;		// length of resource data in words.
	NVRAMSegmentType SegmentType;

	BOOL IsValid();
	NVRAMError ProcessSegmentForReading();
	void PostProcessSegmentForWriting();
	void PreProcessSegmentForWriting();
	void AddSegmentData(CWordArray& data); //, NVRAMSegmentType SegmentType);
	NVRAMError GetSegmentData(CWordArray& data);
	int	GetChecksum();
	int	GetIdentifier();
};

// Specific Segment classes

class NVRAMSegmentCommands : public NVRAMSegment
{
public:
	NVRAMSegmentCommands(int version = 1, int identifier = 0);

};

class NVRAMSegmentProductData : public NVRAMSegment
{
private:

	enum NVRAMDataType
	{
		NVRAMDataType_widestring = 0, // unicode UTF-16 string format
		NVRAMDataType_string     = 1,
		NVRAMDataType_s16        = 2,
		NVRAMDataType_s32        = 3,
		NVRAMDataType_s64        = 4,
		NVRAMDataType_float      = 5,
		NVRAMDataType_words      = 8,
	};

	void SetHeaderFieldName(CString csName);
	void SetHeaderFieldLength(NVRAMDataType datatype, int wordcount);
	NVRAMError FindProductDataName(CString csName, int& pos);
		
public:
	NVRAMSegmentProductData(int version = 1, int identifier = 0);
	~NVRAMSegmentProductData(void);

	void AddField(CString csName, CString csData);
	void AddField(CString csName, float fData);
	void AddField(CString csName, short s16Data);
	void AddField(CString csName, long s32Data);

	NVRAMError GetField(CString csName, CString& csData);
	NVRAMError GetField(CString csName, float& fData);
	NVRAMError GetField(CString csName, short& s16Data);
	NVRAMError GetField(CString csName, long& s32Data);

	void SetLegCurrentScale(float fData);
	void SetBusCurrentScale(float fData);
	void SetBusVoltageScale(float fData);
	void SetPartNumber(CString csData);
	void SetSerialNumber(CString sn);
	void SetPartName(CString csData);
	void SetConfigName(CString csData);
	void SetConfigVersion(CString csData);
	void SetPartVersion(CString csData);
	void SetDate(CString csData);
	void SetWriteDate(CString csData);
	void SetFileDate(CString csData);
	void SetFileName(CString csData);
	void SetDescription(CString csData);

	NVRAMError GetLegCurrentScale(float& fData);
	NVRAMError GetBusCurrentScale(float& fData);
	NVRAMError GetBusVoltageScale(float& fData);
	NVRAMError GetI2tScale(float& fData);
	NVRAMError GetPartNumber(CString& csData);
	NVRAMError GetSerialNumber(CString& sn);
	NVRAMError GetPartName(CString& csData);
	NVRAMError GetPartVersion(CString& csData);
	NVRAMError GetConfigName(CString& csData);
	NVRAMError GetConfigVersion(CString& csData);
	NVRAMError GetDate(CString& csData);
	NVRAMError GetWriteDate(CString& csData);
	NVRAMError GetFileDate(CString& csData);
	NVRAMError GetFileName(CString& csData);
	NVRAMError GetDescription(CString& csData);
};

