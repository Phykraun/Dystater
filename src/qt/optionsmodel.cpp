// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2021-2022 The Dystater developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "optionsmodel.h"
#include "dystaterunits.h"
#include <QSettings>

#include "init.h"
#include "walletdb.h"

OptionsModel::OptionsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    Init();
}

void OptionsModel::Init()
{
    QSettings settings;

    // These are QT-only settings:
    nDisplayUnit = settings.value("nDisplayUnit", DystaterUnits::DYST).toInt();
    bDisplayAddresses = settings.value("bDisplayAddresses", false).toBool();
    fMinimizeToTray = settings.value("fMinimizeToTray", false).toBool();
    fMinimizeOnClose = settings.value("fMinimizeOnClose", false).toBool();
    nTransactionFee = settings.value("nTransactionFee").toLongLong();

    // These are shared with core dystater; we want
    // command-line options to override the GUI settings:
    if (settings.contains("fUseUPnP"))
        SoftSetBoolArg("-upnp", settings.value("fUseUPnP").toBool());
    if (settings.contains("addrProxy") && settings.value("fUseProxy").toBool())
        SoftSetArg("-proxy", settings.value("addrProxy").toString().toStdString());
    if (settings.contains("detachDB"))
        SoftSetBoolArg("-detachdb", settings.value("detachDB").toBool());
}

bool OptionsModel::Upgrade()
{
    QSettings settings;

    if (settings.contains("bImportFinished"))
        return false; // Already upgraded
    
    settings.setValue("bImportFinished", true);

    // Move settings from old wallet.dat (if any):
    CWalletDB walletdb("wallet.dat");

    QList<QString> intOptions;
    intOptions << "nDisplayUnit" << "nTransactionFee";
    foreach(QString key, intOptions)
    {
        int value = 0;
        if (walletdb.ReadSetting(key.toStdString(), value))
        {
            settings.setValue(key, value);
            walletdb.EraseSetting(key.toStdString());
        }
    }
    QList<QString> boolOptions;
    boolOptions << "bDisplayAddresses" << "fMinimizeToTray" << "fMinimizeOnClose" << "fUseProxy" << "fUseUPnP";
    foreach(QString key, boolOptions)
    {
        bool value = false;
        if (walletdb.ReadSetting(key.toStdString(), value))
        {
            settings.setValue(key, value);
            walletdb.EraseSetting(key.toStdString());
        }
    }
    try
    {
        CAddress addrProxyAddress;
        if (walletdb.ReadSetting("addrProxy", addrProxyAddress))
        {
            addrProxy = addrProxyAddress;
            settings.setValue("addrProxy", addrProxy.ToStringIPPort().c_str());
            walletdb.EraseSetting("addrProxy");
        }
    }
    catch (std::ios_base::failure &e)
    {
        // 0.6.0rc1 saved this as a CService, which causes failure when parsing as a CAddress
        if (walletdb.ReadSetting("addrProxy", addrProxy))
        {
            settings.setValue("addrProxy", addrProxy.ToStringIPPort().c_str());
            walletdb.EraseSetting("addrProxy");
        }
    }
    Init();

    return true;
}


int OptionsModel::rowCount(const QModelIndex & parent) const
{
    return OptionIDRowCount;
}

QVariant OptionsModel::data(const QModelIndex & index, int role) const
{
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            return QVariant(GetStartOnSystemStartup());
        case MinimizeToTray:
            return QVariant(fMinimizeToTray);
        case MapPortUPnP:
            return settings.value("fUseUPnP", GetBoolArg("-upnp", true));
        case MinimizeOnClose:
            return QVariant(fMinimizeOnClose);
        case ConnectSOCKS4:
            return settings.value("fUseProxy", false);
        case ProxyIP:
            return QVariant(QString::fromStdString(addrProxy.ToStringIP()));
        case ProxyPort:
            return QVariant(addrProxy.GetPort());
        case Fee:
            return QVariant(nTransactionFee);
        case DisplayUnit:
            return QVariant(nDisplayUnit);
        case DisplayAddresses:
            return QVariant(bDisplayAddresses);
        case DetachDatabases:
            return QVariant(fDetachDB);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool OptionsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    bool successful = true; /* set to false on parse error */
    if(role == Qt::EditRole)
    {
        QSettings settings;
        switch(index.row())
        {
        case StartAtStartup:
            successful = SetStartOnSystemStartup(value.toBool());
            break;
        case MinimizeToTray:
            fMinimizeToTray = value.toBool();
            settings.setValue("fMinimizeToTray", fMinimizeToTray);
            break;
        case MapPortUPnP:
            {
                 bool bUseUPnP = value.toBool();
                settings.setValue("fUseUPnP", bUseUPnP);
                MapPort(bUseUPnP);
            }
            break;
        case MinimizeOnClose:
            fMinimizeOnClose = value.toBool();
            settings.setValue("fMinimizeOnClose", fMinimizeOnClose);
            break;
        case ConnectSOCKS4:
            fUseProxy = value.toBool();
            settings.setValue("fUseProxy", fUseProxy);
            break;
        case ProxyIP:
            {
                // Use CAddress to parse and check IP
                CNetAddr addr(value.toString().toStdString());
                if (addr.IsValid())
                {
                    addrProxy.SetIP(addr);
                    settings.setValue("addrProxy", addrProxy.ToStringIPPort().c_str());
                }
                else
                {
                    successful = false;
                }
            }
            break;
        case ProxyPort:
            {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                int nPort = atoi(value.toString().toLatin1().data());
#else
                int nPort = atoi(value.toString().toLatin1().data());
#endif
                if (nPort > 0 && nPort < std::numeric_limits<unsigned short>::max())
                {
                   addrProxy.SetPort(nPort);
                    settings.setValue("addrProxy", addrProxy.ToStringIPPort().c_str());
                }
                else
                {
                    successful = false;
                }
            }
            break;
        case Fee: {
            nTransactionFee = value.toLongLong();
            settings.setValue("nTransactionFee", nTransactionFee);
            }
            break;
        case DisplayUnit: {
            int unit = value.toInt();
            nDisplayUnit = unit;
            settings.setValue("nDisplayUnit", nDisplayUnit);
            emit displayUnitChanged(unit);
            }
            break;
        case DisplayAddresses: {
            bDisplayAddresses = value.toBool();
            settings.setValue("bDisplayAddresses", bDisplayAddresses);
            }
            break;
        case DetachDatabases: {
            fDetachDB = value.toBool();
            settings.setValue("detachDB", fDetachDB);
        }
        break;
        default:
            break;
        }
    }
    emit dataChanged(index, index);

    return successful;
}

qint64 OptionsModel::getTransactionFee()
{
    return nTransactionFee;
}

bool OptionsModel::getMinimizeToTray()
{
    return fMinimizeToTray;
}

bool OptionsModel::getMinimizeOnClose()
{
    return fMinimizeOnClose;
}

int OptionsModel::getDisplayUnit()
{
    return nDisplayUnit;
}

bool OptionsModel::getDisplayAddresses()
{
    return bDisplayAddresses;
}
