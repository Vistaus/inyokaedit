/**
 * \file CTableTemplate.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Class definition table insert dialog
 */

#ifndef INYOKAEDIT_CTABLETEMPLATE_H_
#define INYOKAEDIT_CTABLETEMPLATE_H_

#include <QDir>
#include <QtGui/QDialog>
#include <QString>

#include "./CParser.h"
#include "./CTextEditor.h"


namespace Ui {
    class CTableTemplateClass;
}

class CParser;
class CSettings;
class CTemplates;
class CTextEditor;

class QDir;
class QTextDocument;

/**
 * \class CTableTemplate
 * \brief Dialog for table insertion
 */
class CTableTemplate : public QDialog {
    Q_OBJECT

  public:
    /**
     * \brief Constructor
     * \param pParent Pointer to parent windiw
     * \param pEditor Pointer to editor module
     */
    CTableTemplate(CTextEditor *pEditor,
                   const QDir &tmpFileOutputDir,
                   const QDir &tmpImgDir,
                   CSettings *pSettings,
                   CTemplates *pTemplates,
                   QWidget *pParent = 0);

  public slots:
    /** \brief Open dialog */
    void newTable();

  private slots:
    /** \brief Show preview */
    void preview();

    /** \brief Dialog finished */
    void accept();

  private:
    /** \brief Generate specific table */
    void generateTable();

    Ui::CTableTemplateClass *m_pUi;

    CTextEditor *m_pEditor;
    QDir m_dirPreview;
    CParser *m_pParser;

    QString m_sTableString;
    QTextDocument *m_pTextDocument;

    CTemplates *m_pTemplates;

    QStringList m_sListTableStyles;
    QStringList m_sListTableStylesPrefix;
};

#endif  // INYOKAEDIT_CTABLETEMPLATE_H_
