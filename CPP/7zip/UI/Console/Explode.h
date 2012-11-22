// Explode.h

#ifndef __EXPLODE_H
#define __EXPLODE_H

#include "Common/Wildcard.h"
#include "../Common/LoadCodecs.h"

HRESULT ExplodeArchives(CCodecs *codecs, const CIntVector &formatIndices,
	bool stdInMode,
	UStringVector &archivePaths, UStringVector &archivePathsFull,
	UInt64 &errors);

#endif

