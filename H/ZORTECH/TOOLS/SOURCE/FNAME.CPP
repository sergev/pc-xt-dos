#include <ctype.h>
#include <fname.hpp>


zFileName::zFileName()
{
    memset(&_drive,'\0',14);
    forbid = " *+=[]:;\",.?/";
    fn = _path = NULL;
}

zFileName::zFileName(const char * fname,
        const char *disallow)
{
    memset(&_drive,'\0',14);
    forbid = (const char *) disallow;
    fn = _path = 0;
    if (fname) parse(fname);
}

zFileName::zFileName(const zFileName& a)
{
    unsigned i;
    if (a.fn) {
        i = strlen(a.fn)+1;
        fn = new char [i];
        if (!fn) goto allerr;
        memmove(fn,a.fn,i);
    } else fn = 0;
    if (a._path) {
        i = strlen(a._path)+1;
        _path = new char [i];
        if (!_path) goto allerr;
        memmove(_path,a._path,i);
    } else _path = 0;
    forbid = a.forbid;
    _drive = a._drive;
    strcpy(_name,a._name);
    strcpy(_ext,a._ext);
    err = 0;
    return;
allerr:
    err = ENOMEM;
}

zFileName& zFileName::operator=(const zFileName& a)
{
    if (this == (zFileName *) &a)
        return *this;

    delete fn;
    delete _path;
    fn = _path = 0;
    unsigned i;
    if (a.fn) {
        i = strlen(a.fn)+1;
        fn = new char [i];
        if (!fn)
            goto allerr1;
        else 
            memmove(fn,a.fn,i);
    } else fn = 0;
    if (a._path) {
        i = strlen(a._path)+1;
        _path = new char[i];
        if (!_path) {
allerr1:
            err = ENOMEM;
            return *this;
        } else 
            memmove(_path,a._path,i);
    } else _path = 0;
    forbid = a.forbid;
    _drive = a._drive;
    strcpy(_name,a._name);
    strcpy(_ext,a._ext);
    return *this;
}

const char *zFileName::operator()()
{
    int length = strlen(_name)+strlen(_ext)+1;
    if (_path)
        length += strlen(_path)+1;
    if (_drive)
        length += 2;
    if (fn)
        delete fn;
    fn = new char[length+1];
    if (!fn) {
        err = ENOMEM;
        return 0;
    }
    if (_drive) {
        *fn = _drive;
        *(fn+1) = ':';
        *(fn+2) = '\0';
    } else
        *fn='\0';
    if (_path) {
        strcat(fn,_path);
        strcat(fn,"\\");
    }
    strcat(fn,_name);
    if (*_ext) {
        strcat(fn,".");
        strcat(fn,_ext);
    }
    return fn;
}

int zFileName::build(char d, const char *p,
        const char *n, const char *e, int modify)
{
    if (d) {
        d = toupper(d);
        if (d < 'A' || d > 'Z')	// DJB
            return FN_BADDRIVE;
        _drive = d;
    } else if (!modify)
        _drive = '\0';
    if (n && *n) {
        if (strlen(n) > 8)
            return FN_NTOOLONG;
        if (strpbrk(n,forbid))
            return FN_BADCHAR;
        strcpy(_name,n);
        strupr(_name);
    } else if (!modify)
        memset(_name,'\0',9);
    if (e && *e) {
        if (strlen(e) > 3)
            return FN_XTOOLONG;
        if (strpbrk(e,forbid))
            return FN_BADCHAR;
        strcpy(_ext,e);
        strupr(_ext);
    } else if (!modify)
        memset(_ext,'\0',4);
    if (p) {
        int len = strlen(p);
        const char *r, *q;
        delete _path;
        _path = 0;
        q = strpbrk(p,forbid);
        if (q && *q != '.')
            return FN_BADCHAR;
        for (r = q = p;;) {
            q = strchr(q,'\\');
            if (!q) {
                if (strlen(r) > 12)
                    return FN_DTOOLONG;
                break;
            }
            if (*(q+1) == '\0') {
                --len;              // trailing backslash
                break;
            } else
                r = ++q;
            if (*r == '\\')
                return FN_BADCHAR;
        }
        if (len > 64)
            return FN_PTOOLONG;
        _path = new char[len+1];
        if (!_path) {
            return ENOMEM;
        }
        strcpy(_path,p);
        strupr(_path);
    } else if (!modify) {
        delete _path;
        _path = 0;
    }
    return(0);
}

int zFileName::parse(const char *s)
{
    char local[80], *t, *p, *q, *e, *n, d = '\0';

    if (!s)
        return FN_NULL;
    strcpy(local,s);
    n = strrchr(local,'\\');
    if (n) {
        if (n > local && *(n-1) == '\\')
            return FN_BADCHAR;
        p = local;
        *n++ = '\0';
    } else {
        p = 0;
        n = local;
    }
    e = strrchr(n,'.');
    if (e)
        *e++ = '\0';
    if (p)
        q = local;
    else
        q = n;
    t = strchr(q,':');
    if (t) {
        if (t > q+1)
            return FN_BADCOLON;
        d = *q;
        if (p)
            p += 2;
        else
            n += 2;
    }
    if (*n == '\0')
        return FN_NULL;
    return build(d,p,n,e,0);
}
