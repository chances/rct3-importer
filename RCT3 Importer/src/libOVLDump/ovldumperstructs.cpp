
#include "ovldumperstructs.h"

#include "OVLDException.h"

void OvlSymbol::SetData(unsigned long data) {
    if (IsPointer)
        throw EOvlD("Tried to set data for a pointer symbol.");

    orgsymbol->data = reinterpret_cast<unsigned long *>(data);
}

void OvlStringTableEntry::SetString(const char* newstr) const {
    unsigned long len = strlen(newstr);
    if (len > orglength)
        throw EOvlD("Tried to set string: too long.");

    if (len != orglength) {
        for (unsigned long l = 0; l < orglength; ++l)
            orgstring[l] = 0;
    }
    strcpy(orgstring, newstr);
}
