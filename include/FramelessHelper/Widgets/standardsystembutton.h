/*
 * MIT License
 *
 * Copyright (C) 2022 by wangwenx190 (Yuhang Zhao)
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

#pragma once

#include "framelesshelperwidgets_global.h"
#include <QtWidgets/qabstractbutton.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

class StandardSystemButtonPrivate;

class FRAMELESSHELPER_WIDGETS_API StandardSystemButton : public QAbstractButton
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(StandardSystemButton)
    Q_DISABLE_COPY_MOVE(StandardSystemButton)
    Q_PROPERTY(Global::SystemButtonType buttonType READ buttonType WRITE setButtonType NOTIFY buttonTypeChanged FINAL)
    Q_PROPERTY(bool hover READ isHover WRITE setHover NOTIFY hoverChanged FINAL)
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor NOTIFY hoverColorChanged FINAL)
    Q_PROPERTY(QColor pressColor READ pressColor WRITE setPressColor NOTIFY pressColorChanged FINAL)

public:
    explicit StandardSystemButton(QWidget *parent = nullptr);
    explicit StandardSystemButton(const Global::SystemButtonType type, QWidget *parent = nullptr);
    ~StandardSystemButton() override;

    Q_NODISCARD QSize sizeHint() const override;

    void setIcon(const QIcon &icon);

    Q_NODISCARD Global::SystemButtonType buttonType();
    void setButtonType(const Global::SystemButtonType value);

    Q_NODISCARD bool isHover() const;
    void setHover(const bool value);

    Q_NODISCARD QColor hoverColor() const;
    void setHoverColor(const QColor &value);

    Q_NODISCARD QColor pressColor() const;
    void setPressColor(const QColor &value);

protected:
    void enterEvent(QT_ENTER_EVENT_TYPE *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void buttonTypeChanged();
    void hoverChanged();
    void hoverColorChanged();
    void pressColorChanged();

private:
    QScopedPointer<StandardSystemButtonPrivate> d_ptr;
};

FRAMELESSHELPER_END_NAMESPACE