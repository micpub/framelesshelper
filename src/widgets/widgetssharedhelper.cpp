/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "widgetssharedhelper_p.h"
#include <QtCore/qcoreevent.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qwindow.h>
#include <QtWidgets/qwidget.h>
#include <framelessconfig_p.h>
#include <micamaterial.h>
#include <micamaterial_p.h>
#include <utils.h>
#include <windowborderpainter.h>
#ifdef Q_OS_WINDOWS
#  include <winverhelper_p.h>
#endif // Q_OS_WINDOWS

FRAMELESSHELPER_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcWidgetsSharedHelper, "wangwenx190.framelesshelper.widgets.widgetssharedhelper")

#ifdef FRAMELESSHELPER_WIDGETS_NO_DEBUG_OUTPUT
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#else
#  define INFO qCInfo(lcWidgetsSharedHelper)
#  define DEBUG qCDebug(lcWidgetsSharedHelper)
#  define WARNING qCWarning(lcWidgetsSharedHelper)
#  define CRITICAL qCCritical(lcWidgetsSharedHelper)
#endif

using namespace Global;

WidgetsSharedHelper::WidgetsSharedHelper(QObject *parent) : QObject(parent)
{
}

WidgetsSharedHelper::~WidgetsSharedHelper() = default;

void WidgetsSharedHelper::setup(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    if (m_targetWidget && (m_targetWidget == widget)) {
        return;
    }
    m_targetWidget = widget;
    m_borderPainter = new WindowBorderPainter(this);
    if (m_borderRepaintConnection) {
        disconnect(m_borderRepaintConnection);
        m_borderRepaintConnection = {};
    }
    m_borderRepaintConnection = connect(m_borderPainter,
        &WindowBorderPainter::shouldRepaint, this, [this](){
            if (m_targetWidget) {
                m_targetWidget->update();
            }
        });
    m_micaMaterial = new MicaMaterial(this);
    if (m_micaRedrawConnection) {
        disconnect(m_micaRedrawConnection);
        m_micaRedrawConnection = {};
    }
    m_micaRedrawConnection = connect(m_micaMaterial, &MicaMaterial::shouldRedraw,
        this, [this](){
            if (m_targetWidget) {
                m_targetWidget->update();
            }
        });
    m_targetWidget->installEventFilter(this);
    updateContentsMargins();
    m_targetWidget->update();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QScreen *screen = m_targetWidget->screen();
#else
    QScreen *screen = m_targetWidget->windowHandle()->screen();
#endif
    handleScreenChanged(screen);
    if (m_screenChangeConnection) {
        disconnect(m_screenChangeConnection);
        m_screenChangeConnection = {};
    }
    m_screenChangeConnection = connect(m_targetWidget->windowHandle(),
        &QWindow::screenChanged, this, &WidgetsSharedHelper::handleScreenChanged);
}

bool WidgetsSharedHelper::isMicaEnabled() const
{
    return m_micaEnabled;
}

void WidgetsSharedHelper::setMicaEnabled(const bool value)
{
    if (m_micaEnabled == value) {
        return;
    }
    m_micaEnabled = value;
    if (m_targetWidget) {
        m_targetWidget->update();
    }
    Q_EMIT micaEnabledChanged();
}

MicaMaterial *WidgetsSharedHelper::rawMicaMaterial() const
{
    return m_micaMaterial;
}

WindowBorderPainter *WidgetsSharedHelper::rawWindowBorder() const
{
    return m_borderPainter;
}

bool WidgetsSharedHelper::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    if (!object || !event) {
        return false;
    }
    if (!m_targetWidget) {
        return QObject::eventFilter(object, event);
    }
    if (!object->isWidgetType()) {
        return QObject::eventFilter(object, event);
    }
    const auto widget = qobject_cast<QWidget *>(object);
    if (widget != m_targetWidget) {
        return QObject::eventFilter(object, event);
    }
    switch (event->type()) {
    case QEvent::Paint: {
        const auto paintEvent = static_cast<QPaintEvent *>(event);
        paintEventHandler(paintEvent);
    } break;
    case QEvent::WindowStateChange:
        changeEventHandler(event);
        break;
    case QEvent::Move:
    case QEvent::Resize:
        if (m_micaEnabled) {
            m_targetWidget->update();
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void WidgetsSharedHelper::changeEventHandler(QEvent *event)
{
    Q_ASSERT(event);
    if (!event) {
        return;
    }
    if (event->type() != QEvent::WindowStateChange) {
        return;
    }
    updateContentsMargins();
    if (const auto mo = m_targetWidget->metaObject()) {
        if (const int idx = mo->indexOfSignal(QMetaObject::normalizedSignature("hiddenChanged()").constData()); idx >= 0) {
            QMetaObject::invokeMethod(m_targetWidget, "hiddenChanged");
        }
        if (const int idx = mo->indexOfSignal(QMetaObject::normalizedSignature("normalChanged()").constData()); idx >= 0) {
            QMetaObject::invokeMethod(m_targetWidget, "normalChanged");
        }
        if (const int idx = mo->indexOfSignal(QMetaObject::normalizedSignature("zoomedChanged()").constData()); idx >= 0) {
            QMetaObject::invokeMethod(m_targetWidget, "zoomedChanged");
        }
    }
#ifdef Q_OS_WINDOWS
    if (!FramelessConfig::instance()->isSet(Option::UseCrossPlatformQtImplementation)) {
        if (Utils::windowStatesToWindowState(m_targetWidget->windowState()) != Qt::WindowFullScreen) {
            const auto changeEvent = static_cast<QWindowStateChangeEvent *>(event);
            if (Utils::windowStatesToWindowState(changeEvent->oldState()) == Qt::WindowFullScreen) {
                Utils::maybeFixupQtInternals(m_targetWidget->winId());
            }
        }
    }
#endif
}

void WidgetsSharedHelper::paintEventHandler(QPaintEvent *event)
{
    Q_ASSERT(event);
    if (!event) {
        return;
    }
    if (m_micaEnabled && m_micaMaterial) {
        QPainter painter(m_targetWidget);
        m_micaMaterial->paint(&painter, m_targetWidget->size(),
            m_targetWidget->mapToGlobal(QPoint(0, 0)));
    }
    if ((Utils::windowStatesToWindowState(m_targetWidget->windowState()) == Qt::WindowNoState)
            && m_borderPainter) {
        QPainter painter(m_targetWidget);
        m_borderPainter->paint(&painter, m_targetWidget->size(), m_targetWidget->isActiveWindow());
    }
    // Don't eat this event here, we need Qt to keep dispatching this paint event
    // otherwise the widget won't paint anything else from the user side.
}

void WidgetsSharedHelper::handleScreenChanged(QScreen *screen)
{
    Q_ASSERT(m_targetWidget);
    if (!m_targetWidget) {
        return;
    }
    // The QScreen handle can be null if a window was moved out of a screen.
    if (!screen) {
        return;
    }
    if (m_screen == screen) {
        return;
    }
    m_screen = screen;
    m_screenDpr = m_screen->devicePixelRatio();
    if (m_screenDpiChangeConnection) {
        disconnect(m_screenDpiChangeConnection);
        m_screenDpiChangeConnection = {};
    }
    m_screenDpiChangeConnection = connect(m_screen, &QScreen::physicalDotsPerInchChanged,
        this, [this](const qreal dpi){
            Q_UNUSED(dpi);
            const qreal currentDpr = m_screen->devicePixelRatio();
            if (m_screenDpr == currentDpr) {
                return;
            }
            m_screenDpr = currentDpr;
            if (m_micaEnabled && m_micaMaterial) {
                MicaMaterialPrivate::get(m_micaMaterial)->maybeGenerateBlurredWallpaper(true);
            }
        });
}

void WidgetsSharedHelper::updateContentsMargins()
{
#ifdef Q_OS_WINDOWS
    const auto margins = [this]() -> QMargins {
        if (!Utils::isWindowFrameBorderVisible() || WindowsVersionHelper::isWin11OrGreater()) {
            return {};
        }
        if (Utils::windowStatesToWindowState(m_targetWidget->windowState()) != Qt::WindowNoState) {
            return {};
        }
        return {0, kDefaultWindowFrameBorderThickness, 0, 0};
    }();
    m_targetWidget->setContentsMargins(margins);
#endif
}

FRAMELESSHELPER_END_NAMESPACE
