/*_ common.cpp   Tue Mar 22 1988   Modified by: Walter Bright */

#include	<stream.hpp>
#include	<common.hpp>

istream& istream::operator>>(common& rc)
{
    if (skipws)
	eatwhite(*this);
    else
    {	if (tied_to)
	    tied_to->bp->overflow();	/* empty the buffer		*/
    }
    return rc.read(this);
}

