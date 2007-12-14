
#ifndef _OVLDEXCEPTION_H_
#define _OVLDEXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

class EOvlD: public std::exception {
public:
    EOvlD(const string& message);
    virtual const char* what() const throw() {
        return m_message.c_str();
    }
    virtual const string& swhat() const throw() {
        return m_message;
    }
    ~EOvlD() throw() {};
protected:
    string m_message;
};


#endif
