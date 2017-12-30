/**
 * \file uu_knowledgebox.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2013-2017 The InyokaEdit developers
 *
 * This file is part of InyokaEdit.
 *
 * InyokaEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * InyokaEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition uu.de knowledge box dialog
 */

#ifndef INYOKAEDIT_UU_KNOWLEDGEBOX_H_
#define INYOKAEDIT_UU_KNOWLEDGEBOX_H_

#include <QDialog>
#include <QDir>
#include <QtPlugin>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QString>

#include "../../application/texteditor.h"
#include "../../application/ieditorplugin.h"

namespace Ui {
  class Uu_KnowledgeBoxClass;
}
class QDir;
class QTextDocument;

/**
 * \class Uu_KnowledgeBox
 * \brief Dialog for table insertion
 */
class Uu_KnowledgeBox : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.uuknowledgebox")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir userDataDir, const QString sSharePath);
  QString getPluginName() const;
  QString getPluginVersion() const;
  QTranslator* getPluginTranslator(const QString &sSharePath,
                                   const QString &sLocale);
  QString getCaption() const;
  QIcon getIcon() const;
  bool includeMenu() const;
  bool includeToolbar() const;
  bool hasSettings() const;
  void setCurrentEditor(TextEditor *pEditor);
  void setEditorlist(QList<TextEditor *> listEditors);

 public slots:
  void callPlugin();
  void executePlugin();
  void showSettings();
  void showAbout();

 private slots:
  void accept();
  void addRow();
  void deleteRow(QWidget *widget);

 private:
  void loadTemplateDefaults();
  void loadTemplateEntries();
  void buildUi(QWidget *pParent);
  void writeSettings();
  void createRow(const bool &bActive, const QString &sText);

  Ui::Uu_KnowledgeBoxClass *m_pUi;
  QDialog *m_pDialog;
  QSettings *m_pSettings;
  QString m_sExt;
  TextEditor *m_pEditor;
  QList<bool> m_bListEntryActive;
  QStringList m_sListEntries;
  QSignalMapper *m_pSigMapDeleteRow;
  QList<QPushButton *> m_listDelRowButtons;
  bool m_bCalledSettings;
};

#endif  // INYOKAEDIT_UU_KNOWLEDGEBOX_H_