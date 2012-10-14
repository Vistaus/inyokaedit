/**
 * \file CTextEditor.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
 *
 * This file is part of the examples of the Qt Toolkit.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 *
 * \section DESCRIPTION
 * Class definition for extended QTextEdit (editor widget).
 */

#ifndef INYOKAEDIT_CTEXTEDITOR_H_
#define INYOKAEDIT_CTEXTEDITOR_H_

#include <QTextEdit>
#include <QTimer>

#include "ui_CInyokaEdit.h"

class QCompleter;

/**
 * \class CTextEditor
 * \brief Extended QTextEdit (editor widget) with simple code completition.
 */
class CTextEditor : public QTextEdit {
    Q_OBJECT

  public:
    CTextEditor(Ui::CInyokaEdit *pGUI,
                bool bCompleter,
                QStringList sListTplMacros,
                quint16 nAutosave,
                QString sUserAppDir,
                QWidget *pParent = 0);
    ~CTextEditor();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

  protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

  private slots:
    void insertCompletion(const QString &sCompletion);
    void saveArticleAuto();

  private:
    QTimer *m_pTimerAutosave;
    QString m_UserAppDir;
    QString textUnderCursor() const;
    QCompleter *m_pCompleter;
    bool m_bCodeCompState;
    QStringList m_sListCompleter;
};

#endif  // INYOKAEDIT_CTEXTEDITOR_H_
