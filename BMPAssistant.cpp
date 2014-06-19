#include "stdafx.h"

#include "BMPAssistant.h"

#include <windows.h>
#include <stdio.h>       // for memset
#include <cmath>

BYTE* BMPAssistant::ConvertRGBToBMPBuffer ( BYTE* Buffer, int width, int height, long* newsize )
{

	// first make sure the parameters are valid
	if ( ( nullptr == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return nullptr;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// we can already store the size of the new padded buffer
	*newsize = height * psw;

	// and create new buffer
	BYTE* newbuf (new BYTE[*newsize]);	
	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on	

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;   
	long newpos = 0;
	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < 3 * width; x+=3 )
		{
			bufpos = y * 3 * width + x;     // position in original buffer
			newpos = ( height - y - 1 ) * psw + x;           // position in padded buffer

			newbuf[newpos] = Buffer[bufpos+2];       // swap r and b
			newbuf[newpos + 1] = Buffer[bufpos + 1]; // g stays
			newbuf[newpos + 2] = Buffer[bufpos];     // swap b and r
		}

	}

	return newbuf;
}

BYTE* BMPAssistant::ConvertBMPToRGBBuffer ( BYTE* Buffer, int width, int height )
{
	// first make sure the parameters are valid
	if ( ( nullptr == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return nullptr;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf (new BYTE[width*height*3]);

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;   
	long newpos = 0;
	for ( int y = 0; y < height; y++ )
		for ( int x = 0; x < 3 * width; x+=3 )
		{
			newpos = y * 3 * width + x;     
			bufpos = ( height - y - 1 ) * psw + x;

			newbuf[newpos] = Buffer[bufpos + 2];       
			newbuf[newpos + 1] = Buffer[bufpos+1]; 
			newbuf[newpos + 2] = Buffer[bufpos];     
		}

		return newbuf;
}

BYTE* BMPAssistant::Flip ( BYTE* Buffer, int width, int height )
{
	// first make sure the parameters are valid
	if ( ( nullptr == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return nullptr;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf (new BYTE[width*height*3]);

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;   
	long newpos = 0;
	for ( int y = 0; y < height; y++ )
		for ( int x = 0; x < 3 * width; x+=3 )
		{
			newpos = y * 3 * width + x;     
			bufpos = ( height - y - 1 ) * psw + x;

			newbuf[newpos] = Buffer[bufpos];       
			newbuf[newpos + 1] = Buffer[bufpos+1]; 
			newbuf[newpos + 2] = Buffer[bufpos + 2];     
		}

		return newbuf;
}

bool BMPAssistant::LoadBMPIntoDC ( HDC hDC, LPCTSTR bmpfile )
{
	// check if params are valid 
	if ( ( NULL == hDC  ) || ( NULL == bmpfile ) )
		return false;      

	// load bitmap into a bitmap handle
	HANDLE hBmp = LoadImage ( NULL, bmpfile, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE );

	if ( NULL == hBmp )
		return false;        // failed to load image

	// bitmaps can only be selected into memory dcs:
	HDC dcmem = CreateCompatibleDC ( NULL );

	// now select bitmap into the memory dc
	if ( NULL == SelectObject ( dcmem, hBmp ) )
	{	// failed to load bitmap into device context
		DeleteDC ( dcmem ); 
		return false; 
	}


	// now get the bmp size
	BITMAP bm;
	GetObject ( hBmp, sizeof(bm), &bm );
	// and blit it to the visible dc
	if ( BitBlt ( hDC, 0, 0, bm.bmWidth, bm.bmHeight, dcmem,
		0, 0, SRCCOPY ) == 0 )
	{	// failed the blit
		DeleteDC ( dcmem ); 
		return false; 
	}

	DeleteDC ( dcmem );  // clear up the memory dc

	return true;
}


bool BMPAssistant::SaveBMP ( BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile )
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// andinitialize them to zero
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );

	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFile ( bmpfile , GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( file == NULL )
	{
		CloseHandle ( file );
		return false;
	}

	// write file header
	unsigned long bwritten;
	if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write infoheader
	if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write image data
	if ( WriteFile ( file, Buffer, paddedsize, &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}

	// and close file
	CloseHandle ( file );

	return true;
}

BYTE* BMPAssistant::LoadBMP ( int* width, int* height, long* size, LPCTSTR bmpfile )
{
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	DWORD bytesread;
	// open file to read from
	HANDLE file = CreateFile ( bmpfile , GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if ( NULL == file )
		return nullptr; // coudn't open file


	// read file header
	if ( ReadFile ( file, &bmpheader, sizeof ( BITMAPFILEHEADER ), &bytesread, NULL ) == false )
	{
		CloseHandle ( file );
		return nullptr;
	}

	//read bitmap info

	if ( ReadFile ( file, &bmpinfo, sizeof ( BITMAPINFOHEADER ), &bytesread, NULL ) == false )
	{
		CloseHandle ( file );
		return nullptr;
	}

	// check if file is actually a bmp
	if ( bmpheader.bfType != 'MB' )
	{
		CloseHandle ( file );
		return nullptr;
	}

	// get image measurements
	*width   = bmpinfo.biWidth;
	*height  = abs ( bmpinfo.biHeight );

	// check if bmp is uncompressed
	if ( bmpinfo.biCompression != BI_RGB )
	{
		CloseHandle ( file );
		return nullptr;
	}

	// check if we have 24 bit bmp
	if ( bmpinfo.biBitCount != 24 )
	{
		CloseHandle ( file );
		return nullptr;
	}


	// create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;
	BYTE* Buffer(new BYTE[ *size ]);
	// move file pointer to start of bitmap data
	SetFilePointer ( file, bmpheader.bfOffBits, NULL, FILE_BEGIN );
	// read bmp data
	if ( ReadFile ( file, Buffer, *size, &bytesread, NULL ) == false )
	{		
		CloseHandle ( file );
		return nullptr;
	}

	// everything successful here: close file and return buffer

	CloseHandle ( file );

	return Buffer;
}
