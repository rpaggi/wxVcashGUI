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
 
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/clipbrd.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#endif

#include "HistoryPage.h"
#include "Resources.h"
#include "VcashApp.h"

namespace wxGUI {
    int cmpHistory(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortDt) {
        HistoryPage::SortData *sortData = (HistoryPage::SortData *) sortDt;
        HistoryPage *historyPage = sortData->historyPage;
        wxListCtrl *listCtrl = historyPage->listCtrl;
        auto order = sortData->order;

        for(int i=0; i<order.size(); i++) {
            auto col = order[i].first;
            int result;
            switch(col) {
                case HistoryPage::Date: {
                    std::string txid1 = *(std::string *) item1;
                    std::string txid2 = *(std::string *) item2;
                    std::time_t t1 = historyPage->transactions.find(txid1)->second.time;
                    std::time_t t2 = historyPage->transactions.find(txid2)->second.time;
                    result = (t1 < t2) ? -1 : (t1 > t2);
                    break;
                }

                case HistoryPage::Status: {
                    long index1 = listCtrl->FindItem(-1, item1);
                    auto str1 = listCtrl->GetItemText(index1, col);

                    long index2 = listCtrl->FindItem(-1, item2);
                    auto str2 = listCtrl->GetItemText(index2, col);

                    result = str1.Cmp(str2);
                    break;
                }

                case HistoryPage::Amount: {
                    long index1 = listCtrl->FindItem(-1, item1);
                    auto str1 = listCtrl->GetItemText(index1, col);
                    long index2 = listCtrl->FindItem(-1, item2);
                    auto str2 = listCtrl->GetItemText(index2, col);
                    double amount1, amount2;
                    if (str1.ToDouble(&amount1) && str2.ToDouble(&amount2)) {
                        result = (amount1 < amount2) ? -1 : (amount1 > amount2);
                    } else
                        result = 0;
                    break;
                }
            }
            if((result != 0) || (i==order.size()-1))
                return order[i].second ? result : -result;
        }
    }
}

using namespace wxGUI;

HistoryPage::HistoryPage(VcashApp &vcashApp, wxWindow &parent)
        : transactions(), wxPanel(&parent) {

    vcashApp.view.historyPage = this;

    listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(430, wxDefaultSize.GetHeight()),
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);

    auto addMargin = [](wxBitmap bm, bool rotate) {
        const int leftMargin = 5;
        wxImage img(bm.GetWidth()+leftMargin, bm.GetHeight());

        img.InitAlpha();
        unsigned char *alpha = img.GetAlpha();
        memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, img.GetWidth()*img.GetHeight());

        wxImage imgBm = bm.ConvertToImage();
        if(rotate)
            imgBm = imgBm.Rotate180();
        img.Paste(imgBm, leftMargin, 0);
        return img;
    };

    wxBitmap red = Resources::redR;
    wxImage redL = addMargin(red, true);
    wxImage redR = addMargin(red, false);

    wxBitmap green = Resources::greenR;
    wxImage greenL = addMargin(green, true);
    wxImage greenR = addMargin(green, false);

    wxBitmap yellow = Resources::yellowR;
    wxImage yellowL = addMargin(yellow, true);
    wxImage yellowR = addMargin(yellow, false);

    int w = redL.GetWidth();
    int h = redL.GetHeight();
    wxImageList *statusImages = new wxImageList(w, h, true);
    statusImages->Add(redL);
    statusImages->Add(greenL);
    statusImages->Add(yellowL);
    statusImages->Add(redR);
    statusImages->Add(greenR);
    statusImages->Add(yellowR);

    listCtrl->SetImageList(statusImages, wxIMAGE_LIST_SMALL);

    listCtrl->InsertColumn(Icon, wxT(""), wxLIST_FORMAT_CENTER, 30);
    listCtrl->InsertColumn(Date, wxT("Date"), wxLIST_FORMAT_LEFT, 135);
    listCtrl->InsertColumn(Status, wxT("Status"), wxLIST_FORMAT_LEFT, 125);
    listCtrl->InsertColumn(Amount, wxT("Amount"), wxLIST_FORMAT_LEFT, 130);

    wxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
    pageSizer->Add(listCtrl, 1, wxALL | wxEXPAND, 5);

    SetSizerAndFit(pageSizer);

    // Sort is according to first element in order vector. true means ascending order.
    // In case of tie, we use next next element in vector
    sortData = { this, { { Date, false }, { Amount, true }, { Status, true } }};

    listCtrl->Bind(wxEVT_LIST_COL_CLICK, [this](wxListEvent &ev) {
        Column column = static_cast<Column>(ev.GetColumn());

        int i;
        for(i=0; (i<sortData.order.size()) && (sortData.order[i].first != column); i++)
            ;

        if(i<sortData.order.size()) {
            std::pair<Column, bool> p = sortData.order[i];
            p.second = !p.second; // invert order

            // move clicked column to first position
            for(int j=i; j>0; j--)
                sortData.order[j] = sortData.order[j-1];
            sortData.order[0] = p;
            listCtrl->SortItems(cmpHistory, (wxIntPtr) &sortData);
        }
    });

    listCtrl->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, [this, &vcashApp](wxListEvent &event) {
        long index = event.GetIndex();

        if (index >= 0) {
            enum PopupMenu {
                BlockExplorer, Copy, Info, Lock
            };
            wxMenu popupMenu;
            popupMenu.Append(BlockExplorer, wxT("&Block explorer"));
            popupMenu.Append(Copy, wxT("&Copy"));
            popupMenu.Append(Info, wxT("&Information"));
            popupMenu.Append(Lock, wxT("&Lock"));

            auto select = GetPopupMenuSelectionFromUser(popupMenu);
            listCtrl->SetItemState(index, 0,
                                   wxLIST_STATE_FOCUSED | wxLIST_STATE_SELECTED);

            switch (select) {
                case BlockExplorer: {
                    std::string txid = *((std::string *) listCtrl->GetItemData(index));
                    std::string url = "https://www.blockexperts.com/xvc/tx/" + txid;
                    wxLaunchDefaultBrowser(url);
                    break;
                }

                case Copy: {
                    std::string txid = *((std::string *) listCtrl->GetItemData(index));

                    auto clipboard = wxTheClipboard;

                    if (clipboard->Open()) {
                        clipboard->Clear();
                        clipboard->SetData(new wxTextDataObject(txid));
                        clipboard->Flush();

                        clipboard->Close();
                    }
                    break;
                }

                case Info: {
                    std::string txid = *((std::string *) listCtrl->GetItemData(index));
                    std::string cmd = "gettransaction " + txid;
                    vcashApp.controller.onConsoleCommandEntered(cmd);
                    break;
                }

                case Lock: {
                    std::string txid = *((std::string *) listCtrl->GetItemData(index));
                    vcashApp.controller.onZerotimeLockTransaction(txid);
                    break;
                }

                default: {
                    break;
                };
            }
        }
    });
}

void HistoryPage::addTransaction(const std::string &txid, const std::time_t &time, const std::string &status,
                                 const std::string &amount) {
    long newItemIndex = transactions.size();
    TxData txData = { time };
    auto pair = transactions.insert(std::make_pair(txid, txData));

    long index;

    if (pair.second) {
        // Not in list. This is a new insertion
        wxListItem item{};
        item.SetId(newItemIndex);

        index = listCtrl->InsertItem(item);
        if (index >= 0) {
            listCtrl->SetItemPtrData(index, (wxUIntPtr) &(pair.first->first)); // set txid as item txData
        } else
            transactions.erase(pair.first); // was not inserted in listCtrl. Remove it from transactions map
    } else {
        // This is a replacement
        index = listCtrl->FindItem(-1, (wxUIntPtr) &(pair.first->first)); // search for item with this txid
    }

    if (index >= 0) {
        char formattedTime[256];
        std::strftime(formattedTime, sizeof(formattedTime), "%m/%d/%y %H:%M:%S", std::localtime(&time));

        listCtrl->SetItem(index, Icon, wxString(""), Yellow);
        listCtrl->SetItem(index, Date, wxString(formattedTime));
        listCtrl->SetItem(index, Status, wxString(status));
        listCtrl->SetItem(index, Amount, wxString(amount));
        listCtrl->SetItemState(index, 0, 0);
        // listCtrl->EnsureVisible(index);
    }

    listCtrl->SortItems(cmpHistory, (wxIntPtr) &sortData);
}

void HistoryPage::setColour(const std::string &txid, BulletColor color) {
    auto it = transactions.find(txid);
    if (it != transactions.end()) {
        long index = listCtrl->FindItem(-1,
                                        (wxUIntPtr) &(it->first));  // find index of item in listCtrl with this txid
        if (index >= 0) {
            bool isOut = listCtrl->GetItemText(index, Amount)[0] == '-';
            int numImages = listCtrl->GetImageList(wxIMAGE_LIST_SMALL)->GetImageCount();
            listCtrl->SetItem(index, Icon, wxString(""), isOut ? color : numImages / 2 + color);
        }
    }
}

void HistoryPage::setStatus(const std::string &txid, const std::string &status) {
    auto it = transactions.find(txid);
    if (it != transactions.end()) {
        long index = listCtrl->FindItem(-1,
                                        (wxUIntPtr) &(it->first));  // find index of item in listCtrl with this txid
        if (index >= 0) {
            listCtrl->SetItem(index, Status, wxString(status));
        }
    }
}
