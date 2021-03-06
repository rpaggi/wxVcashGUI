/******************************************************************************
 * wxVcashGUI: a GUI for Vcash, a decentralized currency
 *             for the internet (https://vcash.info).
 *
 * Copyright (c) The Vcash Developers
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#include "Resources.h"

#include "resources/greenR16.cpp"
#include "resources/redR16.cpp"
#include "resources/yellowR16.cpp"
#include "resources/connecting22.cpp"

#if defined (__WXGTK__)
#include "resources/locked22.cpp"
#include "resources/tools22.cpp"
#include "resources/unlocked22.cpp"
#include "resources/settings22.cpp"
#elif defined (__WXMSW__)
#include "resources/locked20.cpp"
#include "resources/tools20.cpp"
#include "resources/unlocked20.cpp"
#include "resources/settings20.cpp"
#elif defined (__WXOSX__)
#include "resources/locked15.cpp"
#include "resources/tools15.cpp"
#include "resources/unlocked15.cpp"
#include "resources/settings15.cpp"
#else
#error "You must define one of: __WXGTK__, __WXMSW__ or __WXOSX__"
#endif


#if defined ( __WXGTK__) || defined (__WXMSW__)
#include "resources/vcash.cpp"
#endif

using namespace wxGUI;

void Resources::init() {
    wxInitAllImageHandlers();

    redR = wxBITMAP_PNG_FROM_DATA(redR16);
    greenR = wxBITMAP_PNG_FROM_DATA(greenR16);
    yellowR = wxBITMAP_PNG_FROM_DATA(yellowR16);

    // empty = wxBITMAP_PNG_FROM_DATA(connecting22);


    #if defined (__WXGTK__)
    locked = wxBITMAP_PNG_FROM_DATA(locked22);
    tools = wxBITMAP_PNG_FROM_DATA(tools22);
    unlocked = wxBITMAP_PNG_FROM_DATA(unlocked22);
    settings = wxBITMAP_PNG_FROM_DATA(settings22);
    #elif defined (__WXMSW__)
    locked = wxBITMAP_PNG_FROM_DATA(locked20);
    tools = wxBITMAP_PNG_FROM_DATA(tools20);
    unlocked = wxBITMAP_PNG_FROM_DATA(unlocked20);
    settings = wxBITMAP_PNG_FROM_DATA(settings20);
    #elif defined (__WXOSX__)
    locked = wxBITMAP_PNG_FROM_DATA(locked15);
    tools = wxBITMAP_PNG_FROM_DATA(tools15);
    unlocked = wxBITMAP_PNG_FROM_DATA(unlocked15);
    settings = wxBITMAP_PNG_FROM_DATA(settings15);
    #endif

    wxImage im(locked.GetWidth(), locked.GetHeight());
    im.InitAlpha();
    unsigned char *alpha = im.GetAlpha();
    memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, im.GetWidth()*im.GetHeight());
    empty = im;

    wxBitmap bm = wxBITMAP_PNG_FROM_DATA(vcash);
    vcashIcon.CopyFromBitmap(bm);
}

wxBitmap Resources::redR;

wxBitmap Resources::greenR;

wxBitmap Resources::yellowR;

wxBitmap Resources::empty;

wxBitmap Resources::locked;

wxBitmap Resources::settings;

wxBitmap Resources::tools;

wxBitmap Resources::unlocked;

wxIcon Resources::vcashIcon;
