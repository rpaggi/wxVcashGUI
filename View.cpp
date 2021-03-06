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
#include <wx/msgdlg.h>
#include <wx/notifmsg.h>
#endif

#include "AccountPage.h"
#include "AddressesPage.h"
#include "ConsolePage.h"
#include "EntryDialog.h"
#include "HistoryPage.h"
#include "MainFrame.h"
#include "MiningPage.h"
#include "StatusBarWallet.h"
#include "StatusBar.h"
#include "ToolsPanel.h"
#include "View.h"
#include "WalletActions.h"

using namespace wxGUI;

// Account

void View::setBalance(const std::string &balance) {
    accountPage->setBalance(balance);
}

void View::setUnconfirmed(const std::string &unconfirmed) {
    accountPage->setUnconfirmed(unconfirmed);
}

void View::setStake(const std::string &stake) {
    accountPage->setStake(stake);
}

void View::setImmature(const std::string &immature) {
    accountPage->setImmature(immature);
}

// Wallet state

// sets wallet state shown in view. Calls controller too
void View::setWalletStatus(WalletStatus walletStatus) {
    walletLock->setWalletStatus(walletStatus);
}

// gets wallet state shown in view
WalletStatus View::getWalletStatus() {
    return walletLock->getWalletStatus();
}

// Status bar

void View::setStatusBarMessage(const std::string &msg) {
    statusBar->SetMessage(wxString(msg));
}

// Mining state

// sets mining state shown in view. Calls controller too
void View::setMining(bool mining) {
    miningPage->setMining(mining);
}

// sets mining state shown in view
bool View::getMining() {
    return miningPage->getMining();
}

// Address book

void View::addAddress(const std::string &account, const std::string &address) {
    addressesPage->addAddress(account, address);
}

// Console

// apends a new line of text to console output
void View::appendToConsole(const std::string &text, bool bold) {
    toolsPanel->showConsolePage();
    consolePage->appendToConsole(text, bold);
}

// History

void View::addTransaction(const std::string &txid, const std::time_t &date, const std::string &status,
                               const std::string &amount) {
    historyPage->addTransaction(txid, date, status, amount);
}

void View::setColour(const std::string &txid, BulletColor color) {
    historyPage->setColour(txid, color);
}

void View::setStatus(const std::string &txid, const std::string &status) {
    historyPage->setStatus(txid, status);
}

// Statistics

void View::setTCP(const std::string &tcp) {
    statisticsPage->setTCP(tcp);
}

void View::setUDP(const std::string &udp) {
    statisticsPage->setUDP(udp);
}

void View::setEstimated(const std::string &estimated) {
    statisticsPage->setEstimated(estimated);
}

void View::setTotal(const std::string &total) {
    statisticsPage->setTotal(total);
}

void View::setDifficulty(const std::string &difficulty) {
    statisticsPage->setDifficulty(difficulty);
}

void View::setSupply(const std::string &supply) {
    statisticsPage->setSupply(supply);
}

void View::messageBox(const std::string &msg, const std::string &title, long style) {
    wxMessageBox(msg, title, style, mainFrame);
}

void View::notificationBox(const std::string &msg, const std::string &title) {
    wxNotificationMessage *notificationMessage = new wxNotificationMessage(wxString(title), wxString(msg), mainFrame);
    notificationMessage->Show(2);
}

std::pair<bool, std::string> View::restoreHDSeed() {
    return WalletActions::restoreHDSeed(*mainFrame);
}