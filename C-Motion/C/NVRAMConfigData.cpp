#include "stdafx.h"
#include "NVRAMConfigData.h"

/*
With a PMD NVRAM header of [0,0,0,0] a product data segment may be put anywhere after the four erased word (0xFFFF) sequence that terminates the initialization command sequence.
0000 PMD Header
0000
0000
0000
0000 User Header
0000
0000
0000
1062 SetDriveFaultParameter 1 2
0002
0001
FFFF End mark
FFFF
FFFF
FFFF

With a PMD NVRAM header of [0,0,0,1] a product data segment may be put anywhere after the four word user header.
0000 PMD Header
0000
0000
0001
0000 User Header
0000
0000
0000
9026 Product data header code (0x90) and checksum
0000 identifier (not used)
0000 reserved
000B data length in words (11)
0000 high word of data length field
		First field, Product Name "Atlas"
4E50 "PN" field name 
0000 zero pad field name
1003 string type, length 3 words
7441 "At"
616C "la"
0073 "s" zero padded 
		Second field, leg current scaling 1.0 mA/count
4C49 "IL" field name
0053 "S" field name, zero padded
4002 floating point, length 2 words
0000 low word of value 1.0
3F80 high word of value 1.0

FFFF End mark
FFFF
FFFF
FFFF
*/

NVRAMData::NVRAMData(int version)
{
	// initialize buffer to all 1's so that it could be appended to without erasing the entire sector.
	memset(NVRAMbuffer, 0xFF, NVRAM_SIZE_BYTES);
	BufIndex = 0;
	Version = version;
	// initialize PMD header (version 1)
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = Version;

	// user header
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = 0x0000;
	NVRAMbuffer[BufIndex++] = 0x0000;

	// position index past user header to where data will be added.
}

NVRAMData::~NVRAMData(void){}

void NVRAMData::CopyBufferFrom(NVRAMData &ExistingConfigData)
{
	// initialize buffer to all 1's so that it could be appended to without erasing the entire sector.
	memset(NVRAMbuffer, 0xFF, NVRAM_SIZE_BYTES);
	memcpy(NVRAMbuffer, ExistingConfigData.NVRAMbuffer, NVRAM_SIZE_BYTES);
	// position index past user header to where data will be added.
	// position index past user header to where data will be added.
	BufIndex = 8;
}

void NVRAMData::CopyNVRAMDataBufferForRead(CWordArray& data)
{
	int wordcount = data.GetSize();
	ASSERT (wordcount < NVRAM_SIZE_BYTES);
	for( int i=0; i<wordcount; i++ )
	{
		NVRAMbuffer[i] = data[i];
	}
	// position index past user header to where data will be added.
	BufIndex = 8;
}

void NVRAMData::GetNVRAMDataForWrite(CWordArray& data)
{
	int wordcount = GotoEndOfData();
	if (Version > 1)
	{
		while (wordcount % 4)
		{
			wordcount++;
		}
	}
	data.SetSize(wordcount);
	NVRAMbuffer[3] = Version;

	for( int i=0; i<wordcount; i++ )
	{
		data.SetAt(i, NVRAMbuffer[i]);
	}
}

int NVRAMData::GetBufIndex()
{
	return BufIndex;
}

void NVRAMData::SetBufIndex(int index)
{
	BufIndex = index;
}
/*
int NVRAMData::GetByteCount()
{
	return ByteCount;
}
/*
void NVRAMData::AssignNVRAMDataBufferForWrite(short* pdata, int wordcount)
{
	ASSERT(ByteCount <= NVRAM_SIZE_BYTES);
	NVRAMbuffer = pdata;
}

/*
  |    checksum    |      0x90     |
  |  data length in words (low)    |
  |  data length in words (high)   |
  |            data word 0         |
  |              ...               |
*/
bool NVRAMData::IsEndOfData(int index)
{
	ASSERT(index < NVRAM_SIZE_WORDS-4);
	if ((NVRAMbuffer[index+0] == (short)0xFFFF) &&
	    (NVRAMbuffer[index+1] == (short)0xFFFF) &&
	    (NVRAMbuffer[index+2] == (short)0xFFFF) &&
	    (NVRAMbuffer[index+3] == (short)0xFFFF))
		return true;
	return false;
}

int NVRAMData::GotoEndOfData()
{
	int index = 8;
	while (!IsEndOfData(index))
	{
		index++;
	}
	return index;
}

// Calculate the checksum on each byte in the buffer.
// start = word index location to start checksum calculation.
// wordcount = number of words to include in checksum calculation.
char NVRAMData::ComputeChecksum(int start, int wordcount)
{
	BYTE* pbuf = (BYTE*)&NVRAMbuffer[start];
	int ByteCount = wordcount*2;
	/* The checksum is the ones complement of an 8-bit, ones complement checksum with a seed of 0xAA. 
	The checksum is computed using all header words and all data words.
	If the checksum field is computed correctly then the checksum over all words in the Product Data 
	block will be 255 (0xFF).*/
	int sum = 0xAA; //seed
	for( int i=0; i<ByteCount; i++ )
	{
		sum += pbuf[i];
		sum = (0xFF & sum) + (sum >> 8); // add in the carry
	}

	return sum;
}

const char* NVRAMData::GetErrorString(NVRAMError error)
{
    return error == NVRAMError_None            ? "No error" :
    error == NVRAMError_Erased                 ? "Erased" :
    error == NVRAMError_SegmentNotFound        ? "Segment not found" :
    error == NVRAMError_ProductDataNotFound    ? "Product data not found" :
    error == NVRAMError_Checksum               ? "Checksum mismatch" :
    error == NVRAMError_InvalidHeader          ? "Invalid header" :
    error == NVRAMError_InvalidSegmentType     ? "Invalid segment type" : 
    error == NVRAMError_InvalidDataCount       ? "Invalid segment data count" : 
    error == NVRAMError_ZeroDataCount          ? "Empty segment" : 
    error == NVRAMError_UnexpectedDataType     ? "Unexpected data type" : 

	"Undefined error occurred";
}


void NVRAMData::AddBytes(void* data, int bytecount)
{
	int length = bytecount >> 1;

	ASSERT(bytecount <= NVRAM_SIZE_BYTES);
	if (bytecount % 2 == 1)
	{
		NVRAMbuffer[BufIndex + length] = 0; // set the last byte to 0 if there is an odd number of bytes
		length++;
	}
	memcpy_s(&NVRAMbuffer[BufIndex], NVRAM_SIZE_BYTES - BufIndex, data, bytecount);
	BufIndex += length;
}

void NVRAMData::AddString(CString str)
{
	int length = str.GetLength();

	for (int i=0; i<length; i++)
	{
		Adds16((WORD)str[i]);
	}
}

void NVRAMData::Addf32(float data)
{
	*(float*)&NVRAMbuffer[BufIndex] = data;
	BufIndex += 2;
}

void NVRAMData::Adds16(short data)
{
	*(short*)&NVRAMbuffer[BufIndex] = data;
	BufIndex += 1;
}

void NVRAMData::Adds32(long data)
{
	*(long*)&NVRAMbuffer[BufIndex] = data;
	BufIndex += 2;
}

void NVRAMData::Sets32(int index, long data)
{
	*(long*)&NVRAMbuffer[index] = data;
}

void NVRAMData::Sets16(int index, short data)
{
	*(short*)&NVRAMbuffer[index] = data;
}

float NVRAMData::Getf32(int index)
{
	float data = *(float*)&NVRAMbuffer[index];

	return data;
}

short NVRAMData::Gets16(int index)
{
	short data = *(short*)&NVRAMbuffer[index];

	return data;
}

long NVRAMData::Gets32(int index)
{
	long data = *(long*)&NVRAMbuffer[index];

	return data;
}

char* NVRAMData::GetBytes(int index)
{
	return (char*)&NVRAMbuffer[index];
}

void NVRAMData::GetString(int index, int wordcount, CString& str)
{
	for (int i=0; i<wordcount; i++)
	{
		str.AppendChar((char)NVRAMbuffer[index+i]);
	}
}

NVRAMSegment::NVRAMSegment(int version, int identifier) 
    : NVRAMData(version)
{
	SegmentType = NVRAMSegmentType_Unknown;
	// Position index past segment header to where data will be added.
	// Header is generated last.
	HeaderWords = 5; //Includes checksum, identifier, reserved, and data count words.
	Identifier = identifier;
	m_bValid = FALSE;
	PreProcessSegmentForWriting();
}

void NVRAMSegment::CopyFrom(NVRAMSegment& ExistingSegment)
{
	CopyBufferFrom(ExistingSegment);
	PreProcessSegmentForWriting();
}

BOOL NVRAMSegment::IsValid()
{
	return m_bValid;
}

/* Header format:
9026 Product data header code (0x90) and checksum
0000 identifier (user defined integer)
0000 reserved
000B data length in words (low word)
0000 data length (high word)
*/
void NVRAMSegment::PreProcessSegmentForWriting()
{
	HeaderStart = GotoEndOfData();

	//Radey: Because of the ECC checking in flash for the sION piccolo, each segment has to start on an offset from the beginning that is a multiple of 4. You should add 0 to 3 padding words of 0xFFFF in order to get the alignment right.
	if (Version > 1)
	{
		while (HeaderStart % 4)
		{
			HeaderStart++;
		}
	}
	// Position index past segment header to where data will be added.
	// Header is generated last.
	DataIndex = HeaderStart + 1; //(HeaderWords - 2); //Adds16 for identifier and reserved will increment BufIndex so subtract 2
	SetBufIndex(DataIndex);
	Adds16(Identifier); // identifier
	Adds16(0); // reserved (DataIndex will now be 5)
	Adds32(0); // datacount
	DataCount = 0;
}

NVRAMSegment::~NVRAMSegment(void){}

// Process the NVRAM data block to be downloaded
// by adding the checksum and total data count fields
void NVRAMSegment::PostProcessSegmentForWriting()
{
	int checksum = 0;

	DataCount = GetBufIndex() - (HeaderStart + HeaderWords); // 5 excludes checksum, identifier, reserved, and data count words
	Sets32(HeaderStart+3, DataCount);

	Sets16(HeaderStart, (0x80 | SegmentType)); // set segment type and checksum = 0 to include in checksum calculation.
//	Sets16(HeaderStart+1, Identifier);
	checksum = ComputeChecksum(HeaderStart, DataCount + HeaderWords); // 5 includes checksum, identifier, reserved, and data count words
	checksum ^= 0xFF;

	Sets16(HeaderStart, (checksum << 8) | 0x80 | SegmentType);
}

// Process the raw uploaded NVRAM data block
// Locate the applicable segment and set the indeces accordingly.
NVRAMError NVRAMSegment::ProcessSegmentForReading()
{
	int wordpos = 0;
	int checksum;
	char computedchecksum;
	int datawordcount;
	int segmenttype;
	int identifier;
	int reserved;

	BufIndex = 0;

	if ((WORD)Gets16(0) == (WORD)0xFFFF)
		return NVRAMError_Erased;

	// verify PMD NVRAM header
	if ((Gets16(0) != 0x0000) ||
		(Gets16(1) != 0x0000) ||
		(Gets16(2) != 0x0000) ||
		(Gets16(3) != Version))
		return NVRAMError_InvalidHeader;


	BufIndex = 4;

	// skip 4 word user header 
	BufIndex += 4;

	while (!IsEndOfData(BufIndex))
	{
		// bit 7 must be 1 in segment type byte.
		if ((Gets16(BufIndex) & 0x80) != 0x80)
			return NVRAMError_InvalidSegmentType;

		int start = BufIndex; // location to start checksum calculation
		checksum = (Gets16(BufIndex) >> 8) & 0xFF;
		segmenttype = Gets16(BufIndex++) & 0x7F;
		identifier = Gets16(BufIndex++);
		reserved = Gets16(BufIndex++);
		datawordcount = Gets32(BufIndex);
		BufIndex+=2;

		if (datawordcount > NVRAM_SIZE_WORDS - HeaderWords)
			return NVRAMError_InvalidDataCount;

		if (datawordcount == 0)
			return NVRAMError_ZeroDataCount;

		computedchecksum = ComputeChecksum(start, datawordcount + HeaderWords);
		if (computedchecksum != (char)0xFF)
			return NVRAMError_Checksum;

		if (segmenttype == SegmentType && Identifier == identifier)
		{
			HeaderStart = start;
			DataIndex = BufIndex;
			DataCount = datawordcount;
			Checksum = checksum;
			m_bValid = TRUE;
			return NVRAMError_None;
		}
		BufIndex += datawordcount; // move to next segment
		if (Version > 1)
			while (BufIndex % 4)
				BufIndex++;

	};
	return NVRAMError_SegmentNotFound;
}

int	NVRAMSegment::GetChecksum()
{
	return Checksum;
}

int	NVRAMSegment::GetIdentifier()
{
	return Identifier;
}

NVRAMError NVRAMSegment::GetSegmentData(CWordArray& data)
{
	NVRAMError error = ProcessSegmentForReading();
	if (error == NVRAMError_None)
	{
		data.SetSize(DataCount);
		for( int i=0; i<DataCount; i++ )
		{
			data[i] = Gets16(DataIndex+i);
		}
	}
	return error;
}

void NVRAMSegment::AddSegmentData(CWordArray& data)
{
	DataCount = data.GetSize();
	for( int i=0; i<DataCount; i++ )
		Adds16(data[i]);
	PostProcessSegmentForWriting();
}


// Specific Segment classes
// ********************************************************************************

NVRAMSegmentCommands::NVRAMSegmentCommands(int version, int identifier) 
	: NVRAMSegment(version, identifier)
{
	SegmentType = NVRAMSegmentType_InitializationCommands;
}

NVRAMSegmentProductData::NVRAMSegmentProductData(int version, int identifier)
	: NVRAMSegment(version, identifier)
{
	SegmentType = NVRAMSegmentType_ProductData;
}

NVRAMSegmentProductData::~NVRAMSegmentProductData(void){}


#define PRODUCT_NAME_BYTES		4
// set the 4 byte name header to csName
void NVRAMSegmentProductData::SetHeaderFieldName(CString csName)
{
	char FieldName[PRODUCT_NAME_BYTES];
	int bytecount = csName.GetLength();

	ASSERT(bytecount <= PRODUCT_NAME_BYTES);
	memset(FieldName, 0, PRODUCT_NAME_BYTES);
	for( int i=0; i<bytecount; i++ )
	{
		FieldName[i] = csName[i];
	}
	AddBytes(FieldName, PRODUCT_NAME_BYTES);
}

// set the 2 byte length and type header field
void NVRAMSegmentProductData::SetHeaderFieldLength(NVRAMDataType datatype, int wordcount)
{
	ASSERT(wordcount < (1<<12));
	Adds16(datatype << 12 | wordcount);
}

// AddField adds a Product Data field to the NVRAM buffer
/*
  |     name 1     |    name 0     |
  |     name 3     |    name 2     |
  |  type  |  data length in words |
  |     data 1     |    data 0     |
  |              ...               |
*/
// Add wide string field
void NVRAMSegmentProductData::AddField(CString csName, CString data)
{
	int length = data.GetLength();
	SetHeaderFieldName(csName);
	SetHeaderFieldLength(NVRAMDataType_widestring, length);
	AddString(data);
}

void NVRAMSegmentProductData::AddField(CString csName, float data)
{
	SetHeaderFieldName(csName);
	SetHeaderFieldLength(NVRAMDataType_float, 2);
	Addf32(data);
}

void NVRAMSegmentProductData::AddField(CString csName, short data)
{
	SetHeaderFieldName(csName);
	SetHeaderFieldLength(NVRAMDataType_s16, 1);
	Adds16(data);
}

void NVRAMSegmentProductData::AddField(CString csName, long data)
{
	SetHeaderFieldName(csName);
	SetHeaderFieldLength(NVRAMDataType_s32, 2);
	Adds32(data);
}

// pos will be assigned the buffer index of data length field.
NVRAMError NVRAMSegmentProductData::FindProductDataName(CString csName, int& pos)
{
	int datastart = HeaderStart + HeaderWords;
	pos = datastart;

	ASSERT(csName.GetLength() <= 4);
	while (pos < datastart + DataCount)
	{
		if (0 == strncmp(csName, GetBytes(pos), csName.GetLength()))
		{
			pos += 2;
			return NVRAMError_None;
		}
		pos += 2;						// move past name
		int datalength = Gets16(pos) & 0x0FFF;	
		pos++;							// move past data length word
		pos += datalength;				// move past data of current product data item
	}
	return NVRAMError_ProductDataNotFound;
}

// GetField retrieves a Product Data field from the NVRAM buffer
NVRAMError NVRAMSegmentProductData::GetField(CString csName, CString& str)
{
	int pos;
	NVRAMError error;

	error = FindProductDataName(csName, pos);
	if (error == NVRAMError_None)
	{
		int wordcount = Gets16(pos) & 0x0FFF;	
		int type = Gets16(pos) >> 12;	
		if (type != NVRAMDataType_widestring)
			return NVRAMError_UnexpectedDataType;
		pos++; // move to data
		str.Empty();
		GetString(pos, wordcount, str);
	}
	return error;
}

NVRAMError NVRAMSegmentProductData::GetField(CString csName, float& data)
{
	int pos;
	NVRAMError error;

	error = FindProductDataName(csName, pos);
	if (error == NVRAMError_None)
	{
		int type = Gets16(pos) >> 12;
		// verify that the stored type is the same as the expected type.
		if (type != NVRAMDataType_float)
			return NVRAMError_UnexpectedDataType;
		pos++;
		data = Getf32(pos);
	}
	return error;
}

NVRAMError NVRAMSegmentProductData::GetField(CString csName, short& data)
{
	int pos;
	NVRAMError error;

	error = FindProductDataName(csName, pos);
	if (error == NVRAMError_None)
	{
		int type = Gets16(pos) >> 12;
		// verify that the stored type is the same as the expected type.
		if (type != NVRAMDataType_s16)
			return NVRAMError_UnexpectedDataType;
		pos++;
		data = Gets16(pos);
	}
	return error;
}

NVRAMError NVRAMSegmentProductData::GetField(CString csName, long& data)
{
	int pos;
	NVRAMError error;

	error = FindProductDataName(csName, pos);
	if (error == NVRAMError_None)
	{
		int type = Gets16(pos) >> 12;
		// verify that the stored type is the same as the expected type.
		if (type != NVRAMDataType_s32)
			return NVRAMError_UnexpectedDataType;
		pos++;
		data = Gets32(pos);
	}
	return error;
}


void NVRAMSegmentProductData::SetLegCurrentScale(float fData)
{
	AddField("ILS", fData);
}

void NVRAMSegmentProductData::SetBusCurrentScale(float fData)
{
	AddField("IBS", fData);
}

void NVRAMSegmentProductData::SetBusVoltageScale(float fData)
{
	AddField("VBS", fData);
}

void NVRAMSegmentProductData::SetPartNumber(CString csData)
{
	AddField("P#", csData);
}

void NVRAMSegmentProductData::SetSerialNumber(CString csData)
{
	AddField("S#", csData);
}

void NVRAMSegmentProductData::SetPartName(CString csData)
{
	AddField("PN", csData);
}

void NVRAMSegmentProductData::SetPartVersion(CString csData)
{
	AddField("PVER", csData);
}

void NVRAMSegmentProductData::SetConfigName(CString csData)
{
	AddField("CN", csData);
}

void NVRAMSegmentProductData::SetConfigVersion(CString csData)
{
	AddField("CVER", csData);
}

void NVRAMSegmentProductData::SetDate(CString csData)
{
	AddField("DATE", csData);
}

void NVRAMSegmentProductData::SetWriteDate(CString csData)
{
	AddField("WD", csData);
}

void NVRAMSegmentProductData::SetFileDate(CString csData)
{
	AddField("FD", csData);
}

void NVRAMSegmentProductData::SetFileName(CString csData)
{
	AddField("FN", csData);
}

void NVRAMSegmentProductData::SetDescription(CString csData)
{
	AddField("DESC", csData);
}

NVRAMError NVRAMSegmentProductData::GetConfigName(CString& csData)
{
	return GetField("CN", csData);
}

NVRAMError NVRAMSegmentProductData::GetConfigVersion(CString& csData)
{
	return GetField("CVER", csData);
}

NVRAMError NVRAMSegmentProductData::GetLegCurrentScale(float& fData)
{
	return GetField("ILS", fData);
}

NVRAMError NVRAMSegmentProductData::GetBusCurrentScale(float& fData)
{
	return GetField("IBS", fData);
}

NVRAMError NVRAMSegmentProductData::GetBusVoltageScale(float& fData)
{
	return GetField("VBS", fData);
}

NVRAMError NVRAMSegmentProductData::GetI2tScale(float& fData)
{
	return GetField("I2TS", fData);
}

NVRAMError NVRAMSegmentProductData::GetPartNumber(CString& csData)
{
	return GetField("P#", csData);
}

NVRAMError NVRAMSegmentProductData::GetSerialNumber(CString& csData)
{
	return GetField("S#", csData);
}

NVRAMError NVRAMSegmentProductData::GetPartName(CString& csData)
{
	return GetField("PN", csData);
}

NVRAMError NVRAMSegmentProductData::GetPartVersion(CString& csData)
{
	return GetField("PVER", csData);
}

NVRAMError NVRAMSegmentProductData::GetDate(CString& csData)
{
	return GetField("DATE", csData);
}

NVRAMError NVRAMSegmentProductData::GetWriteDate(CString& csData)
{
	return GetField("WD", csData);
}

NVRAMError NVRAMSegmentProductData::GetFileDate(CString& csData)
{
	return GetField("FD", csData);
}

NVRAMError NVRAMSegmentProductData::GetFileName(CString& csData)
{
	return GetField("FN", csData);
}

NVRAMError NVRAMSegmentProductData::GetDescription(CString& csData)
{
	return GetField("DESC", csData);
}



