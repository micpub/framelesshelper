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

#include "quickmicamaterial.h"
#include "quickmicamaterial_p.h"
#include <micamaterial.h>
#include <QtCore/qmutex.h>
#include <QtGui/qscreen.h>
#include <QtGui/qpainter.h>
#include <QtGui/qguiapplication.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qsgsimpletexturenode.h>
#ifndef FRAMELESSHELPER_QUICK_NO_PRIVATE
#  include <QtQuick/private/qquickitem_p.h>
#endif // FRAMELESSHELPER_QUICK_NO_PRIVATE

FRAMELESSHELPER_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQuickMicaMaterial, "wangwenx190.framelesshelper.quick.quickmicamaterial")

#ifdef FRAMELESSHELPER_QUICK_NO_DEBUG_OUTPUT
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#else
#  define INFO qCInfo(lcQuickMicaMaterial)
#  define DEBUG qCDebug(lcQuickMicaMaterial)
#  define WARNING qCWarning(lcQuickMicaMaterial)
#  define CRITICAL qCCritical(lcQuickMicaMaterial)
#endif

using namespace Global;

struct QuickMicaData
{
    QMutex mutex;
};

Q_GLOBAL_STATIC(QuickMicaData, g_data)

class WallpaperImageNode : public QObject, public QSGTransformNode
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(WallpaperImageNode)

public:
    explicit WallpaperImageNode(QuickMicaMaterial *item);
    ~WallpaperImageNode() override;

public Q_SLOTS:
    void maybeUpdateWallpaperImageClipRect();
    void maybeGenerateWallpaperImageCache(const bool force = false);

private:
    void initialize();

private:
    QSGTexture *m_texture = nullptr;
    QPointer<QuickMicaMaterial> m_item = nullptr;
    QSGSimpleTextureNode *m_node = nullptr;
    QPixmap m_pixmapCache = {};
    MicaMaterial *m_micaMaterial = nullptr;
};

WallpaperImageNode::WallpaperImageNode(QuickMicaMaterial *item)
{
    Q_ASSERT(item);
    if (!item) {
        return;
    }
    m_item = item;
    initialize();
}

WallpaperImageNode::~WallpaperImageNode() = default;

void WallpaperImageNode::initialize()
{
    g_data()->mutex.lock();

    QQuickWindow * const window = m_item->window();
    m_micaMaterial = new MicaMaterial(this);

    m_node = new QSGSimpleTextureNode;
    m_node->setFiltering(QSGTexture::Linear);

    g_data()->mutex.unlock();

    maybeGenerateWallpaperImageCache();
    maybeUpdateWallpaperImageClipRect();

    appendChildNode(m_node);

    connect(m_micaMaterial, &MicaMaterial::shouldRedraw, this, [this](){
        maybeGenerateWallpaperImageCache(true);
    });
    connect(window, &QQuickWindow::beforeRendering, this,
        &WallpaperImageNode::maybeUpdateWallpaperImageClipRect, Qt::DirectConnection);

    QuickMicaMaterialPrivate::get(m_item)->appendNode(this);
}

void WallpaperImageNode::maybeGenerateWallpaperImageCache(const bool force)
{
    const QMutexLocker locker(&g_data()->mutex);
    if (!m_pixmapCache.isNull() && !force) {
        return;
    }
    const QSize desktopSize = QGuiApplication::primaryScreen()->virtualSize();
    static constexpr const QPoint originPoint = {0, 0};
    m_pixmapCache = QPixmap(desktopSize);
    m_pixmapCache.fill(kDefaultTransparentColor);
    QPainter painter(&m_pixmapCache);
    m_micaMaterial->paint(&painter, desktopSize, originPoint);
    if (m_texture) {
        delete m_texture;
        m_texture = nullptr;
    }
    m_texture = m_item->window()->createTextureFromImage(m_pixmapCache.toImage());
    m_node->setTexture(m_texture);
}

void WallpaperImageNode::maybeUpdateWallpaperImageClipRect()
{
    const QMutexLocker locker(&g_data()->mutex);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const QSizeF itemSize = m_item->size();
#else
    const QSizeF itemSize = {m_item->width(), m_item->height()};
#endif
    m_node->setRect(QRectF(QPointF(0.0, 0.0), itemSize));
    m_node->setSourceRect(QRectF(m_item->mapToGlobal(QPointF(0.0, 0.0)), itemSize));
}

QuickMicaMaterialPrivate::QuickMicaMaterialPrivate(QuickMicaMaterial *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

QuickMicaMaterialPrivate::~QuickMicaMaterialPrivate() = default;

QuickMicaMaterialPrivate *QuickMicaMaterialPrivate::get(QuickMicaMaterial *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

const QuickMicaMaterialPrivate *QuickMicaMaterialPrivate::get(const QuickMicaMaterial *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

void QuickMicaMaterialPrivate::initialize()
{
    Q_Q(QuickMicaMaterial);
    q->setFlag(QuickMicaMaterial::ItemHasContents);
    q->setSmooth(true);
    q->setAntialiasing(true);
    q->setClip(true);
}

void QuickMicaMaterialPrivate::rebindWindow()
{
    Q_Q(QuickMicaMaterial);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    QQuickItem * const rootItem = window->contentItem();
    q->setParent(rootItem);
    q->setParentItem(rootItem);
#ifndef FRAMELESSHELPER_QUICK_NO_PRIVATE
    QQuickItemPrivate::get(q)->anchors()->setFill(rootItem);
#endif // FRAMELESSHELPER_QUICK_NO_PRIVATE
    q->setZ(-999); // Make sure we always stays on the bottom most place.
    if (m_rootWindowXChangedConnection) {
        disconnect(m_rootWindowXChangedConnection);
        m_rootWindowXChangedConnection = {};
    }
    if (m_rootWindowYChangedConnection) {
        disconnect(m_rootWindowYChangedConnection);
        m_rootWindowYChangedConnection = {};
    }
    m_rootWindowXChangedConnection = connect(window, &QQuickWindow::xChanged, q, [q](){ q->update(); });
    m_rootWindowYChangedConnection = connect(window, &QQuickWindow::yChanged, q, [q](){ q->update(); });
}

void QuickMicaMaterialPrivate::forceRegenerateWallpaperImageCache()
{
    if (m_nodes.isEmpty()) {
        return;
    }
    for (auto &&node : std::as_const(m_nodes)) {
        if (node) {
            node->maybeGenerateWallpaperImageCache(true);
        }
    }
}

void QuickMicaMaterialPrivate::appendNode(WallpaperImageNode *node)
{
    Q_ASSERT(node);
    if (!node) {
        return;
    }
    if (m_nodes.contains(node)) {
        return;
    }
    m_nodes.append(node);
}

QuickMicaMaterial::QuickMicaMaterial(QQuickItem *parent)
    : QQuickItem(parent), d_ptr(new QuickMicaMaterialPrivate(this))
{
}

QuickMicaMaterial::~QuickMicaMaterial() = default;

void QuickMicaMaterial::itemChange(const ItemChange change, const ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);
    Q_D(QuickMicaMaterial);
    switch (change) {
    case ItemDevicePixelRatioHasChanged: {
        d->forceRegenerateWallpaperImageCache();
        update(); // Force re-paint immediately.
    } break;
    case ItemSceneChange: {
        if (value.window) {
            d->rebindWindow();
        }
    } break;
    default:
        break;
    }
}

QSGNode *QuickMicaMaterial::updatePaintNode(QSGNode *old, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    auto node = static_cast<WallpaperImageNode *>(old);
    if (!node) {
        node = new WallpaperImageNode(this);
    }
    return node;
}

void QuickMicaMaterial::classBegin()
{
    QQuickItem::classBegin();
}

void QuickMicaMaterial::componentComplete()
{
    QQuickItem::componentComplete();
}

FRAMELESSHELPER_END_NAMESPACE

#include "quickmicamaterial.moc"
