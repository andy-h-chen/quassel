/***************************************************************************
 *   Copyright (C) 2005-2014 by the Quassel Project                        *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

//#include "chatviewsettings.h"
#include "bb10uistyle.h"

#include <bb/cascades/Application>
#include <QFile>
#include <QTextStream>

#include "bb10uisettings.h"
#include "qssparser.h"

Bb10UiStyle::Bb10UiStyle(QObject *parent) : QObject(parent)
{
    //ChatViewSettings s;
    //s.notify("TimestampFormat", this, SLOT(updateTimestampFormatString()));
    //updateTimestampFormatString();
    loadStyleSheet();
}


Bb10UiStyle::~Bb10UiStyle() {
    qDeleteAll(_metricsCache);
}

void Bb10UiStyle::updateTimestampFormatString()
{
    //ChatViewSettings s;
    //setTimestampFormatString(s.timestampFormatString());
}


void Bb10UiStyle::generateSettingsQss() const
{
    QFile settingsQss(Quassel::configDirPath() + "settings.qss");
    if (!settingsQss.open(QFile::WriteOnly|QFile::Truncate)) {
        qDebug() << "xxxxx Could not open" << settingsQss.fileName() << "for writing!";
        return;
    }
    QTextStream out(&settingsQss);

    out << "// Style settings made in Quassel's configuration dialog\n"
        << "// This file is automatically generated, do not edit\n";

    // ChatView
    ///////////
    Bb10UiStyleSettings fs("Fonts");
    if (fs.value("UseCustomChatViewFont").toBool())
        out << "\n// ChatView Font\n"
            << "ChatLine { " << fontDescription(fs.value("ChatView").value<QFont>()) << "; }\n";

    Bb10UiStyleSettings s("Colors");
    if (s.value("UseChatViewColors").toBool()) {
        out << "\n// Custom ChatView Colors\n"

        // markerline is special in that it always used to use a gradient, so we keep this behavior even with the new implementation
        << "Palette { marker-line: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " << color("MarkerLine", s) << ", stop: 0.1 transparent); }\n"
        << "ChatView { background: " << color("ChatViewBackground", s) << "; }\n\n"
        << "ChatLine[label=\"highlight\"] {\n"
        << "  foreground: " << color("Highlight", s) << ";\n"
        << "  background: " << color("HighlightBackground", s) << ";\n"
        << "}\n\n"
        << "ChatLine::timestamp { foreground: " << color("Timestamp", s) << "; }\n\n"

        << msgTypeQss("plain", "ChannelMsg", s)
        << msgTypeQss("notice", "ServerMsg", s)
        << msgTypeQss("action", "ActionMsg", s)
        << msgTypeQss("nick", "CommandMsg", s)
        << msgTypeQss("mode", "CommandMsg", s)
        << msgTypeQss("join", "CommandMsg", s)
        << msgTypeQss("part", "CommandMsg", s)
        << msgTypeQss("quit", "CommandMsg", s)
        << msgTypeQss("kick", "CommandMsg", s)
        << msgTypeQss("kill", "CommandMsg", s)
        << msgTypeQss("server", "ServerMsg", s)
        << msgTypeQss("info", "ServerMsg", s)
        << msgTypeQss("error", "ErrorMsg", s)
        << msgTypeQss("daychange", "ServerMsg", s)
        << msgTypeQss("topic", "CommandMsg", s)
        << msgTypeQss("netsplit-join", "CommandMsg", s)
        << msgTypeQss("netsplit-quit", "CommandMsg", s)
        << msgTypeQss("invite", "CommandMsg", s)
        << "\n";
    }

    if (s.value("UseSenderColors").toBool()) {
        out << "\n// Sender Colors\n"
            << "ChatLine::sender#plain[sender=\"self\"] { foreground: " << color("SenderSelf", s) << "; }\n\n";

        for (int i = 0; i < 16; i++)
            out << senderQss(i, s);
    }

    // ItemViews
    ////////////

    UiStyleSettings uiFonts("Fonts");
    if (uiFonts.value("UseCustomItemViewFont").toBool()) {
        QString fontDesc = fontDescription(uiFonts.value("ItemView").value<QFont>());
        out << "\n// ItemView Font\n"
            << "ChatListItem { " << fontDesc << "; }\n"
            << "NickListItem { " << fontDesc << "; }\n\n";
    }

    UiStyleSettings uiColors("Colors");
    if (uiColors.value("UseBufferViewColors").toBool()) {
        out << "\n// BufferView Colors\n"
            << "ChatListItem { foreground: " << color("DefaultBuffer", uiColors) << "; }\n"
            << chatListItemQss("inactive", "InactiveBuffer", uiColors)
            << chatListItemQss("channel-event", "ActiveBuffer", uiColors)
            << chatListItemQss("unread-message", "UnreadBuffer", uiColors)
            << chatListItemQss("highlighted", "HighlightedBuffer", uiColors);
    }

    if (uiColors.value("UseNickViewColors").toBool()) {
        out << "\n// NickView Colors\n"
            << "NickListItem[type=\"category\"] { foreground: " << color("DefaultBuffer", uiColors) << "; }\n"
            << "NickListItem[type=\"user\"] { foreground: " << color("OnlineNick", uiColors) << "; }\n"
            << "NickListItem[type=\"user\", state=\"away\"] { foreground: " << color("AwayNick", uiColors) << "; }\n";
    }

    settingsQss.close();
}


QString Bb10UiStyle::color(const QString &key, UiSettings &settings) const
{
    return settings.value(key).value<QColor>().name();
}


QString Bb10UiStyle::fontDescription(const QFont &font) const
{
    QString desc = "font: ";
    if (font.italic())
        desc += "italic ";
    if (font.bold())
        desc += "bold ";
    if (!font.italic() && !font.bold())
        desc += "normal ";
    desc += QString("%1pt \"%2\"").arg(font.pointSize()).arg(font.family());
    return desc;
}


QString Bb10UiStyle::msgTypeQss(const QString &msgType, const QString &key, UiSettings &settings) const
{
    return QString("ChatLine#%1 { foreground: %2; }\n").arg(msgType, color(key, settings));
}


QString Bb10UiStyle::senderQss(int i, UiSettings &settings) const
{
    QString dez = QString::number(i);
    if (dez.length() == 1) dez.prepend('0');

    return QString("ChatLine::sender#plain[sender=\"0%1\"] { foreground: %2; }\n").arg(QString::number(i, 16), color("Sender"+dez, settings));
}


QString Bb10UiStyle::chatListItemQss(const QString &state, const QString &key, UiSettings &settings) const
{
    return QString("ChatListItem[state=\"%1\"] { foreground: %2; }\n").arg(state, color(key, settings));
}

QVariant Bb10UiStyle::channelListViewItemData(const QModelIndex &index, int role) const
{
    BufferInfo::Type type = (BufferInfo::Type)index.data(NetworkModel::BufferTypeRole).toInt();
    bool isActive = index.data(NetworkModel::ItemActiveRole).toBool();

    if (role == Qt::DecorationRole) {
        return QVariant();
/*
        switch (type) {
        case BufferInfo::ChannelBuffer:
            if (isActive)
                return _channelJoinedIcon;
            else
                return _channelPartedIcon;
        case BufferInfo::QueryBuffer:
            if (!isActive)
                return _userOfflineIcon;
            if (index.data(NetworkModel::UserAwayRole).toBool())
                return _userAwayIcon;
            else
                return _userOnlineIcon;
        default:
            return QVariant();
        } */
    }

    quint32 fmtType = BufferViewItem;
    switch (type) {
    case BufferInfo::StatusBuffer:
        fmtType |= NetworkItem;
        break;
    case BufferInfo::ChannelBuffer:
        fmtType |= ChannelBufferItem;
        break;
    case BufferInfo::QueryBuffer:
        fmtType |= QueryBufferItem;
        break;
    default:
        return QVariant();
    }

    QTextCharFormat fmt = _listItemFormats.value(BufferViewItem);
    fmt.merge(_listItemFormats.value(fmtType));

    BufferInfo::ActivityLevel activity = (BufferInfo::ActivityLevel)index.data(NetworkModel::BufferActivityRole).toInt();
    if (activity & BufferInfo::Highlight) {
        fmt.merge(_listItemFormats.value(BufferViewItem | HighlightedBuffer));
        fmt.merge(_listItemFormats.value(fmtType | HighlightedBuffer));
    }
    else if (activity & BufferInfo::NewMessage) {
        fmt.merge(_listItemFormats.value(BufferViewItem | UnreadBuffer));
        fmt.merge(_listItemFormats.value(fmtType | UnreadBuffer));
    }
    else if (activity & BufferInfo::OtherActivity) {
        fmt.merge(_listItemFormats.value(BufferViewItem | ActiveBuffer));
        fmt.merge(_listItemFormats.value(fmtType | ActiveBuffer));
    }
    else if (!isActive) {
        fmt.merge(_listItemFormats.value(BufferViewItem | InactiveBuffer));
        fmt.merge(_listItemFormats.value(fmtType | InactiveBuffer));
    }
    else if (index.data(NetworkModel::UserAwayRole).toBool()) {
        fmt.merge(_listItemFormats.value(BufferViewItem | UserAway));
        fmt.merge(_listItemFormats.value(fmtType | UserAway));
    }
    //qDebug() << "xxxxx Bb10UiStyle::channelListViewItemData fmt.font = " << fmt.font().styleName();
    //qDebug() << "xxxxx Bb10UiStyle::channelListViewItemData fmt.foreground = " << fmt.property(QTextFormat::ForegroundBrush).value<QBrush>().color().name();
    //qDebug() << "xxxxx Bb10UiStyle::channelListViewItemData fmt.background = " << fmt.property(QTextFormat::BackgroundBrush).value<QBrush>().color().name();

    return itemData(role, fmt);
}

QVariant Bb10UiStyle::itemData(int role, const QTextCharFormat &format) const
{
    switch (role) {
    case Qt::FontRole:
        return format.font();
    case Qt::ForegroundRole:
        return format.property(QTextFormat::ForegroundBrush).value<QBrush>().color().rgba();
    case Qt::BackgroundRole:
        return format.property(QTextFormat::BackgroundBrush).value<QBrush>().color().rgba();
    default:
        return QVariant();
    }
}

void Bb10UiStyle::loadStyleSheet()
{
    qDeleteAll(_metricsCache);
    _metricsCache.clear();
    _formatCache.clear();
    _formats.clear();

    UiStyleSettings s;

    QString styleSheet;
    styleSheet += loadStyleSheet("file:///" + Quassel::findDataFilePath("stylesheets/jussi01-darktheme.qss"));
    styleSheet += loadStyleSheet("file:///" + Quassel::configDirPath() + "settings.qss");
    if (s.value("UseCustomStyleSheet", false).toBool())
        styleSheet += loadStyleSheet("file:///" + s.value("CustomStyleSheetPath").toString(), true);
    styleSheet += loadStyleSheet("file:///" + Quassel::optionValue("qss"), true);

    if (!styleSheet.isEmpty()) {
        QssParser parser;

        parser.processStyleSheet(styleSheet);

        QApplication::setPalette(parser.palette());

        _uiStylePalette = parser.uiStylePalette();
        _formats = parser.formats();
        _listItemFormats = parser.listItemFormats();
        styleSheet = styleSheet.trimmed();
        // this is for qApp, not needed for bb10
        //if (!styleSheet.isEmpty())
            //qApp->setStyleSheet(styleSheet);  // pass the remaining sections to the application
    }

    emit changed();
}


QString Bb10UiStyle::loadStyleSheet(const QString &styleSheet, bool shouldExist)
{
    QString ss = styleSheet;
    if (ss.startsWith("file:///")) {
        ss.remove(0, 8);
        if (ss.isEmpty())
            return QString();

        QFile file(ss);
        if (file.open(QFile::ReadOnly)) {
            QTextStream stream(&file);
            ss = stream.readAll();
            file.close();
        }
        else {
            if (shouldExist)
                qWarning() << "xxxxx Could not open stylesheet file:" << file.fileName();
            return QString();
        }
    }
    return ss;
}