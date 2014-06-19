#pragma once

class BMPAssistant
{
public:
	/*******************************************************************
	BYTE* ConvertRGBToBMPBuffer ( BYTE* Buffer, int width, 
	int height, long* newsize )


	This function takes as input an array of RGB values, it's width
	and height.
	The buffer gets then transformed to an array that can be used
	to write to a windows bitmap file. The size of the array
	is returned in newsize, the array itself is the
	return value of the function.
	Both input and output buffers must be deleted by the
	calling function.

	The input buffer is expected to consist of width * height
	RGB triplets. Thus the total size of the buffer is taken as
	width * height * 3.

	The function then transforms this buffer so that it can be written 
	to a windows bitmap file:
	First the RGB triplets are converted to BGR.
	Then the buffer is swapped around since .bmps store
	images uside-down.
	Finally the buffer gets DWORD ( 32bit ) aligned, 
	meaning that each scanline ( 3 * width bytes ) gets
	padded with 0x00 bytes up to the next DWORD boundary
	*******************************************************************/
	static BYTE* ConvertRGBToBMPBuffer ( BYTE* Buffer, int width, int height, long* newsize );
	/***************************************************************
	BYTE* ConvertBMPToRGBBuffer ( BYTE* Buffer, 
	int width, int height )

	This function takes as input the data array
	from a bitmap and its width and height.
	It then converts the bmp data into an RGB array.
	The calling function must delete both the input
	and output arrays.
	The size of the returned array will be 
	width * height * 3
	On error the returb value is NULL, else the
	RGB array.


	The Buffer is expected to be the exact data read out
	from a .bmp file.  
	The function will swap it around, since images
	are stored upside-down in bmps.
	The BGR triplets from the image data will
	be converted to RGB.
	And finally the function removes padding bytes.
	The returned arraay consits then of
	width * height RGB triplets.

	*****************************************************************/
	static BYTE* ConvertBMPToRGBBuffer ( BYTE* Buffer, int width, int height );
	/***********************************************
	BYTE* Flip ( BYTE* Buffer, int width, int height )
	Just Flip the buffer.

	************************************************/
	static BYTE* Flip ( BYTE* Buffer, int width, int height );

	/***********************************************
	bool LoadBMPIntoDC ( HDC hDC, LPCTSTR bmpfile )

	Takes in a device context and the name of a
	bitmap to load. If an error occurs the function
	returns false, else the contents of the bmp
	are blitted to the HDC 

	************************************************/
	static bool LoadBMPIntoDC ( HDC hDC, LPCTSTR bmpfile );

	/***************************************************************
	bool SaveBMP ( BYTE* Buffer, int width, int height, 
	long paddedsize, LPCTSTR bmpfile )

	Function takes a buffer of size <paddedsize> 
	and saves it as a <width> * <height> sized bitmap 
	under the supplied filename.
	On error the return value is false.

	***************************************************************/
	static bool SaveBMP ( BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile );


	/*******************************************************************
	BYTE* LoadBMP ( int* width, int* height, long* size 
	LPCTSTR bmpfile )

	The function loads a 24 bit bitmap from bmpfile, 
	stores it's width and height in the supplied variables
	and the whole size of the data (padded) in <size>
	and returns a buffer of the image data 

	On error the return value is NULL. 

	NOTE: make sure you [] delete the returned array at end of 
	program!!!
	*******************************************************************/
	static BYTE* LoadBMP ( int* width, int* height, long* size, LPCTSTR bmpfile );
};


// void TestBMPCopy (LPCTSTR input, LPCTSTR output)
// {
// 	int width, height;
// 	long imgsize;
// 	BYTE* srcBmp = BMPAssistant::LoadBMP ( &width, &height, &imgsize, input );
// 	BMPAssistant::SaveBMP ( srcBmp, width, height, imgsize, output );	
// }
// 
// void TestBMPCopy2(LPCTSTR input, LPCTSTR output)
// {
// 	int width, height;
// 	long imgsize, imgsize2;
// 	BYTE* srcBmp = BMPAssistant::LoadBMP ( &width, &height, &imgsize, input );
// 	BYTE* rgbBmp = BMPAssistant::ConvertBMPToRGBBuffer ( std::move(srcBmp), width, height );
// 	BYTE* dstBmp = BMPAssistant::ConvertRGBToBMPBuffer ( std::move(rgbBmp), width, height, &imgsize2 );	
// 	BMPAssistant::SaveBMP ( dstBmp, width, height, imgsize2, output );
// }