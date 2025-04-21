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
// AkIOMemMgr.h
//
// IO memory management.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_IO_MEM_MGR_H_
#define _AK_IO_MEM_MGR_H_

#include "AkStreamMgr.h"
#include <AK/Tools/Common/AkListBare.h>
#include <AK/Tools/Common/AkListBareLight.h>
#include <AK/Tools/Common/AkArray.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include "AkBuddyBlockPool.h"

// ------------------------------------------------------------------------------
// Defines.
// ------------------------------------------------------------------------------

namespace AK
{
namespace StreamMgr
{
	class CAkIOThread;
	class CAkLowLevelTransfer;

	// ------------------------------------------------------------------------------
    // IO buffer indexation.
	// AkMemBlock. Block of memory with the representation of the data it contains.
    // ------------------------------------------------------------------------------
	struct AkMemBlock
	{
		AkMemBlock( void * in_pMemAddress )
			: uPosition( 0 )
			, pData( in_pMemAddress )
			, pTransfer( NULL )
			, uAvailableSize( 0 )
			, uAllocSize( 0 )
			, cacheID( AK_INVALID_CACHE_ID )
			, uRefCount( 0 )			
		{}

		// A memory block is busy being filled with new data when it's associated low-level transfer is set.
		inline bool IsBusy() { return ( pTransfer != NULL ); }

		// A memory is tagged with data if file ID is set.
		inline bool IsTagged() { return cacheID != AK_INVALID_CACHE_ID; }

		// Attributes.
		AkUInt64		uPosition;		// Position of data in file, relative to start. Files with the same ID but different uSector (typically, packaged or not) may share the same data.
		void *			pData;			// Memory address.
		union	// Note: a block cannot be free (using pNextBlock) and having a low-level transfer pending (using pTransfer) at the same time.
		{
			AkMemBlock *	pNextBlock;		// List bare sibling for memory pool. 
			CAkLowLevelTransfer *	pTransfer;	// Low-level transfer on its way to refill this block. When the block is idle, pTransfer is NULL.
		};
		AkUInt32		uAvailableSize;	// Number of bytes of valid data in this memory block.
		AkUInt32		uAllocSize;		// Buffer allocation size
		AkCacheID		cacheID;		// File ID. If the block is not tagged (and therefore cannot be shared), it is AK_INVALID_FILE_ID.
		AkUInt16		uRefCount;		// Block's ref count.
	};
	template <class T>
	struct AkListBareNextMemBlock
	{
		static AkForceInline T *& Get( T * in_pItem ) 
		{
			return in_pItem->pNextBlock;
		}
	};

	
	// ------------------------------------------------------------------------------
    // Memory blocks dictionnary. 
	// Array extended with an optimized Move() function, useful when changing memory 
	// blocks information.
	// ------------------------------------------------------------------------------
	typedef AkArrayAllocatorNoAlign<AkMemID_Streaming> ArrayPoolLocal;
	typedef AkArray<AkMemBlock*,AkMemBlock*, ArrayPoolLocal> AkArrayBlocks;
	class AkMemBlocksDictionnary : public AkArrayBlocks
	{
	public:
		AkMemBlocksDictionnary() {}
		~AkMemBlocksDictionnary() {}

		void Move( unsigned int in_uIndexSource, unsigned int in_uIndexDestination )
		{
			AKASSERT( in_uIndexSource < Length() );
			AkMemBlock* sourceItem = m_pItems[in_uIndexSource];
			if ( in_uIndexSource >= in_uIndexDestination )
			{
				AKASSERT( in_uIndexDestination < Length() );
				for ( unsigned int uItem = in_uIndexSource; uItem > in_uIndexDestination; uItem-- )
					m_pItems[uItem] = m_pItems[uItem-1];				
			}
			else
			{
				// Note: since our source item is removed from the beginning of the array compared to the 
				// destination, the actual desired destination needs to be decremented by one.
				--in_uIndexDestination;
				AKASSERT( in_uIndexDestination < Length() );
				for ( unsigned int uItem = in_uIndexSource + 1; uItem <= in_uIndexDestination; uItem++ )
					m_pItems[uItem-1] = m_pItems[uItem];
			}
			m_pItems[in_uIndexDestination] = sourceItem;
		}
	};

	// ------------------------------------------------------------------------------
    // IO Memory manager.  
	// Manages a streaming IO memory pool, and data caching.
    // ------------------------------------------------------------------------------
	class CAkIOMemMgr
	{
	public:
		CAkIOMemMgr();
		virtual ~CAkIOMemMgr();

		AKRESULT Init( const AkDeviceSettings &	in_settings,
			CAkIOThread* in_pIoThread );
		void Term();

		// IO memory access.
		// IMPORTANT: These methods are not thread safe. 
		//

		// Release memory block.
		// Returns refcount after releasing.
		AkUInt32 ReleaseBlock(
			AkMemBlock *	in_pMemBlock		// Released memory block.
			);

		// Get a free memory block.
		// Removes it from the list and returns an addref'd memory block if available. NULL if none found.
		void GetOldestFreeBlock(
			AkUInt32 in_uRequestedBufferSize,
			AkUInt32 in_uBlockAlignment,
			AkMemBlock *&	out_pMemBlock		// Returned memory block is addref'd (to 1), NULL if none found.
			);

		// Finds a cached memory block if available.
		// If a match was found, out_pMemBlock is set and addref'd. NULL otherwise.
		// Returns the offset that is useable for the client within the cache buffer (0 if no block found). 
		// The actual data size io_uRequestedSize is updated, and is always between in_uMinSize and 
		// io_uRequestedSize, except if it is the last memory region of the file.
		// Returns the useable address within the cache buffer if a match was found, NULL otherwise.
		// Notes: 
		// - Returned memory block is addref'd.
		// - Alignment and minimum data size constraint applicable to cache are handled herein.
		AkUInt32 GetCachedBlock(
			AkCacheID		in_fileID,			// Block's associated file ID.
			AkUInt64		in_uPosition,		// Desired position in file.
			AkUInt32		in_uMinSize,		// Minimum data size acceptable (discard otherwise).
			AkUInt32		in_uRequiredAlign,	// Required data alignment.
			bool			in_bEof,			// True if last buffer in file
			AkUInt32 &		io_uRequestedSize,	// In: desired data size; Out: returned valid size. Modified only if a block is found.
			AkMemBlock *&	out_pMemBlock		// Returned memory block.
			);
		
		// Tag a block with caching info BEFORE IO transfer.
		AKRESULT TagBlock(
			AkMemBlock *	in_pMemBlock,		// Memory block to tag with cache info.
			CAkLowLevelTransfer * in_pTransfer,	// Associated transfer.
			AkCacheID		in_fileID,			// File ID.
			AkUInt64		in_uPosition,		// Absolute position in file.
			AkUInt32		in_uDataSize		// Size of valid data fetched from Low-Level IO.
			);
		// Untag a block after a cancelled or failed IO transfer.
		void UntagBlock(
			AkMemBlock *	in_pMemBlock		// Memory block to tag with caching info.
			);
		// Untag all blocks (cache flush).
		void FlushCache();

		// Temporary blocks. 
		// Clone a memory block in order to have simultaneous low-level transfers on the 
		// same mapped memory (needed by standard streams in deferred device). 
		void CloneTempBlock( 
			AkMemBlock * in_pMemBlockBase,
			AkMemBlock *& out_pMemBlock 
			);
		void DestroyTempBlock( 
			AkMemBlock * in_pMemBlockBase,
			AkMemBlock * in_pMemBlock 
			);

		
		// Getters.
		//
		inline bool UseCache() { return m_bUseCache; }

		// Profiling.
		//
		void GetProfilingData(
			AkUInt32 in_uBlockSize,
			AkDeviceData &  out_deviceData
			);

	private:

		void UpdatePeakMemUsed();

#ifdef _DEBUG
		void CheckCacheConsistency( AkMemBlock * in_pMustFindBlock = NULL );
		#define CHECK_CACHE_CONSISTENCY( _arg_ ) CheckCacheConsistency( _arg_ )
#else
		#define CHECK_CACHE_CONSISTENCY( _arg_ )
#endif

#ifdef AK_STREAM_MGR_INSTRUMENTATION
		void PrintCache();
		#define PRINT_CACHE() PrintCache()
#else
		#define PRINT_CACHE()
#endif


	private:

		void FreeMemBlock( AkMemBlock* in_pBlk );
		AkMemBlock* AllocMemBlock( AkUInt32 in_uAllocSize, AkUInt32 in_uBufferSize, AkUInt32 in_uBlockAlignment );
		
		// Collection of memory blocks.
		typedef AkListBare<AkMemBlock, AkListBareNextMemBlock,AkCountPolicyWithCount>	AkMemBlockList;
		
		AkMemBlockList		m_listCachedBuffers;	// Free blocks (ref count = 0). This is a linked list for MRU: always get the buffer that has been free for the longest time.
		AkUInt32			m_totalCachedMem;		// Sum of the sizes of m_listCachedBuffers. 
		AkUInt32			m_totalAllocedMem;

		// Dictionnary of cached data.
		// The memory manager keeps blocks sorted in order to perform binary searching.
		// Keys:
		// 1) File ID (increasing)
		// 2) File position (decreasing)
		// 3) Address of data (increasing)
		// Only keys 1 and 2 are used in order to find a suitable cached block. 
		// All 3 keys are used in order to find a unique block (for tagging).
		AkMemBlocksDictionnary	m_arTaggedBlocks;		// Repository of all memory blocks: accessed for caching.

		// Global settings.
		AkBuddyBlockPool	m_StreamPool;
		bool				m_bUseCache;		// True if AkDeviceSettings::bUseStreamCache is true

		CAkIOThread* 		m_pIoThread;

		// Profiling.
#ifndef AK_OPTIMIZED
		AkUInt32        m_streamIOPoolSize;     // IO memory size.
		AkUInt32		m_uAllocs;				// Allocation stats.
		AkUInt32		m_uFrees;
		AkUInt32		m_uPeakUsed;
#endif
	};
}
}

#endif // _AK_IO_MEM_MGR_H_
