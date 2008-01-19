
#include "packagelistbox.h"

#include <algorithm>
#include <set>

#include "htmlentities.h"
#include "managerapp.h"
#include "managerresourcefiles.h"

wxString wxPackageListBox::wxPackageListBoxPackage::GetName(int settings) const {
    wxString ret;
    if ((settings & TYPE_SHORTNAME) && (!shortname.IsEmpty())) {
        ret = shortname;
    } else {
        ret = name;
    }
    if (!version.IsSameAs(wxT("0")))
        ret += wxT(" V") + version;
    return wxEncodeHtmlEntities(ret);
};

wxString wxPackageListBox::wxPackageListBoxPackage::GetAuthors() const {
    switch (authors.size()) {
        case 0:
            return wxT("");
        case 1:
            return wxEncodeHtmlEntities(authors[0]);
        case 2:
            return wxEncodeHtmlEntities(authors[0] + wxT(" & ") + authors[1]);
    }
    // More than 2
    int i;
    wxString ret;
    for (i = 0; i < authors.size() - 2; ++i) {
        ret += authors[i] + wxT(", ");
    }
    ret += authors[i] + wxT(" & ") + authors[i+1];
    return wxEncodeHtmlEntities(ret);
}

wxString wxPackageListBox::wxPackageListBoxPackage::GetDisplay(int settings) const {
    wxString ret;
    if (!present)
        ret += wxT("<font color=#666666>");
    if (settings & TYPE_AUTHORLIST) {
        ret += HTML_INSET_START;
        ret += GetName(settings);
        if (authors.size() > 1)
            ret += _(" <i>by</i> ") + GetAuthors();
        ret += HTML_INSET_END;
    } else {
        ret += GetName(settings);
        ret += _(" <i>by</i> ") + GetAuthors();
    }
    if (!present)
        ret += wxT("</font>");
    return ret;
};

wxString wxPackageListBox::wxPackageListBoxPackage::GetShortDisplay(int settings) const {
    wxString ret;
    if (!present)
        ret += wxT("<font color=#666666>");
    ret += GetName(settings);
    if (!present)
        ret += wxT("</font>");
    return ret;
};

void wxPackageListBox::wxPackageListBoxAuthor::SortPackages(int settings) {
    std::sort(packages.begin(), packages.end(), wxPackageComp(settings & TYPE_SHORTNAME));
}

wxString wxPackageListBox::wxPackageListBoxAuthor::GetDisplay(int settings) const {
    wxString ret;
    if ((!display) && (packages.size())) {
        ret += wxString::Format(wxT("<img align=bottom id='%s' width=10 height=10 align=center src='") MEMORY_PREFIX RES_IMAGE_PLUS wxT("'>&nbsp;"), guid.c_str());
        ret += wxT("<b>") + wxEncodeHtmlEntities(name) + wxT("</b>");
        ret += HTML_INSET_START wxT("<font size=-2>");
        int i;
        for (i = 0; i < packages.size() - 1; ++i) {
            ret += packages[i]->GetShortDisplay(settings) + wxT(", ");
        }
        ret += packages[i]->GetShortDisplay(settings);
        ret += wxT("</font>") HTML_INSET_END;
    } else {
        ret += wxString::Format(wxT("<img align=bottom id='%s' width=10 height=10 align=center src='") MEMORY_PREFIX RES_IMAGE_MINUS wxT("'>&nbsp;"), guid.c_str());
        ret += wxT("<b>") + wxEncodeHtmlEntities(name) + wxT("</b>");
    }
    return ret;
};

wxPackageListBox::wxPackageListBox(wxWindow* parent): wxColourHtmlListBox(parent), settings(0) {
	this->Connect( wxEVT_COMMAND_HTML_CELL_CLICKED, wxHtmlCellEventHandler( wxPackageListBox::OnListCellClicked ) );
}

wxPackageListBox::~wxPackageListBox() {
	this->Disconnect( wxEVT_COMMAND_HTML_CELL_CLICKED, wxHtmlCellEventHandler( wxPackageListBox::OnListCellClicked ) );
}

void wxPackageListBox::OnListCellClicked(wxHtmlCellEvent& event) {
    wxString guid = event.GetCell()->GetId();
    if (!guid.IsEmpty()) {
        for (std::map< wxString, boost::shared_ptr<wxPackageListBoxAuthor> >::iterator it = m_authorlist.begin(); it != m_authorlist.end(); ++it) {
            if (it->second->guid == guid) {
                it->second->display = !it->second->display;
                break;
            }
        }
        AssignList();
    }
}

void wxPackageListBox::AssignList() {
    if (settings & TYPE_AUTHORLIST) {
        int l = GetFirstVisibleLine();
        m_displaylist.clear();
        for (std::map< wxString, boost::shared_ptr<wxPackageListBoxAuthor> >::iterator it = m_authorlist.begin(); it != m_authorlist.end(); ++it) {
            m_displaylist.push_back(it->second);
            it->second->SortPackages(settings);
            if (it->second->display) {
                for (std::vector< boost::shared_ptr<wxPackageListBoxPackage> >::iterator itp = it->second->packages.begin(); itp != it->second->packages.end(); ++itp) {
                    m_displaylist.push_back(*itp);
                }
            }
        }
        //m_displaylist.insert(0, tmplist.begin(), tmplist.end());
        Freeze();
        SetItemCount(m_displaylist.size());
        RefreshAll();
        if (l >= 0)
            ScrollToLine(l);
        Thaw();
    }
}

void wxPackageListBox::RefreshList() {
    m_authorlist.clear();
    m_displaylist.clear();
    if (settings & TYPE_AUTHORLIST) {

        wxSQLite3ResultSet fs = ::wxGetApp().GetManagerDB().ExecuteQuery(wxT("select name, guid, version, official, shortname from packagelist;"));
        while (fs.NextRow()) {
            wxString guid = fs.GetString(1);
            boost::shared_ptr<wxPackageListBoxPackage> p(new wxPackageListBoxPackage(fs.GetString(0), guid, fs.GetString(4), fs.GetString(2), fs.GetBool(3), false, false));
            wxSQLite3StatementBuffer buf;
            buf.Format("select a.name, a.guid from packagelist_authors as p join authorlist as a on p.authorguid = a.guid where p.guid = '%q';", guid.mb_str(wxConvUTF8).data());
            wxSQLite3ResultSet fsa = ::wxGetApp().GetManagerDB().ExecuteQuery(buf);
            while (fsa.NextRow()) {
                wxString author = fsa.GetString(0);
                p->AddAuthor(author);
                std::map< wxString, boost::shared_ptr<wxPackageListBoxAuthor> >::iterator au = m_authorlist.find(author);
                if (au == m_authorlist.end()) {
                    std::pair<std::map< wxString, boost::shared_ptr<wxPackageListBoxAuthor> >::iterator, bool> ret;
                    boost::shared_ptr<wxPackageListBoxAuthor> newauthor(new wxPackageListBoxAuthor(author, fsa.GetString(1), settings & TYPE_INITSHOW));
                    ret = m_authorlist.insert(std::pair< wxString, boost::shared_ptr<wxPackageListBoxAuthor> >(author, newauthor));
                    au = ret.first;
                }
                au->second->AddPackage(p);
            }
            p->authors.Sort(wxNameComp::Compare);
        }

        AssignList();
    } else {
        std::set< boost::shared_ptr<wxPackageListBoxPackage>, wxPackageComp > tmplist(wxPackageComp(settings & TYPE_SHORTNAME));

        wxSQLite3ResultSet fs = ::wxGetApp().GetManagerDB().ExecuteQuery(wxT("select name, guid, version, official, shortname from packagelist;"));
        while (fs.NextRow()) {
            wxString guid = fs.GetString(1);
            boost::shared_ptr<wxPackageListBoxPackage> p(new wxPackageListBoxPackage(fs.GetString(0), guid, fs.GetString(4), fs.GetString(2), fs.GetBool(3), false, false));
            wxSQLite3StatementBuffer buf;
            buf.Format("select a.name from packagelist_authors as p join authorlist as a on p.authorguid = a.guid where p.guid = '%q';", guid.mb_str(wxConvUTF8).data());
            wxSQLite3ResultSet fsa = ::wxGetApp().GetManagerDB().ExecuteQuery(buf);
            while (fsa.NextRow()) {
                p->AddAuthor(fsa.GetString(0));
            }
            p->authors.Sort(wxNameComp::Compare);
            tmplist.insert(p);
        }

        m_displaylist.clear();
        m_displaylist.resize(tmplist.size());
        std::copy(tmplist.begin(), tmplist.end(), m_displaylist.begin());
//        for (std::set< boost::shared_ptr<wxPackageListBoxPackage> >::iterator it = tmplist.begin(); it != tmplist.end(); ++it)
//            m_displaylist.push_back(*it);
        Freeze();
        SetItemCount(m_displaylist.size());
        RefreshAll();
        Thaw();
    }
}

wxString wxPackageListBox::OnGetItem(size_t n) const {
/*
    wxString ret = m_packages[n].name;
    if (m_packages[n].authors.size())
        ret += _("<i> by </i>") + m_packages[n].authors[0];
*/
    return m_displaylist[n]->GetDisplay(settings);
}

