#include "StdAfx.h"

#include "DataChecksum.h"

#include "Windows/Error.h"
#include "../../Archive/IArchive.h"
#include "../Common/OpenArchive.h"
#include "OpenCallbackConsole.h"
#include "../../Archive/7z/7zItem.h"
#include "../../Archive/7z/7zOut.h"
#include "../../Archive/7z/7zHandler.h"
#include "../../Common/FileStreams.h"
#include "../../Common/LimitedStreams.h"
#include "../../Compress/CopyCoder.h"
#include "../../../Windows/FileDir.h"
#include "../../Archive/Common/OutStreamWithCRC.h"
#include "../../../Common/IntToString.h"

using namespace NWindows;

#define SHOW_ERROR(x) g_StdOut << endl << "Error: " << archivePath << ": " \
	<< x << endl; numErrors++; 

static HRESULT WriteRange(IInStream *inStream, ISequentialOutStream *outStream,
	UInt64 position, UInt64 size, ICompressProgressInfo *progress)
{
	RINOK(inStream->Seek(position, STREAM_SEEK_SET, 0));
	CLimitedSequentialInStream *streamSpec = new CLimitedSequentialInStream;
	CMyComPtr<CLimitedSequentialInStream> inStreamLimited(streamSpec);
	streamSpec->SetStream(inStream);
	streamSpec->Init(size);

	NCompress::CCopyCoder *copyCoderSpec = new NCompress::CCopyCoder;
	CMyComPtr<ICompressCoder> copyCoder = copyCoderSpec;
	RINOK(copyCoder->Code(inStreamLimited, outStream, NULL, NULL, progress));
	return (copyCoderSpec->TotalSize == size ? S_OK : E_FAIL);
}

HRESULT DataChecksumArchives(CCodecs *codecs, const CIntVector &formatIndices,
	bool stdInMode,
	UStringVector &arcPaths, UStringVector &arcPathsFull, UInt64 &numErrors)
{
	int numArcs = arcPaths.Size();
	for (int i = 0; i < numArcs; i++)
	{
		UString archivePath = arcPaths[i];

		UInt64 arcPackSize = 0;
		if (!stdInMode)
		{
			NFile::NFind::CFileInfoW fi;
			if (!fi.Find(archivePath) || fi.IsDir())
			{
				SHOW_ERROR("is not a file.");
				continue;
			}
			arcPackSize = fi.Size;
		}

		g_StdOut << endl << "Checking : " << archivePath << endl << endl;

		CArchiveLink archiveLink;

		COpenCallbackConsole openCallback;
		openCallback.OutStream = &g_StdOut;

#ifndef _NO_CRYPTO
		openCallback.PasswordIsDefined = false;
#endif

		HRESULT result = archiveLink.Open2(codecs, formatIndices, stdInMode, NULL, archivePath, &openCallback);
	
		if (result != S_OK)
		{
			if (result == E_ABORT)
				return result;
			else if (result == S_FALSE) {
				SHOW_ERROR("Can not open file as archive");
			} else if (result == E_OUTOFMEMORY) {
				SHOW_ERROR("Can't allocate required memory");
			}
			else {
				SHOW_ERROR(NError::MyFormatMessage(result));
			}
			continue;
		}

		// multi-volume isn't supported
		if (archiveLink.VolumePaths.Size() != 1) {
			SHOW_ERROR("Checking multi-volume archives isn't supported.");
			continue;
		}

		bool szArchive = true;
		for (int x = 0; x < archiveLink.Arcs.Size(); x++)
		{
			const UString szName = L"7z";
			const CArc &arc = archiveLink.Arcs[x];
			if (codecs->Formats[arc.FormatIndex].Name != szName) {
				szArchive = false;
				break;
			}
		}

		if (!szArchive) {
			SHOW_ERROR("Only 7z archives can be checked.");
			continue;
		}

		// Only 7z is supported, and it's been checked
		using namespace NArchive::N7z;
		IInArchive* inArc = archiveLink.GetArchive();
		CHandler* szHandler = (CHandler*)inArc;
		CArchiveDatabaseEx db = szHandler->GetArchiveDatabase();

		// Open the file so we can checksum its data
		CInFileStream* _inStream = new CInFileStream;
		CMyComPtr<CInFileStream> inStream(_inStream);
		if (!inStream->Open(archivePath)) {
			SHOW_ERROR("Cannot be opened for reading.");
			continue;
		}	
		// Checksum every block
		for (int x = 0; x < db.Folders.Size(); x++) {		
			CMyComPtr<COutStreamWithCRC> crcStream(new COutStreamWithCRC());
			crcStream->Init();
			UInt64 folderLen = db.GetFolderFullPackSize(x);
			UInt64 folderStartPackPos = db.GetFolderStreamPos(x,0);
			RINOK(WriteRange(inStream, crcStream, 
				folderStartPackPos, folderLen, NULL));
			
			char crc_str[9];
			ConvertUInt32ToHexWithZeros(crcStream->GetCRC(), crc_str);
			//g_StdOut << "Block " << x << " CRC : " << crc_str << endl;
			g_StdOut << crc_str << endl;
		}		

		archiveLink.Close();
	}

	return S_OK;
}