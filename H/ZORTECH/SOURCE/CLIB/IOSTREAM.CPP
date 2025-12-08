// Iostreams Package
// Bruce Perens, July-August 1990
//
// Modified Steve Teale April 1991
// Copyright Zortech 1990-1991. All Rights Reserved.

#include <iostream.hpp>

iostream::iostream() : ios(0) {}
// dummy constructor for when derived classes initialize
// ios directly

iostream::iostream(streambuf *buf) : ios(buf) {}

iostream::~iostream() {}
