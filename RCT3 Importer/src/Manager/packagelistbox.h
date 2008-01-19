#ifndef PACKAGELISTBOX_H_INCLUDED
#define PACKAGELISTBOX_H_INCLUDED

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "colhtmllbox.h"

struct wxNameComp {
    bool operator() (const wxString& lhs, const wxString& rhs) const {
        return wxNameComp::Compare(lhs, rhs) < 0;
    }
    static int Compare (const wxString& lhs, const wxString& rhs) {
        return lhs.CmpNoCase(rhs);
    }
};

class wxPackageListBox: public wxColourHtmlListBox {
public:
    enum {
        TYPE_AUTHORLIST = 1,
        TYPE_SHORTNAME  = 2,
        TYPE_ALL        = 4,
        TYPE_INITSHOW   = 8
    };
private:
    class wxPackageListBoxEntry {
    public:
        wxString name;
        wxString guid;

        wxPackageListBoxEntry(const wxString& pname, const wxString& pguid): name(pname), guid(pguid) {};
        virtual wxString GetDisplay(int settings) const = 0;
    };
    class wxPackageListBoxPackage: public wxPackageListBoxEntry {
    public:
        wxString shortname;
        wxArrayString authors;
        wxString version;
        bool official;
        bool installed;
        bool present;

        wxPackageListBoxPackage(const wxString& pname, const wxString& pguid, const wxString& pshortname,
                                const wxString& pversion, bool pofficial, bool pinstalled, bool ppresent):
                                wxPackageListBoxEntry(pname, pguid), shortname(pshortname), version(pversion), official(pofficial),
                                installed(pinstalled), present(ppresent) {};
        void AddAuthor(const wxString& author) {
            authors.push_back(author);
        };
        wxString GetName(int settings) const;
        wxString GetAuthors() const;
        virtual wxString GetDisplay(int settings) const;
        wxString GetShortDisplay(int settings) const;
    };
    class wxPackageListBoxAuthor: public wxPackageListBoxEntry {
    public:
        bool display;
        std::vector< boost::shared_ptr<wxPackageListBoxPackage> > packages;

        wxPackageListBoxAuthor(const wxString& pname, const wxString& pguid, bool pdisplay):
                               wxPackageListBoxEntry(pname, pguid), display(pdisplay) {};
        inline void AddPackage(const boost::shared_ptr<wxPackageListBoxPackage>& package) {
            packages.push_back(package);
        }
        void SortPackages(int settings);
        virtual wxString GetDisplay(int settings) const;
    };
    struct wxPackageComp {
        bool shortname;
        wxPackageComp(bool pshortname = false): shortname(pshortname) {}
        bool operator() (const boost::shared_ptr<wxPackageListBoxPackage>& lhs, const boost::shared_ptr<wxPackageListBoxPackage>& rhs) const {
            if (shortname)
                return wxNameComp::Compare(((lhs->shortname.IsEmpty())?lhs->name:lhs->shortname), ((rhs->shortname.IsEmpty())?rhs->name:rhs->shortname)) < 0;
            else
                return wxNameComp::Compare(lhs->name, rhs->name)<0;
        }
    };

    std::vector< boost::shared_ptr<wxPackageListBoxEntry> > m_displaylist;
    std::map< wxString, boost::shared_ptr<wxPackageListBoxAuthor>, wxNameComp > m_authorlist;
    int settings;

    void AssignList();
protected:
    virtual wxString OnGetItem(size_t n) const;
    void OnListCellClicked(wxHtmlCellEvent& event);
public:
    wxPackageListBox(wxWindow* parent);
    virtual ~wxPackageListBox();
    void SetSettings(int psettings) {
        settings = psettings;
    }
    void RefreshList();
};

#endif // PACKAGELISTBOX_H_INCLUDED
