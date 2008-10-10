/////////////////////////////////////////////////////////////////////////////
// Name:        silence.h
// Purpose:     Support classes/interface to turn off need for user interaction
// Author:      Tobias Minich
// Modified by:
// Created:     Nov 29 2006
// RCS-ID:
// Copyright:   (c) 2006 Tobias Minich
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef SILENCE_H_INCLUDED
#define SILENCE_H_INCLUDED

class wxSilent/*: public wxObject*/ {
//DECLARE_CLASS(wxSilent)
public:
    wxSilent()/*:wxObject()*/{m_silent=false;};
    wxSilent(const wxSilent& copyFrom);

    virtual ~wxSilent(){}

    virtual void Silence(bool silent = true) {m_silent = silent;};

//    virtual wxSilent *Clone() const { return new wxSilent(*this); }
    bool Copy(const wxSilent& val);
protected:
    bool m_silent;
private:
    wxSilent& operator=(const wxSilent&);
};

class wxSilencer {
public:
    wxSilencer(wxSilent* subject);
    wxSilencer(void* subject);
    ~wxSilencer();
private:
    wxSilent *m_subject;
};


#endif // SILENCE_H_INCLUDED
