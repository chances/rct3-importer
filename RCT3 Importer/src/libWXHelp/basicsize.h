
#ifndef BASICSIZE_H_INCLUDED
#define BASICSIZE_H_INCLUDED

#if 0

#if !wxUSE_IMAGE
class wxSize {
private:
    int w, h;
public:
    wxSize() {
        w = 0;
        h = 0;
    }
    wxSize(int nw, int nh) {
        w = nw;
        h = nh;
    }
    int GetWidth() {return w;}
    int GetHeight() {return h;}
};
#endif

#endif

#endif
