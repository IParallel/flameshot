// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

class GlobalShortcutFilter
  : public QObject
  , public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit GlobalShortcutFilter(QObject* parent = nullptr);
    ~GlobalShortcutFilter();

    bool nativeEventFilter(const QByteArray& eventType,
                           void* message,
                           qintptr* result);

private:
    quint32 getNativeModifier(Qt::KeyboardModifiers modifiers);
    quint32 nativeKeycode(Qt::Key key);

    static constexpr int HOTKEY_ID_CAPTURE = 1;
    static constexpr int HOTKEY_ID_HISTORY = 2;

    quint32 m_captureKeycode = 0;
    quint32 m_captureModifiers = 0;
    bool m_captureRegistered = false;

    quint32 m_historyKeycode = 0;
    quint32 m_historyModifiers = 0;
    bool m_historyRegistered = false;
};
