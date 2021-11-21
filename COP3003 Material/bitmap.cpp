//---------------------------------------------------------
// File   : bitmap.cpp
// Author : P.Allen
// Purpose: read a bitmap file into memory structures
//---------------------------------------------------------

#include <iostream>
#include <string>
#include <fstream>  // file input lib
#include <cmath>	// floor function

using namespace std;

// Global defines
// --------------------------------------------------------

// data type sizes for data structures
#define BYTE uint8_t		// 1-byte
#define WORD uint16_t		// 2-bytes
#define DWORD uint32_t		// 4-bytes

typedef struct {
	BYTE  bitmapType[2];
	DWORD fileSize;
	WORD  reserved1;
	WORD  reserved2;
	DWORD offset;
} BMFileHeader;

typedef struct {
	DWORD headerSize;
	DWORD bitmapWidth;
	DWORD bitmapHeight;
	WORD  colorPlanes;
	WORD  bitDepth;
	DWORD compressionMethod;
	DWORD imageSize;
	DWORD horizontalResolution;
	DWORD verticalResolution;
	DWORD numberColors;
	DWORD numberImportantColors;
} BMInfoHeader;

typedef struct {
	BYTE red;
	BYTE green;
	BYTE blue;
} Pixel;


// Function prototypes
// --------------------------------------------------------
int readByte(ifstream &bmFile, BYTE &field);
int readWord(ifstream &bmFile, WORD &field);
int readDWord(ifstream &bmFile, DWORD &field);

int main() {

	// open bitmap file
	string filename;

	cout << "Bitmap file name: ";
	cin >> filename;

	ifstream bmFile;

	bmFile.open(filename, ios::binary);

	int bytesRead = 0;

	// read the file header
	BMFileHeader bmFileHeader;

	bytesRead += readByte(bmFile, bmFileHeader.bitmapType[0]);
	bytesRead += readByte(bmFile, bmFileHeader.bitmapType[1]);
	bytesRead += readDWord(bmFile, bmFileHeader.fileSize);
	bytesRead += readWord (bmFile, bmFileHeader.reserved1);
	bytesRead += readWord (bmFile, bmFileHeader.reserved2);
	bytesRead += readDWord(bmFile, bmFileHeader.offset);

	cout << "File type:    " << bmFileHeader.bitmapType[0] << bmFileHeader.bitmapType[1] << endl;
	cout << "File Size:    " << bmFileHeader.fileSize << endl;
	cout << "Pixel Offset: " << bmFileHeader.offset << endl;

	BMInfoHeader bmInfoHeader;
	bytesRead += readDWord(bmFile, bmInfoHeader.headerSize);
	bytesRead += readDWord(bmFile, bmInfoHeader.bitmapWidth);
	bytesRead += readDWord(bmFile, bmInfoHeader.bitmapHeight);
	bytesRead += readWord(bmFile, bmInfoHeader.colorPlanes);
	bytesRead += readWord(bmFile, bmInfoHeader.bitDepth);
	bytesRead += readDWord(bmFile, bmInfoHeader.compressionMethod);
	bytesRead += readDWord(bmFile, bmInfoHeader.imageSize);
	bytesRead += readDWord(bmFile, bmInfoHeader.horizontalResolution);
	bytesRead += readDWord(bmFile, bmInfoHeader.verticalResolution);
	bytesRead += readDWord(bmFile, bmInfoHeader.numberColors);
	bytesRead += readDWord(bmFile, bmInfoHeader.numberImportantColors);

	cout << "Header Size: " << bmInfoHeader.headerSize << endl;
	cout << "width      : " << bmInfoHeader.bitmapWidth << endl;
	cout << "height     : " << bmInfoHeader.bitmapHeight << endl;
	cout << "bitDepth   : " << bmInfoHeader.bitDepth << endl;


	// store remaining bytes between info header and pixel array
	BYTE *gap1 = nullptr;
	int gap1Size = bmFileHeader.offset - bytesRead;
	if (gap1Size) {
		gap1 = new BYTE[gap1Size];
		for (int next = 0; next < gap1Size; next++) {
			bytesRead += readByte(bmFile, gap1[next]);
		}
	} // gap 1 exists


	// read pixel array

	int rowSize = floor((bmInfoHeader.bitDepth * bmInfoHeader.bitmapWidth + 31) / 32) * 4;

	Pixel **pixels = new Pixel * [bmInfoHeader.bitmapHeight];
	for (int row = 0; row < bmInfoHeader.bitmapHeight; row++) {

		pixels[row] = new Pixel[bmInfoHeader.bitmapWidth];

		int rowBytesRead = 0;

		for (int column = 0; column < bmInfoHeader.bitmapWidth; column++) {
			Pixel pixel = { 0 };
			
			rowBytesRead += readByte(bmFile, pixel.blue);
			rowBytesRead += readByte(bmFile, pixel.green);
			rowBytesRead += readByte(bmFile, pixel.red);

			pixels[row][column] = pixel;
		} // for each column

		int padding = rowSize - rowBytesRead;
		for (int pad = 0; pad < padding; pad++) {
			BYTE scratch;
			rowBytesRead += readByte(bmFile, scratch);
		}

		bytesRead += rowBytesRead;

	} // for each row



	return 0;
} // end main


/**
 * read a single byte from file stream
 * @param bmFile - files stream
 * @param field  - fill from file stream
 * @return int   - number of bytes read
 */
int readByte(ifstream& bmFile, BYTE& field)
{
	int sizeRead = sizeof(BYTE);
	bmFile.read(reinterpret_cast<char*>(&field), sizeRead);
	return sizeRead;
} // end readByte


/**
 * read a two bytes from file stream
 * @param bmFile - files stream
 * @param field  - fill from file stream
 * @return int   - number of bytes read
 */
int readWord(ifstream& bmFile, WORD& field)
{
	int sizeRead = sizeof(WORD);
	bmFile.read(reinterpret_cast<char*>(&field), sizeRead);
	return sizeRead;
} // end readWord


/**
 * read a four bytes from file stream
 * @param bmFile - files stream
 * @param field  - fill from file stream
 * @return int   - number of bytes read
 */
int readDWord(ifstream& bmFile, DWORD& field)
{
	int sizeRead = sizeof(DWORD);
	bmFile.read(reinterpret_cast<char*>(&field), sizeRead);
	return sizeRead;
} // end readDWord