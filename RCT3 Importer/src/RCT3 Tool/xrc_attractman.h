//
// This file was automatically generated by wxrc, do not edit by hand.
//

#ifndef __xrc_attractman_h__
#define __xrc_attractman_h__
class dlgAttractMan : public wxDialog {
protected:
 wxListBox* m_AttractionList;
 wxButton* m_Add;
 wxButton* m_Edit;
 wxButton* m_Delete;
 wxButton* m_Close;

private:
 void InitWidgetsFromXRC(wxWindow *parent){
  wxXmlResource::Get()->LoadObject(this,parent,_T("dlgAttractMan"), _T("wxDialog"));
  m_AttractionList = XRCCTRL(*this,"m_AttractionList",wxListBox);
  m_Add = XRCCTRL(*this,"m_Add",wxButton);
  m_Edit = XRCCTRL(*this,"m_Edit",wxButton);
  m_Delete = XRCCTRL(*this,"m_Delete",wxButton);
  m_Close = XRCCTRL(*this,"m_Close",wxButton);
 }
public:
dlgAttractMan(wxWindow *parent=NULL){
  InitWidgetsFromXRC((wxWindow *)parent);
 }
};

void 
InitXmlAttractMan();
#endif
