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

#ifndef QTUISTYLE_H_
#define QTUISTYLE_H_

#include "message.h"
#include "uisettings.h"

class Bb10UiStyle : public QObject
{
    Q_OBJECT

public:
    typedef QList<QPair<quint16, quint32> > FormatList;
    //! This enumerates the possible formats a text element may have. */
    /** These formats are ordered on increasing importance, in cases where a given property is specified
     *  by multiple active formats.
     *  \NOTE: Do not change/add values here without also adapting the relevant
     *         methods in this class (in particular mergedFormat())!
     *         Also, we _do_ rely on certain properties of these values in styleString() and friends!
     */
    enum FormatType {
        Base            = 0x00000000,
        Invalid         = 0xffffffff,

        // Message Formats (mutually exclusive!)
        PlainMsg        = 0x00000001,
        NoticeMsg       = 0x00000002,
        ActionMsg       = 0x00000003,
        NickMsg         = 0x00000004,
        ModeMsg         = 0x00000005,
        JoinMsg         = 0x00000006,
        PartMsg         = 0x00000007,
        QuitMsg         = 0x00000008,
        KickMsg         = 0x00000009,
        KillMsg         = 0x0000000a,
        ServerMsg       = 0x0000000b,
        InfoMsg         = 0x0000000c,
        ErrorMsg        = 0x0000000d,
        DayChangeMsg    = 0x0000000e,
        TopicMsg        = 0x0000000f,
        NetsplitJoinMsg = 0x00000010,
        NetsplitQuitMsg = 0x00000020,
        InviteMsg       = 0x00000030,

        // Standard Formats
        Bold            = 0x00000100,
        Italic          = 0x00000200,
        Underline       = 0x00000400,
        Reverse         = 0x00000800,

        // Individual parts of a message
        Timestamp       = 0x00001000,
        Sender          = 0x00002000,
        Contents        = 0x00004000,
        Nick            = 0x00008000,
        Hostmask        = 0x00010000,
        ChannelName     = 0x00020000,
        ModeFlags       = 0x00040000,

        // URL is special, we want that to take precedence over the rest...
        Url             = 0x00080000

                          // mIRC Colors - we assume those to be present only in plain contents
                          // foreground: 0x0.400000
                          // background: 0x.0800000
    };

    enum MessageLabel {
        OwnMsg          = 0x00000001,
        Highlight       = 0x00000002,
        Selected        = 0x00000004 // must be last!
    };

    enum ItemFormatType {
        BufferViewItem    = 0x00000001,
        NickViewItem      = 0x00000002,

        NetworkItem       = 0x00000010,
        ChannelBufferItem = 0x00000020,
        QueryBufferItem   = 0x00000040,
        IrcUserItem       = 0x00000080,
        UserCategoryItem  = 0x00000100,

        InactiveBuffer    = 0x00001000,
        ActiveBuffer      = 0x00002000,
        UnreadBuffer      = 0x00004000,
        HighlightedBuffer = 0x00008000,
        UserAway          = 0x00010000
    };

    Bb10UiStyle(QObject *parent = 0);
    virtual ~Bb10UiStyle();

    QVariant channelListViewItemData(const QModelIndex &networkModelIndex, int role) const;
    virtual inline qreal firstColumnSeparator() const { return 6; }
    virtual inline qreal secondColumnSeparator() const { return 6; }
    void loadStyleSheet();
    QString loadStyleSheet(const QString &name, bool shouldExist = false);

    struct StyledString {
        QString plainText;
        QString formatedHtml;
        FormatList formatList; // starting pos, ftypes
    };
    class StyledMessage;

    static FormatType formatType(Message::Type msgType);
    static StyledString styleString(const QString &string, quint32 baseFormat = Base);
    static QString mircToInternal(const QString &);
    static inline QString timestampFormatString() { return _timestampFormatString; }

    QTextCharFormat format(quint32 formatType, quint32 messageLabel) const;
public slots:
    void generateSettingsQss() const;
signals:
    void changed();

private slots:
    void updateTimestampFormatString();

protected:
    QString fontDescription(const QFont &font) const;
    QString color(const QString &key, UiSettings &settings) const;

    QTextCharFormat format(quint64 key) const;
    QTextCharFormat cachedFormat(quint32 formatType, quint32 messageLabel) const;
    void setCachedFormat(const QTextCharFormat &format, quint32 formatType, quint32 messageLabel) const;
    void mergeFormat(QTextCharFormat &format, quint32 formatType, quint64 messageLabel) const;
    void mergeSubElementFormat(QTextCharFormat &format, quint32 formatType, quint64 messageLabel) const;

    QString msgTypeQss(const QString &msgType, const QString &key, UiSettings &settings) const;
    QString senderQss(int i, UiSettings &settings) const;
    QString chatListItemQss(const QString &state, const QString &key, UiSettings &settings) const;

    static FormatType formatType(const QString &code);
    static QString formatCode(FormatType);
    static void setTimestampFormatString(const QString &format);

    QVariant itemData(int role, const QTextCharFormat &format) const;
private:
    QVector<QBrush> _uiStylePalette;
    QHash<quint64, QTextCharFormat> _formats;
    mutable QHash<quint64, QTextCharFormat> _formatCache;
    mutable QHash<quint64, QFontMetricsF *> _metricsCache;
    QHash<quint32, QTextCharFormat> _listItemFormats;
    static QString _timestampFormatString;
    static QHash<QString, FormatType> _formatCodes;
    bool _allowMircColors;
};

class Bb10UiStyle::StyledMessage : public Message
{
    Q_DECLARE_TR_FUNCTIONS(UiStyle::StyledMessage)

public:
    explicit StyledMessage(const Message &message);

    QString decoratedTimestamp() const;
    QString plainSender() const;           //!< Nickname (no decorations) for Plain and Notice, empty else
    QString decoratedSender() const;
    const QString &plainContents() const;
    const QString& formatedHtmlContents() const;

    const FormatList &contentsFormatList() const;

    quint8 senderHash() const;

protected:
    void style() const;

private:
    mutable StyledString _contents;
    mutable quint8 _senderHash;
};
#endif
