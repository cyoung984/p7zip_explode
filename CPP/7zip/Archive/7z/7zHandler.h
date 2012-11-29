// 7z/Handler.h

#ifndef __7Z_HANDLER_H
#define __7Z_HANDLER_H

#include "../../ICoder.h"
#include "../IArchive.h"

#include "../../Common/CreateCoder.h"

#ifndef EXTRACT_ONLY
#include "../Common/HandlerOut.h"
#endif

#include "7zCompressionMode.h"
#include "7zIn.h"

namespace NArchive {
namespace N7z {
#ifndef __7Z_SET_PROPERTIES

#ifdef EXTRACT_ONLY
#if !defined(_7ZIP_ST) && !defined(_SFX)
#define __7Z_SET_PROPERTIES
#endif
#else
#define __7Z_SET_PROPERTIES
#endif

#endif
	// todo: refactor
	class CSzTree;
	struct szExplodeData
	{
		CArchiveDatabase newDatabase;
		CRecordVector<UInt64> folderSizes, folderPositions;
	};
class CHandler:
  #ifndef EXTRACT_ONLY
  public NArchive::COutHandler,
  #endif
  public IInArchive,
  #ifdef __7Z_SET_PROPERTIES
  public ISetProperties,
  #endif
  #ifndef EXTRACT_ONLY
  public IOutArchive,
  #endif
  PUBLIC_ISetCompressCodecsInfo
  public CMyUnknownImp
{
public:
  MY_QUERYINTERFACE_BEGIN2(IInArchive)
  #ifdef __7Z_SET_PROPERTIES
  MY_QUERYINTERFACE_ENTRY(ISetProperties)
  #endif
  #ifndef EXTRACT_ONLY
  MY_QUERYINTERFACE_ENTRY(IOutArchive)
  #endif
  QUERY_ENTRY_ISetCompressCodecsInfo
  MY_QUERYINTERFACE_END
  MY_ADDREF_RELEASE

  INTERFACE_IInArchive(;)

  #ifdef __7Z_SET_PROPERTIES
  STDMETHOD(SetProperties)(const wchar_t **names, const PROPVARIANT *values, Int32 numProperties);
  #endif

  #ifndef EXTRACT_ONLY
  INTERFACE_IOutArchive(;)
  #endif

  DECL_ISetCompressCodecsInfo

  CHandler();

  // Explode the database into one database per folder.
  void Explode(CObjectVector<szExplodeData>& exploded, const UInt64 maxDepth);

private:
	
	void Explode(CSzTree* tree, CObjectVector<szExplodeData>& exploded, 
		UInt64 maxdepth, 
		szExplodeData* szExplode = NULL, UInt64 curDepth = 0);

	void AddFolderToDatabase(CArchiveDatabaseEx& input, int folderIndex,
		szExplodeData& out);
	void AddBlocksToDatabase(szExplodeData& out, CSzTree* tree);

  
private:
  CMyComPtr<IInStream> _inStream;
  NArchive::N7z::CArchiveDatabaseEx _db;
  
  #ifndef _NO_CRYPTO
  bool _passwordIsDefined;
  #endif

  #ifdef EXTRACT_ONLY
  
  #ifdef __7Z_SET_PROPERTIES
  UInt32 _numThreads;
  #endif

  UInt32 _crcSize;

  #else
  
  CRecordVector<CBind> _binds;

  HRESULT SetCompressionMethod(CCompressionMethodMode &method,
      CObjectVector<COneMethodInfo> &methodsInfo
      #ifndef _7ZIP_ST
      , UInt32 numThreads
      #endif
      );
public: // todo: was private
  HRESULT SetCompressionMethod(
      CCompressionMethodMode &method,
      CCompressionMethodMode &headerMethod);

  #endif

  bool IsEncrypted(UInt32 index2) const;
  #ifndef _SFX

  CRecordVector<UInt64> _fileInfoPopIDs;
  void FillPopIDs();

  #endif

  DECL_EXTERNAL_CODECS_VARS
};

}}

#endif
