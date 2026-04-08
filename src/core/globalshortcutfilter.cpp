// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "globalshortcutfilter.h"
#include "core/flameshot.h"
#include "utils/confighandler.h"

#include <QKeySequence>
#include <qt_windows.h>

GlobalShortcutFilter::GlobalShortcutFilter(QObject* parent)
  : QObject(parent)
{
    ConfigHandler config;

    // Register capture hotkey (default: Print Screen)
    QString captureStr = config.shortcut("PRINT_SCREEN_CAPTURE");
    if (!captureStr.isEmpty()) {
        QKeySequence seq(captureStr);
        if (!seq.isEmpty()) {
            int combined = seq[0].toCombined();
            Qt::Key key =
              static_cast<Qt::Key>(combined & ~Qt::KeyboardModifierMask);
            Qt::KeyboardModifiers mods =
              static_cast<Qt::KeyboardModifiers>(combined &
                                                 Qt::KeyboardModifierMask);
            m_captureKeycode = nativeKeycode(key);
            m_captureModifiers = getNativeModifier(mods);
            if (RegisterHotKey(NULL,
                               HOTKEY_ID_CAPTURE,
                               m_captureModifiers,
                               m_captureKeycode)) {
                m_captureRegistered = true;
            }
        }
    }

    // Register history hotkey (default: Shift+Print Screen)
    QString historyStr = config.shortcut("PRINT_SCREEN_HISTORY");
    if (!historyStr.isEmpty()) {
        QKeySequence seq(historyStr);
        if (!seq.isEmpty()) {
            int combined = seq[0].toCombined();
            Qt::Key key =
              static_cast<Qt::Key>(combined & ~Qt::KeyboardModifierMask);
            Qt::KeyboardModifiers mods =
              static_cast<Qt::KeyboardModifiers>(combined &
                                                 Qt::KeyboardModifierMask);
            m_historyKeycode = nativeKeycode(key);
            m_historyModifiers = getNativeModifier(mods);
            if (RegisterHotKey(NULL,
                               HOTKEY_ID_HISTORY,
                               m_historyModifiers,
                               m_historyKeycode)) {
                m_historyRegistered = true;
            }
        }
    }
}

GlobalShortcutFilter::~GlobalShortcutFilter()
{
    if (m_captureRegistered) {
        UnregisterHotKey(NULL, HOTKEY_ID_CAPTURE);
    }
    if (m_historyRegistered) {
        UnregisterHotKey(NULL, HOTKEY_ID_HISTORY);
    }
}

bool GlobalShortcutFilter::nativeEventFilter(const QByteArray& eventType,
                                             void* message,
                                             qintptr* result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);

#ifdef ENABLE_IMGUR
        // Show screenshots history
        if (m_historyRegistered && keycode == m_historyKeycode &&
            modifiers == m_historyModifiers) {
            Flameshot::instance()->history();
            return true;
        }
#endif
        // Capture screen
        if (m_captureRegistered && keycode == m_captureKeycode &&
            modifiers == m_captureModifiers) {
            Flameshot::instance()->requestCapture(
              CaptureRequest(CaptureRequest::GRAPHICAL_MODE));
            return true;
        }
    }
    return false;
}

quint32 GlobalShortcutFilter::getNativeModifier(
  Qt::KeyboardModifiers modifiers)
{
    quint32 nativeMods = 0;
    if (modifiers & Qt::ShiftModifier) {
        nativeMods |= MOD_SHIFT;
    }
    if (modifiers & Qt::ControlModifier) {
        nativeMods |= MOD_CONTROL;
    }
    if (modifiers & Qt::AltModifier) {
        nativeMods |= MOD_ALT;
    }
    if (modifiers & Qt::MetaModifier) {
        nativeMods |= MOD_WIN;
    }
    return nativeMods;
}

quint32 GlobalShortcutFilter::nativeKeycode(Qt::Key key)
{
    // Special keys
    switch (key) {
    case Qt::Key_Print:
        return VK_SNAPSHOT;
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Tab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    default:
        break;
    }

    // Alphanumeric keys: Qt::Key values for A-Z and 0-9 match ASCII/VK codes
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return static_cast<quint32>(key);
    }
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return static_cast<quint32>(key);
    }

    return 0;
}
