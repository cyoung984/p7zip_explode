// DataChecksum.h

#ifndef __DATACHECKSUM_H
#define __DATACHECKSUM_H

#include "Common/Wildcard.h"
#include "../Common/LoadCodecs.h"

HRESULT DataChecksumArchives(CCodecs *codecs, const CIntVector &formatIndices,
	bool stdInMode,
	UStringVector &arcPaths, UStringVector &arcPathsFull, UInt64 &numErrors);

#endif

