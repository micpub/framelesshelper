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

#pragma once

#include "framelesshelperwidgets_global.h"
#include <QtWidgets/qdialog.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(lcFramelessDialog)

class FramelessDialogPrivate;

class FRAMELESSHELPER_WIDGETS_API FramelessDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FramelessDialog)
    Q_DISABLE_COPY_MOVE(FramelessDialog)

public:
    explicit FramelessDialog(QWidget *parent = nullptr);
    ~FramelessDialog() override;

private:
    std::unique_ptr<FramelessDialogPrivate> d_ptr = nullptr;
};

FRAMELESSHELPER_END_NAMESPACE

Q_DECLARE_METATYPE2(FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessDialog))
