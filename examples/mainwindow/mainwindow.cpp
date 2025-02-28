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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qfileiconprovider.h>
#include <Utils>
#include <StandardTitleBar>
#include <StandardSystemButton>
#include <FramelessWidgetsHelper>
#include "../shared/settings.h"
#include "../widget/widget.h"
#include "../dialog/dialog.h"

extern template void Settings::set<QRect>(const QString &, const QString &, const QRect &);
extern template void Settings::set<qreal>(const QString &, const QString &, const qreal &);
extern template void Settings::set<QByteArray>(const QString &, const QString &, const QByteArray &);

extern template QRect Settings::get<QRect>(const QString &, const QString &);
extern template qreal Settings::get<qreal>(const QString &, const QString &);
extern template QByteArray Settings::get<QByteArray>(const QString &, const QString &);

FRAMELESSHELPER_USE_NAMESPACE

using namespace Global;

FRAMELESSHELPER_STRING_CONSTANT(Geometry)
FRAMELESSHELPER_STRING_CONSTANT(State)
FRAMELESSHELPER_STRING_CONSTANT(DevicePixelRatio)

MainWindow::MainWindow(QWidget *parent, const Qt::WindowFlags flags) : FramelessMainWindow(parent, flags)
{
    initialize();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!parent()) {
        Settings::set({}, kGeometry, geometry());
        Settings::set({}, kState, saveState());
        Settings::set({}, kDevicePixelRatio, devicePixelRatioF());
    }
    FramelessMainWindow::closeEvent(event);
}

void MainWindow::initialize()
{
    m_titleBar = new StandardTitleBar(this);
    m_titleBar->setTitleLabelAlignment(Qt::AlignCenter);
    m_mainWindow = new Ui::MainWindow;
    m_mainWindow->setupUi(this);

    QMenuBar * const mb = menuBar();
    mb->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mb->setStyleSheet(FRAMELESSHELPER_STRING_LITERAL(R"(
QMenuBar {
    background-color: transparent;
}

QMenuBar::item {
    background: transparent;
}

QMenuBar::item:selected {
    background: #a8a8a8;
}

QMenuBar::item:pressed {
    background: #888888;
}
    )"));
    const auto titleBarLayout = static_cast<QHBoxLayout *>(m_titleBar->layout());
    titleBarLayout->insertWidget(0, mb);

    // setMenuWidget(): make the menu widget become the first row of the window.
    setMenuWidget(m_titleBar);

    FramelessWidgetsHelper *helper = FramelessWidgetsHelper::get(this);
    helper->setTitleBarWidget(m_titleBar);
    helper->setSystemButton(m_titleBar->minimizeButton(), SystemButtonType::Minimize);
    helper->setSystemButton(m_titleBar->maximizeButton(), SystemButtonType::Maximize);
    helper->setSystemButton(m_titleBar->closeButton(), SystemButtonType::Close);
    helper->setHitTestVisible(mb); // IMPORTANT!
    connect(helper, &FramelessWidgetsHelper::ready, this, [this, helper](){
        const auto savedGeometry = Settings::get<QRect>({}, kGeometry);
        if (savedGeometry.isValid() && !parent()) {
            const auto savedDpr = Settings::get<qreal>({}, kDevicePixelRatio);
            // Qt doesn't support dpi < 1.
            const qreal oldDpr = std::max(savedDpr, qreal(1));
            const qreal scale = (devicePixelRatioF() / oldDpr);
            setGeometry({savedGeometry.topLeft() * scale, savedGeometry.size() * scale});
        } else {
            helper->moveWindowToDesktopCenter();
        }
        const QByteArray savedState = Settings::get<QByteArray>({}, kState);
        if (!savedState.isEmpty() && !parent()) {
            restoreState(savedState);
        }
    });

    setWindowTitle(tr("FramelessHelper demo application - Qt MainWindow"));
    setWindowIcon(QFileIconProvider().icon(QFileIconProvider::Computer));
    connect(m_mainWindow->pushButton, &QPushButton::clicked, this, [this]{
        const auto dialog = new Dialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->exec();
    });
    connect(m_mainWindow->pushButton_2, &QPushButton::clicked, this, [this]{
        const auto widget = new Widget(this);
        widget->setAttribute(Qt::WA_DeleteOnClose);
        widget->show();
    });
}
