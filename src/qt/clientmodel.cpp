// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2021-2022 The Dystater developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "clientmodel.h"
#include "guiconstants.h"
#include "optionsmodel.h"
#include "addresstablemodel.h"
#include "transactiontablemodel.h"

#include "main.h"

#include <QTimer>
#include <QDateTime>

ClientModel::ClientModel(OptionsModel *optionsModel, QObject *parent) :
    QObject(parent), optionsModel(optionsModel),
    cachedNumConnections(0), cachedNumBlocks(0)
{
    numBlocksAtStartup = -1;
}

int ClientModel::getNumConnections() const
{
    return vNodes.size();
}

int ClientModel::getNumBlocks() const
{
    return nBestHeight;
}

int ClientModel::getNumBlocksAtStartup()
{
    if (numBlocksAtStartup == -1) numBlocksAtStartup = getNumBlocks();
    return numBlocksAtStartup;
}

QDateTime ClientModel::getLastBlockDate() const
{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return QDateTime::fromTime_t(pindexBest->GetBlockTime());
#else
    return QDateTime::fromSecsSinceEpoch(pindexBest->GetBlockTime());
#endif
}

void ClientModel::update()
{
    int newNumConnections = getNumConnections();
    int newNumBlocks = getNumBlocks();
    QString newStatusBar = getStatusBarWarnings();

    if(cachedNumConnections != newNumConnections)
        emit numConnectionsChanged(newNumConnections);
    if(cachedNumBlocks != newNumBlocks || cachedStatusBar != newStatusBar)
    {
        // Simply emit a numBlocksChanged for now in case the status message changes,
        // so that the view updates the status bar.
        // TODO: It should send a notification.
        //    (However, this might generate looped notifications and needs to be thought through and tested carefully)
        //    error(tr("Network Alert"), newStatusBar);
        emit numBlocksChanged(newNumBlocks);
    }
    
    cachedNumConnections = newNumConnections;
    cachedNumBlocks = newNumBlocks;
    cachedStatusBar = newStatusBar;
}

bool ClientModel::isTestNet() const
{
    return fTestNet;
}

bool ClientModel::inInitialBlockDownload() const
{
    return IsInitialBlockDownload();
}

int ClientModel::getNumBlocksOfPeers() const
{
    return GetNumBlocksOfPeers();
}

QString ClientModel::getStatusBarWarnings() const
{
    return QString::fromStdString(GetWarnings("statusbar"));
}

OptionsModel *ClientModel::getOptionsModel()
{
    return optionsModel;
}

QString ClientModel::formatFullVersion() const
{
    return QString::fromStdString(FormatFullVersion());
}

QString ClientModel::formatBuildDate() const
{
    return QString::fromStdString(CLIENT_DATE);
}
