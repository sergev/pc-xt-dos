#ifndef __DATEDIT_HPP
#define __DATEDIT_HPP
#include <ctype.h>
#include <tools.hpp>
#include <sedit.hpp>

extern int _usdate;

class zDateEditor : public zStringEditor {
public:
    zDateEditor() : (6,0) {}
    int filter(char, char *, int);
    int convertor(int k, char* s, int pos);
    static int notUS;
private:
    char loc[7];
    int month() {
        return notUS?
        (10*(loc[2]-'0')+loc[3]-'0'):
        (10*(loc[0]-'0')+loc[1] - '0'); }
    int day() {
        return notUS?
        (10*(loc[0]-'0')+loc[1]-'0'):
        (10*(loc[2]-'0') + loc[3] - '0'); }
    int year() {
        return 10*(loc[4]-'0') + loc[5] -'0'; }
};
#endif
