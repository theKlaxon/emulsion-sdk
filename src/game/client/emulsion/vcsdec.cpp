#include "cbase.h"
#include "lzmaDecoder.h"
#include "filesystem.h"

CON_COMMAND(vcsdec, "decode a vcs file in platform/shaders")
{
	CLZMA lzma;
	CUtlBuffer fbuf(4096, 1024 * 1024, CUtlBuffer::READ_ONLY);

	if (!g_pFullFileSystem->ReadFile("shaders/fxc/lightmappedpaint_ps20b.vcs", NULL, fbuf))
		return;

	CUtlBuffer buf = CUtlBuffer(fbuf.PeekGet(0x1B), fbuf.Size());

	// seek to the lzma header (under the ShaderHeader_t in .vcs)
	//buf.SeekGet(CUtlBuffer::SeekType_t::SEEK_HEAD, 0x1B);

	int id = LZMA_ID;
	lzma_header_t* pHeader = (lzma_header_t*)((unsigned char*)buf.Base());

	int originalSize = pHeader->actualSize;//lzma.GetActualSize((unsigned char*)buf.Base());
	unsigned char* pOriginalData = new unsigned char[originalSize];

	lzma.UncompressVCS((unsigned char*)buf.Base(), pOriginalData);
	buf.AssumeMemory(pOriginalData, originalSize, originalSize, CUtlBuffer::READ_ONLY);

	int k = 0;
}