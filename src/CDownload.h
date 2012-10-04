/**
 * \file CDownload.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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
 * Class definition for download functions.
 */

#ifndef CDOWNLOAD_H
#define CDOWNLOAD_H

#include <QNetworkReply>

#include "CInyokaEdit.h"
#include "CDownloadImg.h"

class CInyokaEdit;

/**
 * \class CDownload
 * \brief Download functions (Inyoka styles, articles, article images)
 */
class CDownload : public QObject
{
    Q_OBJECT

public:
    /**
    * \brief Constructor
    * \param pParent Pointer to parent window
    * \param sAppName Application name
    * \param sAppDir Application directory
    * \param StylesDir Folder in which Inyoka style elements should be stored
    */
    CDownload( QWidget *pParent, const QString &sAppName, const QString &sAppDir, const QString &sStylesDir,
               const QString &sImgDir, const QString &sInyokaUrl, const bool bAutomaticImgDL );

    /**
    * \brief Start download of an existing article
    * \param ImgDir Folder in which attached images should be downloaded
    * \param sInyokaUrl Full url to the Inyoka wiki
    * \param bAutomaticImageDownload True or false if attached article images should be downloaded without any question
    * \return True or false for successful / not successful download
    */
    void downloadArticle();

public slots:
    /**
    * \brief Start download of Inyoka style elements
    * \return True or false for successful / not successful download
    */
    bool loadInyokaStyles();

    void showArticle();

private slots:
    void replyFinished( QNetworkReply *reply );

signals:
    void sendArticleText( const QString &, const QString & );  /**< Signal for sending downloaded article text to editor */

private:
    /**
    * \brief Start download of images which are attached to an article
    * \param sArticlename Name of the article
    * \param ImgDir Folder in which attached images should be downloaded
    * \param sInyokaUrl Full url to the Inyoka wiki
    * \param AutomaticImageDownload True or false if attached article images should be downloaded without any question
    * \return True or false for successful / not successful download
    */
    void downloadImages();

    QWidget *m_pParent;    /**< Pointer to parent window */
    QString m_sAppName;    /**< Application name */
    QString m_sAppDir;     /**< Application directory */
    QString m_sStylesDir;  /**< Folder in which Inyoka style elements should be stored */
    QString m_sImgDir;

    QNetworkAccessManager *m_NwManager;
    QNetworkReply *m_Reply;
    QString m_sArticleText;
    QString m_sSitename;
    QString m_sSource;
    QString m_sInyokaUrl;

    CDownloadImg *m_DlImages;
    bool m_bAutomaticImageDownload;
    bool m_bDownloadArticle;
};

#endif // CDOWNLOAD_H
