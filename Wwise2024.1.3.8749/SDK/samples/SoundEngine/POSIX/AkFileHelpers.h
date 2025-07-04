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
//////////////////////////////////////////////////////////////////////
//
// AkFileHelpers.h
//
// Platform-specific helpers for files.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_HELPERS_H_
#define _AK_FILE_HELPERS_H_

#include <AK/Tools/Common/AkAssert.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>

#if defined(AK_EMSCRIPTEN) || defined(AK_QNX) || defined(AK_HARMONY)
	#include <errno.h>
#else
	#include <sys/errno.h>
#endif

#include "../Common/AkFileHelpersBase.h"

#if defined(AK_IOS)
	#include "../iOS/AkFileHelpersiOS.h"
#endif

class CAkFileHelpers : public CAkFileHelpersBase
{
public:

	// Wrapper for OS X CreateFile().
	static AKRESULT OpenFile( 
        const AkOSChar* in_pszFilename,     // File name.
        AkOpenMode      in_eOpenMode,       // Open mode.
        bool            in_bOverlappedIO,	// Use FILE_FLAG_OVERLAPPED flag.
        AkFileHandle &  out_hFile           // Returned file identifier/handle.
        )
	{
		// Check parameters.
		if ( !in_pszFilename )
		{
			AKASSERT( !"NULL file name" );
			return AK_InvalidParameter;
		}
		
		char* mode;
		switch ( in_eOpenMode )
		{
			case AK_OpenModeRead:
				mode = (char*)"r"; 
				break;
			case AK_OpenModeWrite:
				mode = (char*)"w";
				break;
			case AK_OpenModeWriteOvrwr:
				mode = (char*)"w+";
				break;
			case AK_OpenModeReadWrite:
				mode = (char*)"a";
				break;
			default:
					AKASSERT( !"Invalid open mode" );
					out_hFile = NULL;
					return AK_InvalidParameter;
				break;
		}
				
		out_hFile = fopen(in_pszFilename, mode);
		if (!out_hFile)
		{
			if (errno == EACCES)
				return AK_FilePermissionError;

			return AK_FileNotFound;
		}

		return AK_Success;
	}
	
	//Open file and fill AkFileDesc
	static AKRESULT Open(
		const AkOSChar* in_pszFileName,     // File name.
		AkOpenMode      in_eOpenMode,       // Open mode.
		bool			in_bOverlapped,		// Overlapped IO
		AkFileDesc &    out_fileDesc		// File descriptor
		)
	{
		AKRESULT eResult = OpenFile( 
			in_pszFileName,
			in_eOpenMode,
			in_bOverlapped,
			out_fileDesc.hFile );
		if ( eResult == AK_Success )
		{
			if (in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite)
			{
				// Get Info about the file size
				if (fseek(out_fileDesc.hFile, 0L, SEEK_END))
					return AK_UnknownFileError;
				out_fileDesc.iFileSize = (AkInt64)ftell(out_fileDesc.hFile);
				rewind(out_fileDesc.hFile);
			}
			else
			{
				out_fileDesc.iFileSize = 0;
			}
		}
		return eResult;
	}

	// Wrapper for system file handle closing.
	static AKRESULT CloseFile( const AkFileDesc& in_FileDesc )
	{
		if ( !fclose( in_FileDesc.hFile ) )
			return AK_Success;
		
		AKASSERT( !"Failed to close file handle" );
		return AK_Fail;
	}

	//
	// Simple platform-independent API to open and read files using AkFileHandles, 
	// with blocking calls and minimal constraints.
	// ---------------------------------------------------------------------------

	// Open file to use with ReadBlocking().
	static AKRESULT OpenBlocking(
        const AkOSChar* in_pszFilename,     // File name.
        AkFileHandle &  out_hFile           // Returned file handle.
		)
	{
		return OpenFile( 
			in_pszFilename,
			AK_OpenModeRead,
			false,
			out_hFile );
	}

	// Required block size for reads (used by ReadBlocking() below).
	static const AkUInt32 s_uRequiredBlockSize = 1;

	// Simple blocking read method.
	static AKRESULT ReadBlocking(
        AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32		in_uPosition,		// Position from which to start reading.
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32 &		out_uSizeRead		// Returned size read.        
		)
	{
		AKASSERT( in_uSizeToRead % s_uRequiredBlockSize == 0 
			&& in_uPosition % s_uRequiredBlockSize == 0 );

		if( fseek(in_hFile, in_uPosition, SEEK_SET ) )
		{
			return AK_Fail;
		}

		out_uSizeRead = (AkUInt32) fread( in_pBuffer, 1, in_uSizeToRead , in_hFile );
		
		if( out_uSizeRead == in_uSizeToRead )
		{
			return AK_Success;
		}
		
		return AK_Fail;		
	}

	static AKRESULT ReadBlocking(
		AkFileDesc& in_fileDesc,		// Returned file identifier/handle.
		void*		in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32	in_uPosition,		// Position from which to start reading.
		AkUInt32	in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32&	out_uSizeRead		// Returned size read.        
		)
	{
		return ReadBlocking(in_fileDesc.hFile, in_pBuffer, in_uPosition, in_uSizeToRead, out_uSizeRead);
	}

	/// Returns AK_Success if the directory is valid, AK_Fail if not.
	/// For validation purposes only.
	/// Some platforms may return AK_NotImplemented, in this case you cannot rely on it.
	static AKRESULT CheckDirectoryExists( const AkOSChar* in_pszBasePath )
	{
		// WG-68198 Use of stat is forbidden
		DIR * h = opendir(in_pszBasePath);
		if (h == NULL) return AK_Fail;
		closedir(h);
		return AK_Success;
	}

	static AKRESULT WriteBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pData,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.		
		AkUInt64		in_uPosition,		// Position from which to start writing.
		AkUInt32		in_uSizeToWrite)
	{
		
#if defined( AK_QNX ) || defined (AK_EMSCRIPTEN) || defined(AK_HARMONY)
		if( !fseeko( in_hFile, in_uPosition, SEEK_SET ) )
#else

#if defined AK_LINUX  && !defined (AK_LINUX_AOSP)
		fpos_t pos;
		pos.__pos = in_uPosition;
#else
		fpos_t pos = in_uPosition;
#endif
		if( !fsetpos( in_hFile, &pos ) )
#endif
		{
			size_t itemsWritten = fwrite( in_pData, 1, in_uSizeToWrite, in_hFile );
			if( itemsWritten > 0 )
			{
				fflush( in_hFile );
				return AK_Success;
			}
		}
		return AK_Fail;
	}

	static AKRESULT CreateEmptyDirectory( const AkOSChar* in_pszDirectoryPath )
	{
		int iErr = mkdir(in_pszDirectoryPath, S_IRWXU | S_IRWXG | S_IRWXO);
		if (iErr == -1 && errno != EEXIST)
			return AK_Fail;

		return AK_Success;
	}

	static AKRESULT RemoveEmptyDirectory( const AkOSChar* in_pszDirectoryPath )
	{
		int iErr = rmdir(in_pszDirectoryPath);
		if (iErr == -1)
			return AK_Fail;

		return AK_Success;
	}

	static AKRESULT GetDefaultWritablePath(AkOSChar* out_pszPath, AkUInt32 in_pathMaxSize)
	{
		if (out_pszPath == nullptr)
			return AK_InsufficientMemory;

		out_pszPath[0] = '\0';

		#if defined(AK_LINUX_DESKTOP) || defined(AK_MAC_OS_X)
			// No strict writable path enforcement (return "")
		#elif defined(AK_IOS)
			return CAkFileHelpersiOS::GetDefaultWritablePath(out_pszPath, in_pathMaxSize);
		#else
			return AK_NotImplemented;
		#endif

		return AK_Success;
	}
	
	static void NormalizeDirectorySeparators(AkOSChar* io_path) { } // no-op
};

#endif //_AK_FILE_HELPERS_H_
