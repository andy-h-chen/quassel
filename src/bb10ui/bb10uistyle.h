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

#include "uisettings.h"

class Bb10UiStyle : public QObject
{
    Q_OBJECT

public:
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

public slots:
    void generateSettingsQss() const;
signals:
    void changed();

private slots:
    void updateTimestampFormatString();

private:
    QString fontDescription(const QFont &font) const;
    QString color(const QString &key, UiSettings &settings) const;

    QString msgTypeQss(const QString &msgType, const QString &key, UiSettings &settings) const;
    QString senderQss(int i, UiSettings &settings) const;
    QString chatListItemQss(const QString &state, const QString &key, UiSettings &settings) const;
    QVariant itemData(int role, const QTextCharFormat &format) const;
private:
    QVector<QBrush> _uiStylePalette;
    QHash<quint64, QTextCharFormat> _formats;
    mutable QHash<quint64, QTextCharFormat> _formatCache;
    mutable QHash<quint64, QFontMetricsF *> _metricsCache;
    QHash<quint32, QTextCharFormat> _listItemFormats;
};


#endif
