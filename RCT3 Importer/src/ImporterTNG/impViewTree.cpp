///////////////////////////////////////////////////////////////////////////////
//
// RCT3 Importer TNG
// Custom object importer for Ataris Roller Coaster Tycoon 3
// Copyright (C) 2008 Belgabor (Tobias Minich)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, <http://www.gnu.org/licenses/>.
//
// Written by
//   Belgabor (Tobias Minich) - belgabor@gmx.de
//
///////////////////////////////////////////////////////////////////////////////

#include "impViewTree.h"

#include <string>

#include "cXmlXPath.h"
#include "cXmlXPathResult.h"
#include "xmldefs.h"

#include "impElement.h"
#include "impRawovlFileManager.h"
#include "importerxpaths.h"

using namespace xmlcpp;
using namespace std;

class impDataViewModel: public wxDataViewModel {
public:
    bool impItemAdded (wxDataViewItem parent, wxDataViewItem item) {
//        wxLogMessage(wxT("Item Added"));
        return ItemAdded(parent, item);
    }
    bool impItemChanged(wxDataViewItem item) {
//        wxLogMessage(wxT("Item Changed"));
        return ItemChanged(item);
    }
    bool impItemDeleted(wxDataViewItem parent, wxDataViewItem item) {
//        wxLogMessage(wxT("Item Deleted"));
        return ItemDeleted(parent, item);
    }
};

class impViewTreeModel;
class impViewTreeModelNode;
WX_DEFINE_ARRAY_PTR( impViewTreeModelNode*, impViewTreeModelNodes );

class impViewTreeModelNode: public impRawovlFileManagerBaseConnecter {
friend class impViewTreeModel;
public:
    impViewTreeModelNode( impRawovlFileManagerBaseConnecter* parent, impTypes::ELEMENTTYPE type, const wxString& guid = wxT("") ):
            m_parent(dynamic_cast<impViewTreeModelNode*>(parent)), m_type(type) {

        if (impElements[m_type].cattitle)
            m_title = wxGetTranslation(impElements[type].cattitle);

        if (impElements[m_type].role) {
            if (m_type == impTypes::ELEMENTTYPE_PROJECT)
                m_path = wxString::FromUTF8(impElements[m_type].path);
            else
                m_path = wxString::Format(wxT(XPATH_FORMAT_OVLID_GUID), wxString::FromUTF8(impElements[m_type].role).c_str(), guid.c_str());
        } else {
            m_path = wxString::FromUTF8(impElements[m_type].childrenpath);
        }
        if (m_parent) {
            itemChanged.connect(m_parent->itemChanged.make_slot());
            itemAdded.connect(m_parent->itemAdded.make_slot());
            itemDeleted.connect(m_parent->itemDeleted.make_slot());
        } else {
            impDataViewModel* mod = dynamic_cast<impDataViewModel*>(parent);
            if (mod) {
                itemChanged.connect(sigc::mem_fun(*mod, &impDataViewModel::impItemChanged));
                itemAdded.connect(sigc::mem_fun(*mod, &impDataViewModel::impItemAdded));
                itemDeleted.connect(sigc::mem_fun(*mod, &impDataViewModel::impItemDeleted));
            }
        }
        impRawovlFileManagerBaseConnecter::attachConnection(parent);
        update(wxT("/"));
//wxLogMessage(wxT("Created %d, path %s"), m_type, m_path.c_str());
    }

    ~impViewTreeModelNode() {
        clearChildren();
        itemDeleted(wxDataViewItem(m_parent), wxDataViewItem(this));
    }

    virtual bool IsContainer() {
        return m_children.size();
    }

    impViewTreeModelNode*  GetParent()                         { return m_parent; }
    impViewTreeModelNodes& GetChildren()                       { return m_children; }
    impViewTreeModelNode*  GetNthChild( unsigned int n )       { return m_children.Item( n ); }
    void Insert( impViewTreeModelNode* child, unsigned int n) { m_children.Insert( child, n); }
    void Append( impViewTreeModelNode* child )                { m_children.Add( child ); }
    unsigned int GetChildCount()                          { return m_children.GetCount(); }

    virtual wxString getName() {
        if (!m_title.IsEmpty())
            return m_title;

        cXmlXPath c_path = getXPath();
        if (c_path) {
            cXmlXPathResult res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_NAME);
            if (res.size()) {
                return UTF8STRINGWRAP(res[0].content());
            } else {
                res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_FILE);
                if (res.size())
                    return UTF8STRINGWRAP(res[0].content());
            }
        }

        return _("Unknown");
    }

    inline impTypes::ELEMENTTYPE getType() const { return m_type; }
    inline const wxString& getPath() const { return m_path; }

protected: // Signals
    sigc::signal<bool, wxDataViewItem> itemChanged;
    sigc::signal<bool, wxDataViewItem, wxDataViewItem> itemAdded;
    sigc::signal<bool, wxDataViewItem, wxDataViewItem> itemDeleted;
protected:
    wxString            m_title;
    wxString            m_path;
    impViewTreeModelNode   *m_parent;
    impViewTreeModelNodes   m_children;
    impTypes::ELEMENTTYPE m_type;

    void clearChildren() {
        size_t count = m_children.GetCount();
        size_t i;
        for (i = 0; i < count; ++i) {
            impViewTreeModelNode *child = m_children[i];
            delete child;
        }
        m_children.clear();
    }

    virtual void update(const wxString& value) {
        if (!getXml())
            return; // Escape if signals are not connected (root)

        if (m_title.IsEmpty()) {
            cXmlXPath c_path = getXPath();
            if (c_path) {
                cXmlXPathResult res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_NAME);
                if (res.size()) {
//wxLogMessage(wxT("Update '%s' via '%s'"), value.c_str(), res[0].wxpath().c_str());
                    if (value == res[0].wxpath())
                        itemChanged(wxDataViewItem(this));
                } else {
                    res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_FILE);
                    if (res.size())
                        if (value == res[0].wxpath())
                            itemChanged(wxDataViewItem(this));
                }
            }
        }

        if (impElements[m_type].children) {
            if (!m_children.size()) {
                const impTypes::ELEMENTTYPE* t = impElements[m_type].children;
                while (*t) {
                    impViewTreeModelNode* newnode = new impViewTreeModelNode(this, *t);
                    Append(newnode);
                    itemAdded(wxDataViewItem(this), wxDataViewItem(newnode));
                    ++t;
                }
                itemChanged(wxDataViewItem(this));
            } else {
                for (size_t i = 0; i < m_children.size(); ++i) {
                    m_children[i]->update(value);
                }
            }
        } else if (impElements[m_type].childrenpath) {
            vector<cXmlNode> nodes = getXPath()(m_path.utf8_str());

            for (size_t i = m_children.size(); i; --i) {
                if (!m_children[i-1]->update(value, nodes)) {
                    delete m_children[i-1];
                    m_children.erase(m_children.begin() + i - 1);
                }
            }

            if (nodes.size()) {
                for (vector<cXmlNode>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
                    wxString role = UTF8STRINGWRAP(it->getPropVal("role"));
                    wxString guid;
                    cXmlXPath c_nodepath = getXPath();
                    c_nodepath.setNodeContext(*it);
                    cXmlXPathResult noderes = c_nodepath(XPATH_ANY_GUID);
                    if (noderes.size()) {
                        guid = noderes[0].wxcontent();
                    } else {
                        noderes = c_nodepath(XPATH_ANY_METADATA);
                        guid = impRawovlFileManager::newGUID();
                        cXmlNode guidnode(XML_TAG_METADATA_GUID, WX2UTF8(guid), getXml().defaultNs());
                        if (noderes.size()) {
                            noderes[0].appendChildren(guidnode);
                        } else {
                            cXmlNode metanode(XML_TAG_METADATA, getXml().defaultNs());
                            metanode.prop("role", "importer");
                            metanode.appendChildren(guidnode);
                            it->insertNodeAsFirstChild(metanode);
                        }
                    }
                    Append(new impViewTreeModelNode(this, impElementRoles[string(role.utf8_str())], guid));

                }
                itemChanged(wxDataViewItem(this));
            }
        } else {
            for (size_t i = 0; i < m_children.size(); ++i) {
                m_children[i]->update(value);
            }
        }
    }

    virtual bool update(const wxString& value, vector<cXmlNode>& nodes) {
        cXmlNode me = getXPath()(m_path.utf8_str())[0];
        if (me) {
            bool found = false;
            for (vector<cXmlNode>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
                if (me == *it) {
                    nodes.erase(it);
                    found = true;
                    break;
                }
            }
            if (found) {
                update(value);
                return true;
            }
        }
        return false;
    }
};

/*
class impViewTreeModelNode: public sigc::trackable {
public:
    impViewTreeModelNode( impViewTreeModelNode* parent,
                      const wxString &title, const wxString &path ) {
        m_parent = parent;
        m_title = title;
        m_path = path;
        if (m_parent)
            itemChanged.connect(m_parent->itemChanged.make_slot());
    }

    ~impViewTreeModelNode() {
        clearChildren();
    }

    virtual bool IsContainer() {
        return false;
    }

    impViewTreeModelNode*  GetParent()                         { return m_parent; }
    impViewTreeModelNodes& GetChildren()                       { return m_children; }
    impViewTreeModelNode*  GetNthChild( unsigned int n )       { return m_children.Item( n ); }
    void Insert( impViewTreeModelNode* child, unsigned int n) { m_children.Insert( child, n); }
    void Append( impViewTreeModelNode* child )                { m_children.Add( child ); }
    unsigned int GetChildCount()                          { return m_children.GetCount(); }

    virtual wxString getName() {
        if (!m_title.IsEmpty())
            return m_title;

        cXmlXPath c_path = impRawovlFileManager::getManager().getXPath();
        if (c_path) {
            cXmlXPathResult res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_NAME);
            if (res.size()) {
                return UTF8STRINGWRAP(res[0].content());
            } else {
                res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_FILE);
                if (res.size())
                    return UTF8STRINGWRAP(res[0].content());
            }
        }

        return _("Unknown");
    }

protected:
    wxString            m_title;
    wxString            m_path;
    impViewTreeModelNode   *m_parent;
    impViewTreeModelNodes   m_children;
    sigc::signal<bool, wxDataViewItem> itemChanged;

    void clearChildren() {
        size_t count = m_children.GetCount();
        size_t i;
        for (i = 0; i < count; ++i) {
            impViewTreeModelNode *child = m_children[i];
            delete child;
        }
        m_children.clear();
    }

    virtual void update(const wxString& value) {
        if (m_title.IsEmpty()) {
            cXmlXPath c_path = impRawovlFileManager::getManager().getXPath();
            if (c_path) {
                cXmlXPathResult res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_NAME);
                if (res.size()) {
                    if (value == res[0].wxpath())
                        itemChanged(wxDataViewItem(this));
                } else {
                    res = c_path(string(WX2UTF8(m_path)) + "/" + XPATH_ANY_FILE);
                    if (res.size())
                        if (value == res[0].wxpath())
                            itemChanged(wxDataViewItem(this));
                }
            }
        }

        size_t count = m_children.GetCount();
        size_t i;
        for (i = 0; i < count; ++i) {
            m_children[i]->update(value);
        }
    }
};

class impViewTreeModelNodeSection: public impViewTreeModelNode {
public:
    impViewTreeModelNodeSection(impViewTreeModelNode* parent, const wxString &title, const wxString &path):
            impViewTreeModelNode(parent, title, path) {
        Refresh();
    }

    void Refresh() {
        clearChildren();
        cXmlXPath c_path = impRawovlFileManager::getManager().getXPath();
        if (c_path) {
            cXmlXPathResult res = c_path(string(WX2UTF8(m_path)));
            for (int i = 0; i < res.size(); ++i) {
                wxString role = UTF8STRINGWRAP(res[i].getPropVal("role"));
                wxString path;
                cXmlXPath c_nodepath = impRawovlFileManager::getManager().getXPath();
                c_nodepath.setNodeContext(res[i]);
                cXmlXPathResult noderes = c_nodepath(XPATH_ANY_GUID);
                if (noderes.size()) {
                    path = wxString::Format(wxT(XPATH_FORMAT_OVLID_GUID), role.c_str(), STRING_FOR_FORMAT(noderes[0].content()));
                } else {
                    noderes = c_nodepath(XPATH_ANY_METADATA);
                    wxString newguid = impRawovlFileManager::newGUID();
                    cXmlNode guidnode(XML_TAG_METADATA_GUID, WX2UTF8(newguid), impRawovlFileManager::getManager().getXml().defaultNs());
                    if (noderes.size()) {
                        noderes[0].appendChildren(guidnode);
                    } else {
                        cXmlNode metanode(XML_TAG_METADATA, impRawovlFileManager::getManager().getXml().defaultNs());
                        metanode.prop("role", "importer");
                        metanode.appendChildren(guidnode);
                        res[i].insertNodeAsFirstChild(metanode);
                    }
                    path = wxString::Format(wxT(XPATH_FORMAT_OVLID_GUID), role.c_str(), newguid.c_str());
                }
                Append(new impViewTreeModelNode(this, wxT(""), path));
            }
        }
    }

//    virtual wxString getName() {
//        return m_titlepath;
//    }
//
    virtual bool IsContainer() {
        return m_children.size();
    }

protected:
    virtual void update(const wxString& value) {
        if (value == wxT("/"))
            Refresh();
        impViewTreeModelNode::update(value);
    }

};

class impViewTreeModelNodeRoot: public impViewTreeModelNode {
public:
    impViewTreeModelNodeRoot(wxDataViewModel* owner): impViewTreeModelNode(NULL, wxT(""), wxT("/r:" XML_TAG_OVL)) {
        itemChanged.connect(sigc::mem_fun(*owner, &wxDataViewModel::ItemChanged));
        Append(new impViewTreeModelNodeSection(this, _("Scenery OVLs"), wxT("//r:" XML_TAG_OVL "[@role='scenery']")));
        Append(new impViewTreeModelNodeSection(this, _("Texture OVLs"), wxT("//r:" XML_TAG_OVL "[@role='texture']")));
    }
    virtual bool IsContainer() {
        return true;
    }
    // Make public
    virtual void update(const wxString& value) {
        impViewTreeModelNode::update(value);
    }
};
*/

class impViewTreeModel: public impDataViewModel, public impRawovlFileManagerBaseConnecter {
public:
    impViewTreeModel(impRawovlFileManagerBaseConnecter* slotowner) {
        impRawovlFileManagerBaseConnecter::attachConnection(slotowner);
        m_root = new impViewTreeModelNode(this, impTypes::ELEMENTTYPE_PROJECT);
    }

    ~impViewTreeModel() {
        delete m_root;
    }

    void update(const wxString& value) {
        m_root->update(value);
    }

    const wxDataViewItem getRootItem() {
        return wxDataViewItem(m_root);
    }
//    void Delete( const wxDataViewItem &item ) {
//        MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
//        wxDataViewItem parent( node->GetParent() );
//
//        node->GetParent()->GetChildren().Remove( node );
//        delete node;
//
//        // notify control
//        ItemDeleted( parent, item );
//    }


    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                 unsigned int column, bool ascending ) {
        if (IsContainer(item1) && IsContainer(item2)) {
            wxVariant value1,value2;
            GetValue( value1, item1, 0 );
            GetValue( value2, item2, 0 );

            wxString str1 = value1.GetString();
            wxString str2 = value2.GetString();
            int res = str1.Cmp( str2 );
            if (res) return res;

            // items must be different
            unsigned long litem1 = (unsigned long) item1.GetID();
            unsigned long litem2 = (unsigned long) item2.GetID();

            return litem1-litem2;
        }

        return wxDataViewModel::Compare( item1, item2, column, ascending );
    }

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const
    {
        return 1;
    }

    virtual wxString GetColumnType( unsigned int WXUNUSED(col) ) const {
        return wxT("string");
    }

    virtual void GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const {
        impViewTreeModelNode* node = reinterpret_cast<impViewTreeModelNode*>(item.GetID());
        variant = node->getName();
    }

    virtual bool SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) {
        return false;
    }

    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const {
        if (!item.IsOk())
            return wxDataViewItem(0);

        impViewTreeModelNode* node = reinterpret_cast<impViewTreeModelNode*>(item.GetID());

        // Visible root node
        if (node == m_root)
            return wxDataViewItem(0);

        return wxDataViewItem( (void*) node->GetParent() );
    }

    virtual bool IsContainer( const wxDataViewItem &item ) const {
        if (!item.IsOk())
            return true;

        impViewTreeModelNode* node = reinterpret_cast<impViewTreeModelNode*>(item.GetID());
        return node->IsContainer();
    }

    virtual unsigned int GetChildren( const wxDataViewItem &parent, wxDataViewItemArray &array ) const {
        impViewTreeModelNode* node = reinterpret_cast<impViewTreeModelNode*>(parent.GetID());
        if (!node) {
            array.Add( wxDataViewItem( (void*) m_root ) );
            return 1;
        }

        if (node->GetChildCount() == 0) {
            return 0;
        }

        unsigned int count = node->GetChildren().GetCount();
        unsigned int pos;
        for (pos = 0; pos < count; ++pos) {
            impViewTreeModelNode *child = node->GetChildren().Item( pos );
            array.Add( wxDataViewItem( (void*) child ) );
        }
        return count;
    }

    // DnD

//    virtual bool IsDraggable( const wxDataViewItem &item )
//        {
//            // only drag items
//            return (!IsContainer(item));
//        }
//
//    virtual size_t GetDragDataSize( const wxDataViewItem &item, const wxDataFormat &WXUNUSED(format) )
//        {
//            wxPrintf( "GetDragDataSize\n" );
//
//            MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
//            wxString data;
//            data += node->m_title; data += wxT(" ");
//            data += node->m_artist;
//            return strlen( data.utf8_str() ) + 1;
//        }
//    virtual bool GetDragData( const wxDataViewItem &item, const wxDataFormat &WXUNUSED(format),
//                              void* dest, size_t WXUNUSED(size) )
//        {
//            wxPrintf( "GetDragData\n" );
//
//            MyMusicModelNode *node = (MyMusicModelNode*) item.GetID();
//            wxString data;
//            data += node->m_title; data += wxT(" ");
//            data += node->m_artist;
//            wxCharBuffer buffer( data.utf8_str() );
//            memcpy( dest, buffer, strlen(buffer)+1 );
//            return true;
//        }

private:
    impViewTreeModelNode*   m_root;
};

impViewTree::impViewTree(wxWindow* parent):
        wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_NO_HEADER ),
        m_treecolumn(NULL) {
    initInfo(wxT("/"));
    m_model = new impViewTreeModel(this);
//    impRawovlFileManager::getManager().refreshXml();
    AssociateModel(m_model.get());
    m_treecolumn = AppendTextColumn(wxT(""), 0);
    SetExpanderColumn(m_treecolumn);
    m_treecolumn->SetWidth(GetSize().GetWidth());
    Expand(m_model->getRootItem());

    update(wxT("/"));

	this->Connect( wxEVT_SIZE, wxSizeEventHandler( impViewTree::OnResize ) );
	this->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( impViewTree::OnActivate ) );
}

void impViewTree::update(const wxString& value) {
    m_model->update(value);
    if (value == wxT("/"))
        m_model->Cleared();
    Expand(m_model->getRootItem());
}

impViewTree::~impViewTree() {
    //dtor
}

wxString impViewTree::getName() {
    return _("Project tree");
}

void impViewTree::OnResize(wxSizeEvent& event) {
    if (m_treecolumn)
        m_treecolumn->SetWidth(GetClientSize().GetWidth());
    event.Skip();
}

void impViewTree::OnActivate(wxDataViewEvent& event) {
    impViewTreeModelNode* node = reinterpret_cast<impViewTreeModelNode*>(event.GetItem().GetID());
    if (node) {
        executeCommand(impElements[node->getType()].defaultcommand, node->getType(), impElements[node->getType()].defaultview, node->getPath());
    }
    event.StopPropagation();
}

