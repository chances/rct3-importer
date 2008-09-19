/* rifffile.cpp

	Copyright (c) 1996, 1988 by Timothy J. Weber.

	See rifffile.txt for documentation.
*/

#include "rifffile.h"

using namespace std;

/***************************************************************************
	macros and constants
***************************************************************************/

// define REVERSE_ENDIANISM if the endianism of the host platform is not Intel
// (Intel is little-endian)
#ifdef REVERSE_ENDIANISM
 #define SWAP_32(int32) (  \
	((((DWORD) int32) & 0x000000FFL) << 24) +  \
	((((DWORD) int32) & 0x0000FF00L) << 8) +  \
	((((DWORD) int32) & 0x00FF0000L) >> 8) +  \
	((((DWORD) int32) & 0xFF000000L) >> 24))
#endif

struct TypeRecord {
	const char* typeName;  // four-letter name
	const char* realName;  // English name
};

const int numExtraTypes = 24;
const TypeRecord extraTypes[numExtraTypes] = {
	{ "DISP", "Display name" },
	{ "IARL", "Archival location" },
	{ "IART", "Artist" },
	{ "ICMS", "Commissioned" },
	{ "ICMT", "Comments" },
	{ "ICOP", "Copyright" },
	{ "ICRD", "Creation date" },
	{ "ICRP", "Cropped" },
	{ "IDIM", "Dimensions" },
	{ "IDPI", "Dots Per Inch" },
	{ "IENG", "Engineer" },
	{ "IGNR", "Genre" },
	{ "IKEY", "Keywords" },
	{ "ILGT", "Lightness" },
	{ "IMED", "Medium" },
	{ "INAM", "Name" },
	{ "IPLT", "Palette Setting" },
	{ "IPRD", "Product" },
	{ "ISBJ", "Subject" },
	{ "ISFT", "Software" },
	{ "ISHP", "Sharpness" },
	{ "ISRC", "Source" },
	{ "ISRF", "Source Form" },
	{ "ITCH", "Technician" },
};

/***************************************************************************
	typedefs and class definitions
***************************************************************************/

/***************************************************************************
	prototypes for static functions
***************************************************************************/

/***************************************************************************
	static variables
***************************************************************************/

/***************************************************************************
	member functions for RiffFile
***************************************************************************/

RiffFile::RiffFile(const char *name):
    is(&fb)
{
    fb.open(name, ios::in);
	if (is.good() && !rewind()) {
		fb.close();
	}
}

/** @brief RiffFile
  *
  * @todo: document this function
  */
 RiffFile::RiffFile(std::streambuf *buffer):
    is(buffer)
{
    rewind();
}

RiffFile::~RiffFile()
{
	if (fb.is_open())
		 fb.close();
}

bool RiffFile::rewind()
{
	// clear the chunk stack
	while (!chunks.empty())
		chunks.pop();

	// rewind to the start of the file
	is.seekg(0, ios::beg);
	if (is.fail() || is.bad())
		return false;

	// look for a valid RIFF header
	RiffChunk topChunk(*this);

	if (is.eof() || strcmp(topChunk.name, "RIFF"))
		return false;

	// found; push it on the stack, and leave the put pointer in the same place
	// as the get pointer.
	formSize = topChunk.size;
	chunks.push(topChunk);
	return true;
}

bool RiffFile::push(const char* chunkType)
{
	// can't descend if we haven't started out yet.
	if (chunks.empty())
		return false;

	// first, go to the start of the current chunk, if we're looking for a named
	// chunk.
	if (chunkType) {
	    is.seekg(chunks.top().start, ios::beg);
		if (is.bad() || is.fail())
			return false;
	}

	// read chunks until one matches or we exhaust this chunk
	while (!is.eof() && is.tellg() < chunks.top().after) {
		RiffChunk chunk(*this);

		if (!is.eof()) {
			// see if the subchunk type matches
			if (!chunkType || strcmp(chunk.name, chunkType) == 0) {
				// found; synchronize the put pointer, push the chunk, and succeed
				chunks.push(chunk);
				return true;
			} else {
				// not found; go to the next one.
				is.seekg(chunk.after, ios::beg);
				if (is.fail() || is.bad())
					return false;
			}
		}
	}

	// couldn't find it; synchronize the put pointer and return error.
	is.seekg(chunks.top().start, ios::beg);
	return false;
}

bool RiffFile::pop()
{
	// if we've only got the top level chunk (or not even that), then we can't
	// go up.
	if (chunks.size() < 2)
		return false;

	// Position the get and put pointers at the end of the current subchunk.
	is.seekg(chunks.top().after, ios::beg);

	// Pop up the stack.
	chunks.pop();
	return true;
}

long RiffFile::chunkSize() const
{
	if (!chunks.empty())
		return chunks.top().size;
	else
		return 0;
}

const char* RiffFile::chunkName() const
{
	if (!chunks.empty())
		return chunks.top().name;
	else
		return 0;
}

const char* RiffFile::subType() const
{
	if (!chunks.empty() && chunks.top().subType[0])
		return chunks.top().subType;
	else
		return 0;
}

bool RiffFile::getNextExtraItem(string& type, string& value)
{
	// if the current chunk is LIST/INFO, then try to read another subchunk.
	if (strcmp(chunkName(), "LIST") == 0
		&& strcmp(subType(), "INFO") == 0)
	{
		if (push()) {
			if (readExtraItem(type, value))
				return true;
			else
				// unrecognized type.  Continue on.
				return getNextExtraItem(type, value);
		} else {
			// got to the end of the LIST/INFO chunk.  Pop back out and continue
			// looking.
			pop();
			return getNextExtraItem(type, value);
		}
	// we're not in a LIST/INFO chunk, so look for the next DISP or LIST/INFO.
	} else {
		push();

		if (strcmp(chunkName(), "DISP") == 0) {
			// DISP chunk: read and pop back out.
			return readExtraItem(type, value);
		} else if (strcmp(chunkName(), "LIST") == 0
			&& strcmp(subType(), "INFO") == 0)
		{
			// LIST/INFO chunk: read first element
			return getNextExtraItem(type, value);
		} else {
			// Some other chunk.  Pop back out and move on.
			if (pop())
				return getNextExtraItem(type, value);
			else
				return false;
		}
	}
}

// Reads extra data from the current chunk, and pops out of it.
bool RiffFile::readExtraItem(string& type, string& value)
{
	// see if it's one we recognize
	bool found = false;
	for (int i = 0; i < numExtraTypes; i++) {
		if (strcmp(chunkName(), extraTypes[i].typeName) == 0) {
			type = extraTypes[i].realName;
			found = true;
		}
	}

	// DISP chunks skip four bytes before the display name starts.
	if (strcmp(chunkName(), "DISP") == 0) {
		is.get();
		is.get();
		is.get();
		is.get();
	}

	// read the value, if we recognize the type
	if (found) {
		int c;
		value = "";
		while (!is.eof() && ((c = is.get()) != '\0'))
			value += char(c);
	}

	// whether we recognize it or not, pop back out.
	pop();
	return found;
}

/***************************************************************************
	member functions for RiffChunk
***************************************************************************/

RiffChunk::RiffChunk(RiffFile& parent)
{
	// read the chunk name
	parent.stream().read(name, 4);
	name[4] = '\0';

	// read the chunk size
	parent.stream().read(reinterpret_cast<char*>(&size), 4);

#ifdef REVERSE_ENDIANISM
	// reverse the endianism of the chunk size.
	size = SWAP_32(size);
#endif

	// if this is a RIFF or LIST chunk, read its subtype.
	if (strcmp(name, "RIFF") == 0
		|| strcmp(name, "LIST") == 0)
	{
        parent.stream().read(subType, 4);
		subType[4] = '\0';

		// subtract the subtype from the size of the data.
		size -= 4;
	} else
		*subType = '\0';

	// the chunk starts after the name and size.
	start = parent.stream().tellg();

	// the next chunk starts after this one, but starts on a word boundary.
	after = start + size;
	if (after % 2)
		after++;
}

/***************************************************************************
	main()
***************************************************************************/

#ifdef TEST_RIFFFILE

#include <iostream>

static void reportProblem()
{
	cout << "  *** ERROR: Result incorrect." << endl;
}

static void checkResult(bool got, bool expected)
{
	if (got)
		cout << "success." << endl;
	else
		cout << "fail." << endl;

	if (got != expected)
		reportProblem();
}

static void pause()
{
	cout << "Press Enter to continue." << endl;
	cin.get();
}

static void showChunk(RiffFile& file, int indent, bool expandLists)
{
	for (int i = 0; i < indent; i++)
		cout << ' ';

	cout << "Chunk type: " << file.chunkName();

	if (file.subType())
		cout << " (" << file.subType() << ")";

	cout << ", " << file.chunkSize() << " bytes" << endl;

	// show all subchunks
	if (strcmp(file.chunkName(), "RIFF") == 0
		|| (expandLists && strcmp(file.chunkName(), "LIST") == 0))
	{
		while (file.push()) {
			showChunk(file, indent + 2, expandLists);
			file.pop();
		}
	}
}

int main(int argc, const char* argv[])
{
	if (argc == 1)
		cout << "Lists the chunks in a named RIFF file." << endl;
	else {
		RiffFile file(argv[1]);

		if (!file.filep()) {
			cout << "No RIFF form in file " << argv[1] << "." << endl;
		} else {
			cout << "Without expanding lists:" << endl;
			showChunk(file, 0, false);

			cout << endl << "Expanding lists:" << endl;
			showChunk(file, 0, true);
		}
	}

	return 0;
}

#endif
