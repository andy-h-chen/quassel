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

#include "bb10uistyle.h"

#include <bb/cascades/Application>
#include <QFile>
#include <QTextStream>

#include "bb10ui.h"
#include "bb10uisettings.h"
#include "clickable.h"
#include "qssparser.h"

QHash<QString, Bb10UiStyle::FormatType> Bb10UiStyle::_formatCodes;
QString Bb10UiStyle::_timestampFormatString;

Bb10UiStyle::Bb10UiStyle(QObject *parent) : QObject(parent)
{
    // Now initialize the mapping between FormatCodes and FormatTypes...
    _formatCodes["%O"] = Base;
    _formatCodes["%B"] = Bold;
    _formatCodes["%S"] = Italic;
    _formatCodes["%U"] = Underline;
    _formatCodes["%R"] = Reverse;

    _formatCodes["%DN"] = Nick;
    _formatCodes["%DH"] = Hostmask;
    _formatCodes["%DC"] = ChannelName;
    _formatCodes["%DM"] = ModeFlags;
    _formatCodes["%DU"] = Url;

    Bb10UiSettings s;
    s.notify("TimestampFormat", this, SLOT(updateTimestampFormatString()));
    updateTimestampFormatString();
    loadStyleSheet();
}


Bb10UiStyle::~Bb10UiStyle() {
    qDeleteAll(_metricsCache);
}

void Bb10UiStyle::updateTimestampFormatString()
{
    Bb10UiSettings s;
    setTimestampFormatString(s.timestampFormatString());
}

void Bb10UiStyle::setTimestampFormatString(const QString &format)
{
    if (_timestampFormatString != format) {
        _timestampFormatString = format;
        // FIXME reload
    }
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

Bb10UiStyle::FormatType Bb10UiStyle::formatType(Message::Type msgType)
{
    switch (msgType) {
    case Message::Plain:
        return PlainMsg;
    case Message::Notice:
        return NoticeMsg;
    case Message::Action:
        return ActionMsg;
    case Message::Nick:
        return NickMsg;
    case Message::Mode:
        return ModeMsg;
    case Message::Join:
        return JoinMsg;
    case Message::Part:
        return PartMsg;
    case Message::Quit:
        return QuitMsg;
    case Message::Kick:
        return KickMsg;
    case Message::Kill:
        return KillMsg;
    case Message::Server:
        return ServerMsg;
    case Message::Info:
        return InfoMsg;
    case Message::Error:
        return ErrorMsg;
    case Message::DayChange:
        return DayChangeMsg;
    case Message::Topic:
        return TopicMsg;
    case Message::NetsplitJoin:
        return NetsplitJoinMsg;
    case Message::NetsplitQuit:
        return NetsplitQuitMsg;
    case Message::Invite:
        return InviteMsg;
    }
    //Q_ASSERT(false); // we need to handle all message types
    qWarning() << Q_FUNC_INFO << "Unknown message type:" << msgType;
    return ErrorMsg;
}


Bb10UiStyle::FormatType Bb10UiStyle::formatType(const QString &code)
{
    if (_formatCodes.contains(code)) return _formatCodes.value(code);
    return Invalid;
}


QString Bb10UiStyle::formatCode(FormatType ftype)
{
    return _formatCodes.key(ftype);
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

QString Bb10UiStyle::mircToInternal(const QString &mirc_)
{
    QString mirc;
    mirc.reserve(mirc_.size());
    foreach (const QChar &c, mirc_) {
        if ((c < '\x20' || c == '\x7f') && c != '\x03') {
            switch (c.unicode()) {
                case '\x02':
                    mirc += "%B";
                    break;
                case '\x0f':
                    mirc += "%O";
                    break;
                case '\x09':
                    mirc += "        ";
                    break;
                case '\x12':
                case '\x16':
                    mirc += "%R";
                    break;
                case '\x1d':
                    mirc += "%S";
                    break;
                case '\x1f':
                    mirc += "%U";
                    break;
                case '\x7f':
                    mirc += QChar(0x2421);
                    break;
                default:
                    mirc += QChar(0x2400 + c.unicode());
            }
        } else {
            if (c == '%')
                mirc += c;
            mirc += c;
        }
    }

    // Now we bring the color codes (\x03) in a sane format that can be parsed more easily later.
    // %Dcfxx is foreground, %Dcbxx is background color, where xx is a 2 digit dec number denoting the color code.
    // %Dc- turns color off.
    // Note: We use the "mirc standard" as described in <http://www.mirc.co.uk/help/color.txt>.
    //       This means that we don't accept something like \x03,5 (even though others, like WeeChat, do).
    int pos = 0;
    for (;;) {
        pos = mirc.indexOf('\x03', pos);
        if (pos < 0) break;  // no more mirc color codes
        QString ins, num;
        int l = mirc.length();
        int i = pos + 1;
        // check for fg color
        if (i < l && mirc[i].isDigit()) {
            num = mirc[i++];
            if (i < l && mirc[i].isDigit()) num.append(mirc[i++]);
            else num.prepend('0');
            ins = QString("%Dcf%1").arg(num);

            if (i+1 < l && mirc[i] == ',' && mirc[i+1].isDigit()) {
                i++;
                num = mirc[i++];
                if (i < l && mirc[i].isDigit()) num.append(mirc[i++]);
                else num.prepend('0');
                ins += QString("%Dcb%1").arg(num);
            }
        }
        else {
            ins = "%Dc-";
        }
        mirc.replace(pos, i-pos, ins);
    }
    return mirc;
}

// This method expects a well-formatted string, there is no error checking!
// Since we create those ourselves, we should be pretty safe that nobody does something crappy here.
Bb10UiStyle::StyledString Bb10UiStyle::styleString(const QString &s_, quint32 baseFormat)
{
    QString s = s_;
    if (s.length() > 65535) {
        qWarning() << QString("String too long to be styled: %1").arg(s);
        return StyledString();
    }
    StyledString result;
    result.formatList.append(qMakePair((quint16)0, baseFormat));
    quint32 curfmt = baseFormat;
    int pos = 0; quint16 length = 0;
    for (;;) {
        pos = s.indexOf('%', pos);
        if (pos < 0) break;
        if (s[pos+1] == '%') { // escaped %, we just remove one and continue
            s.remove(pos, 1);
            pos++;
            continue;
        }
        if (s[pos+1] == 'D' && s[pos+2] == 'c') { // color code
            if (s[pos+3] == '-') { // color off
                curfmt &= 0x003fffff;
                length = 4;
            }
            else {
                int color = 10 * s[pos+4].digitValue() + s[pos+5].digitValue();
                //TODO: use 99 as transparent color (re mirc color "standard")
                color &= 0x0f;
                if (s[pos+3] == 'f') {
                    curfmt &= 0xf0ffffff;
                    curfmt |= (quint32)(color << 24) | 0x00400000;
                }
                else {
                    curfmt &= 0x0fffffff;
                    curfmt |= (quint32)(color << 28) | 0x00800000;
                }
                length = 6;
            }
        }
        else if (s[pos+1] == 'O') { // reset formatting
            curfmt &= 0x000000ff; // we keep message type-specific formatting
            length = 2;
        }
        else if (s[pos+1] == 'R') { // reverse
            // TODO: implement reverse formatting

            length = 2;
        }
        else { // all others are toggles
            QString code = QString("%") + s[pos+1];
            if (s[pos+1] == 'D') code += s[pos+2];
            FormatType ftype = formatType(code);
            if (ftype == Invalid) {
                pos++;
                qWarning() << (QString("Invalid format code in string: %1").arg(s));
                continue;
            }
            curfmt ^= ftype;
            length = code.length();
        }
        s.remove(pos, length);
        if (pos == result.formatList.last().first)
            result.formatList.last().second = curfmt;
        else
            result.formatList.append(qMakePair((quint16)pos, curfmt));
    }
    result.plainText = s;
    ClickableList cl = ClickableList::fromString(s);
    bool foundUrl = false;
    for (int i=0; i<cl.size(); i++) {
        Clickable c = cl.at(i);
        if (c.type() == Clickable::Url) {
            QString str = s.mid(c.start(), c.length());
            QString link = !str.contains("://") ? "http://" + str : str;
            s.replace(str, "<a href='" + link + "'>" + str.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;") + "</a>");
            foundUrl = true;
            break;  // Assume only one link in each message
        }
    }
    QTextCharFormat baseFmt = Bb10Ui::uiStyle()->format(result.formatList.first().second, 0);
    result.formatedHtml = "<span style='color:" + baseFmt.foreground().color().name() + ";'>";;
    int prevPos = result.formatList.first().first;
    for (int i=1; i<result.formatList.size(); i++) {
        int pos = result.formatList.at(i).first;
        QTextCharFormat fmt = Bb10Ui::uiStyle()->format(result.formatList.at(i).second, 0);
        //result.formatedHtml += "<span style='background-color:" + fmt.background().color().name() + "; color:" + fmt.foreground().color().name() + ";'>";
        result.formatedHtml += "<span style='color:" + fmt.foreground().color().name() + ";'>";
        if (foundUrl)
            result.formatedHtml += s.mid(prevPos, pos - prevPos);
        else
            result.formatedHtml += s.mid(prevPos, pos - prevPos).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
        result.formatedHtml += "</span>";
        prevPos = pos;
    }
    
    if (prevPos < s.length()) {
        if (foundUrl)
            result.formatedHtml += s.right(s.length() - prevPos);
        else
            result.formatedHtml += s.right(s.length() - prevPos).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;");
    }
    result.formatedHtml += "</span>";
    return result;
}


/******** Caching *******/

QTextCharFormat Bb10UiStyle::format(quint64 key) const
{
    return _formats.value(key, QTextCharFormat());
}


QTextCharFormat Bb10UiStyle::cachedFormat(quint32 formatType, quint32 messageLabel) const
{
    return _formatCache.value(formatType | ((quint64)messageLabel << 32), QTextCharFormat());
}


void Bb10UiStyle::setCachedFormat(const QTextCharFormat &format, quint32 formatType, quint32 messageLabel) const
{
    _formatCache[formatType | ((quint64)messageLabel << 32)] = format;
}

// NOTE: This and the following functions are intimately tied to the values in FormatType. Don't change this
//       until you _really_ know what you do!
QTextCharFormat Bb10UiStyle::format(quint32 ftype, quint32 label_) const
{
    if (ftype == Invalid)
        return QTextCharFormat();

    quint64 label = (quint64)label_ << 32;

    // check if we have exactly this format readily cached already
    QTextCharFormat fmt = cachedFormat(ftype, label_);
    if (fmt.properties().count())
        return fmt;

    mergeFormat(fmt, ftype, label & Q_UINT64_C(0xffff000000000000));

    for (quint64 mask = Q_UINT64_C(0x0000000100000000); mask <= (quint64)Selected << 32; mask <<= 1) {
        if (label & mask)
            mergeFormat(fmt, ftype, mask | Q_UINT64_C(0xffff000000000000));
    }

    setCachedFormat(fmt, ftype, label_);
    return fmt;
}

void Bb10UiStyle::mergeFormat(QTextCharFormat &fmt, quint32 ftype, quint64 label) const
{
    mergeSubElementFormat(fmt, ftype & 0x00ff, label);

    // TODO: allow combinations for mirc formats and colors (each), e.g. setting a special format for "bold and italic"
    //       or "foreground 01 and background 03"
    if ((ftype & 0xfff00)) { // element format
        for (quint32 mask = 0x00100; mask <= 0x40000; mask <<= 1) {
            if (ftype & mask) {
                mergeSubElementFormat(fmt, ftype & (mask | 0xff), label);
            }
        }
    }

    // Now we handle color codes
    // We assume that those can't be combined with subelement and message types.
    if (_allowMircColors) {
        if (ftype & 0x00400000)
            mergeSubElementFormat(fmt, ftype & 0x0f400000, label);  // foreground
        if (ftype & 0x00800000)
            mergeSubElementFormat(fmt, ftype & 0xf0800000, label);  // background
        if ((ftype & 0x00c00000) == 0x00c00000)
            mergeSubElementFormat(fmt, ftype & 0xffc00000, label);  // combination
    }

    // URL
    if (ftype & Url)
        mergeSubElementFormat(fmt, ftype & (Url | 0x000000ff), label);
}


// Merge a subelement format into an existing message format
void Bb10UiStyle::mergeSubElementFormat(QTextCharFormat &fmt, quint32 ftype, quint64 label) const
{
    quint64 key = ftype | label;
    fmt.merge(format(key & Q_UINT64_C(0x0000ffffffffff00))); // label + subelement
    fmt.merge(format(key & Q_UINT64_C(0x0000ffffffffffff))); // label + subelement + msgtype
    fmt.merge(format(key & Q_UINT64_C(0xffffffffffffff00))); // label + subelement + nickhash
    fmt.merge(format(key & Q_UINT64_C(0xffffffffffffffff))); // label + subelement + nickhash + msgtype
}


/***********************************************************************************/
Bb10UiStyle::StyledMessage::StyledMessage(const Message &msg)
    : Message(msg)
{
    if (type() == Message::Plain)
        _senderHash = 0xff;
    else
        _senderHash = 0x00;  // this means we never compute the hash for msgs that aren't plain
}


void Bb10UiStyle::StyledMessage::style() const
{
    QString user = userFromMask(sender());
    QString host = hostFromMask(sender());
    QString nick = nickFromMask(sender());
    QString txt = Bb10UiStyle::mircToInternal(contents());
    QString bufferName = bufferInfo().bufferName();
    bufferName.replace('%', "%%"); // well, you _can_ have a % in a buffername apparently... -_-
    host.replace('%', "%%");     // hostnames too...
    user.replace('%', "%%");     // and the username...
    nick.replace('%', "%%");     // ... and then there's totally RFC-violating servers like justin.tv m(
    const int maxNetsplitNicks = 15;

    QString t;
    switch (type()) {
    case Message::Plain:
        //: Plain Message
        t = tr("%1").arg(txt); break;
    case Message::Notice:
        //: Notice Message
        t = tr("%1").arg(txt); break;
    case Message::Action:
        //: Action Message
        t = tr("%DN%1%DN %2").arg(nick).arg(txt);
        break;
    case Message::Nick:
        //: Nick Message
        if (nick == contents()) t = tr("You are now known as %DN%1%DN").arg(txt);
        else t = tr("%DN%1%DN is now known as %DN%2%DN").arg(nick, txt);
        break;
    case Message::Mode:
        //: Mode Message
        if (nick.isEmpty()) t = tr("User mode: %DM%1%DM").arg(txt);
        else t = tr("Mode %DM%1%DM by %DN%2%DN").arg(txt, nick);
        break;
    case Message::Join:
        //: Join Message
        t = tr("%DN%1%DN %DH(%2@%3)%DH has joined %DC%4%DC").arg(nick, user, host, bufferName); break;
    case Message::Part:
        //: Part Message
        t = tr("%DN%1%DN %DH(%2@%3)%DH has left %DC%4%DC").arg(nick, user, host, bufferName);
        if (!txt.isEmpty()) t = QString("%1 (%2)").arg(t).arg(txt);
        break;
    case Message::Quit:
        //: Quit Message
        t = tr("%DN%1%DN %DH(%2@%3)%DH has quit").arg(nick, user, host);
        if (!txt.isEmpty()) t = QString("%1 (%2)").arg(t).arg(txt);
        break;
    case Message::Kick:
    {
        QString victim = txt.section(" ", 0, 0);
        QString kickmsg = txt.section(" ", 1);
        //: Kick Message
        t = tr("%DN%1%DN has kicked %DN%2%DN from %DC%3%DC").arg(nick).arg(victim).arg(bufferName);
        if (!kickmsg.isEmpty()) t = QString("%1 (%2)").arg(t).arg(kickmsg);
    }
    break;
    //case Message::Kill: FIXME

    case Message::Server:
        //: Server Message
        t = tr("%1").arg(txt); break;
    case Message::Info:
        //: Info Message
        t = tr("%1").arg(txt); break;
    case Message::Error:
        //: Error Message
        t = tr("%1").arg(txt); break;
    case Message::DayChange:
    {
        //: Day Change Message
        t = tr("{Day changed to %1}").arg(timestamp().date().toString(Qt::DefaultLocaleLongDate));
    }
        break;
    case Message::Topic:
        //: Topic Message
        t = tr("%1").arg(txt); break;
    case Message::NetsplitJoin:
    {
        QStringList users = txt.split("#:#");
        QStringList servers = users.takeLast().split(" ");

        for (int i = 0; i < users.count() && i < maxNetsplitNicks; i++)
            users[i] = nickFromMask(users.at(i));

        t = tr("Netsplit between %DH%1%DH and %DH%2%DH ended. Users joined: ").arg(servers.at(0), servers.at(1));
        if (users.count() <= maxNetsplitNicks)
            t.append(QString("%DN%1%DN").arg(users.join(", ")));
        else
            t.append(tr("%DN%1%DN (%2 more)").arg(static_cast<QStringList>(users.mid(0, maxNetsplitNicks)).join(", ")).arg(users.count() - maxNetsplitNicks));
    }
    break;
    case Message::NetsplitQuit:
    {
        QStringList users = txt.split("#:#");
        QStringList servers = users.takeLast().split(" ");

        for (int i = 0; i < users.count() && i < maxNetsplitNicks; i++)
            users[i] = nickFromMask(users.at(i));

        t = tr("Netsplit between %DH%1%DH and %DH%2%DH. Users quit: ").arg(servers.at(0), servers.at(1));

        if (users.count() <= maxNetsplitNicks)
            t.append(QString("%DN%1%DN").arg(users.join(", ")));
        else
            t.append(tr("%DN%1%DN (%2 more)").arg(static_cast<QStringList>(users.mid(0, maxNetsplitNicks)).join(", ")).arg(users.count() - maxNetsplitNicks));
    }
    break;
    case Message::Invite:
        //: Invite Message
        t = tr("%1").arg(txt); break;
    default:
        t = tr("[%1]").arg(txt);
    }
    _contents = Bb10UiStyle::styleString(t, Bb10UiStyle::formatType(type()));
}

const QString &Bb10UiStyle::StyledMessage::plainContents() const
{
    if (_contents.plainText.isNull())
        style();

    return _contents.plainText;
}

const QString& Bb10UiStyle::StyledMessage::formatedHtmlContents() const
{
    if (_contents.plainText.isNull())
        style();

    return _contents.formatedHtml;
}

const Bb10UiStyle::FormatList &Bb10UiStyle::StyledMessage::contentsFormatList() const
{
    if (_contents.plainText.isNull())
        style();

    return _contents.formatList;
}


QString Bb10UiStyle::StyledMessage::decoratedTimestamp() const
{
    return timestamp().toLocalTime().toString(Bb10UiStyle::timestampFormatString());
}


QString Bb10UiStyle::StyledMessage::plainSender() const
{
    switch (type()) {
    case Message::Plain:
    case Message::Notice:
        return nickFromMask(sender());
    default:
        return QString();
    }
}


QString Bb10UiStyle::StyledMessage::decoratedSender() const
{
    switch (type()) {
    case Message::Plain:
        return tr("<%1>").arg(plainSender()); break;
    case Message::Notice:
        return tr("[%1]").arg(plainSender()); break;
    case Message::Action:
        return "-*-"; break;
    case Message::Nick:
        return "<->"; break;
    case Message::Mode:
        return "***"; break;
    case Message::Join:
        return "-->"; break;
    case Message::Part:
        return "<--"; break;
    case Message::Quit:
        return "<--"; break;
    case Message::Kick:
        return "<-*"; break;
    case Message::Kill:
        return "<-x"; break;
    case Message::Server:
        return "*"; break;
    case Message::Info:
        return "*"; break;
    case Message::Error:
        return "*"; break;
    case Message::DayChange:
        return "-"; break;
    case Message::Topic:
        return "*"; break;
    case Message::NetsplitJoin:
        return "=>"; break;
    case Message::NetsplitQuit:
        return "<="; break;
    case Message::Invite:
        return "->"; break;
    default:
        return QString("%1").arg(plainSender());
    }
}


// FIXME hardcoded to 16 sender hashes
quint8 Bb10UiStyle::StyledMessage::senderHash() const
{
    if (_senderHash != 0xff)
        return _senderHash;

    QString nick = nickFromMask(sender()).toLower();
    if (!nick.isEmpty()) {
        int chopCount = 0;
        while (chopCount < nick.size() && nick.at(nick.count() - 1 - chopCount) == '_')
            chopCount++;
        if (chopCount < nick.size())
            nick.chop(chopCount);
    }
    quint16 hash = qChecksum(nick.toAscii().data(), nick.toAscii().size());
    return (_senderHash = (hash & 0xf) + 1);
}


/***********************************************************************************/
