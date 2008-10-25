///////////////////////////////////////////////////////////////////////////////
//
// raw ovl xml interpreter
// Command line ovl creation utility
// Copyright (C) 2007 Tobias Minch
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   Tobias Minich - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////



#include "RawParse_cpp.h"

#include "ManagerSND.h"

#include "wave\WAVE.h"
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#define RAWXML_SND_SOUND            "sound"

void cRawParser::ParseSND(cXmlNode& node) {
    USE_PREFIX(node);
    wxString name = ParseString(node, wxT(RAWXML_SND), wxT("name"), NULL, useprefix);
//    wxFileName texture = ParseString(node, RAWXML_TEX, wxT("texture"));
//    if (!texture.IsAbsolute())
//        texture.MakeAbsolute(m_input.GetPathWithSep());
    wxLogVerbose(wxString::Format(_("Adding snd %s to %s."), name.c_str(), m_output.GetFullPath().c_str()));

    cSound sound;
    sound.name = name.ToAscii();
    OPTION_PARSE(unsigned long, sound.loop, ParseUnsigned(node, wxT(RAWXML_SND), wxT("loop")));

    cXmlNode child(node.children());
    while (child) {
        DO_CONDITION_COMMENT(child);

        if (child(RAWXML_SND_SOUND)) {

            cXmlNode datanode(child.children());
            while (datanode && (!datanode(RAWXML_DATA)))
                datanode.go_next();

            if (!datanode)
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Tag snd(%s)/sound misses data."), name.c_str()), child);

            cRawDatablock data = GetDataBlock(wxString::Format(_("tag snd(%s)/sound"), name.c_str()), datanode);
            if (data.datatype().IsEmpty()) {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Could not determine data type in tag snd(%s)/sound."), name.c_str()), child);
            } else if (data.datatype().IsSameAs(wxT("processed"))) {
                throw MakeNodeException<RCT3Exception>(wxString::Format(_("Processed data not supported in tag snd(%s)/sound."), name.c_str()), child);
            } else {
                WaveFile wave;
                boost::iostreams::array_source wavedata(reinterpret_cast<char*>(data.data().get()), data.datasize());
                boost::iostreams::stream_buffer<boost::iostreams::array_source> wavebuf(wavedata);
                if (!wave.OpenRead(&wavebuf)) {
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Failed to open sound data in tag snd(%s)/sound."), name.c_str()), child);
                }
                if ((wave.GetBitsPerChannel() != 16) || (wave.GetNumChannels() != 1) || (wave.GetFormatType() != 1))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Sound data in tag snd(%s)/sound is not 16 bit / mono / uncompressed."), name.c_str()), child);
                sound.format.nSamplesPerSec = wave.GetSampleRate();
                sound.format.nAvgBytesPerSec = wave.GetBytesPerSecond();
                sound.channel1_size = wave.GetDataLength();
                sound.channel1.reset(new int16_t[wave.GetNumSamples()]);
                if (!wave.ReadSamples(sound.channel1.get(), wave.GetNumSamples()))
                    throw MakeNodeException<RCT3Exception>(wxString::Format(_("Failed to read sound data in tag snd(%s)/sound."), name.c_str()), child);
            }
        } else if (child.element()) {
            throw MakeNodeException<RCT3Exception>(wxString::Format(_("Unknown tag '%s' in snd(%s) tag."), child.wxname().c_str(), name.c_str()), child);
        }
        child.go_next();
    }

    if (m_mode != MODE_BAKE) {
        ovlSNDManager* c_snd = m_ovl.GetManager<ovlSNDManager>();
        c_snd->AddItem(sound);
    }
}
