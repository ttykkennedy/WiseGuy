/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

// FreetypeGraphicRenderer.h
/// \file
/// Draw strings to a RGB 565 buffer (or ARGB8888 if FREETYPE_USE_32_BIT_RENDERER is defined)

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include "Platform.h"

#ifndef NDK_APP
#include "Drawing.h"
#endif

#include "stb_truetype.h"

struct PixelType
{
	union
	{
		AkUInt8 col[4];
		AkUInt32 px;
	}; 
};

/// Replaces tags by their platform-specific values
void ReplaceTags( std::string& io_strTagString );

class FreetypeGraphicRenderer
{
public:
	FreetypeGraphicRenderer():
	m_canvasWidth(0),
	m_canvasHeight(0),
	m_layoutWidth(0),
	m_layoutHeight(0),
	m_pWindowBuffer(NULL)
	{}


	~FreetypeGraphicRenderer(){};

	/// Initializes the system's drawing engine for usage.
	/// \return True if the system has been initialized, false otherwise.
	/// \sa TermDrawing()
	bool InitDrawing(AkOSChar* in_szErrorBuffer, ///< - Buffer where error details will be written (if the function returns false)
		unsigned int in_unErrorBufferCharCount, ///< - Number of characters available in in_szErrorBuffer, including terminating NULL character */ 
		int in_canvasWidth,  ///< - canvas width in pixels  (e.g. size of the buffer being rendered to)
		int in_canvasHeight, ///< - canvas height in pixels
		int in_layoutWidth,  ///< - layout width in pixels (e.g. size of the layout, to scale/position glyphs
		int in_layoutHeight  ///< - layout height in pixels

	);

	/// Begins a drawing sequence
	/// \sa DoneDrawing()
	void BeginDrawing();


	/// Draws a string of text on the screen at a given point.
	/// \note The coordinates (0, 0) refer to the top-left corner of the screen.
	/// \warning This function must be called between calls to BeginDrawing and DoneDrawing.
	/// \sa BeginDrawing(), DoneDrawing()
	void DrawTextOnScreen(
						  const char* in_szText,	///< - The string to draw on the screen
						  int in_iXPos,			    ///< - The X value of the layout position
						  int in_iYPos,			    ///< - The Y value of the layout position
						  DrawStyle in_eDrawStyle = DrawStyle_Control		///< - The style with which to draw the text
						  );

	/// Ends the drawing sequence and outputs the drawing.
	/// \sa BeginDrawing()
	 void DoneDrawing();


	/// Closes the drawing engine and releases any used resources.
	/// \sa InitDrawing()
	void TermDrawing();


	/// Gets the height of a line of text drawn using the given style.
	/// \return The height of a line of text, in pixels.
	int GetLayoutLineHeight(
							  DrawStyle in_eDrawStyle		///< - The style of the text being queried
							  );

	/// Returns the bounding box of a character.
	void GetGlyphBoundingBox(int codepoint, int& out_x1, int& out_y1, int& out_x2, int& out_y2, DrawStyle in_eDrawStyle);

	/// Access the data buffer
	/// \return the pointers to bits in the buffer
	const PixelType* GetCanvasBuffer();

	/// Gets the number of bytes of memory per row of pixels
	/// \return The number of bytes that each row takes in memory
	int GetCanvasBufferPitch() { return m_canvasWidth * sizeof(PixelType); }

	/// Gets the total length of the memory represented by the WindowBuffer
	/// \return The total size of the window buffer
	int GetCanvasBufferLength() { return m_canvasWidth * m_canvasHeight * sizeof(PixelType); }

private:

	bool InitFreetype();
	void printString( const char* pStr, int in_layoutX, int in_layoutY, DrawStyle in_eDrawStyle);
	void draw_bitmap( unsigned char* bitmap, int canvasX, int canvasY, int bitmapWidth, int bitmapHeight, uint32_t color);
	
	unsigned int m_canvasWidth;
	unsigned int m_canvasHeight;
	unsigned int m_layoutWidth;
	unsigned int m_layoutHeight;

	stbtt_fontinfo m_font;
	
	PixelType*	  m_pWindowBuffer;
};
