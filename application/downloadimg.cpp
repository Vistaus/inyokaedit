/**
 * \file downloadimg.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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
 * Download manager for images.
 */

#include "./downloadimg.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

DownloadImg::DownloadImg() {
  connect(&m_NwManager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(downloadFinished(QNetworkReply*)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::setDLs(const QStringList &sListUrls,
                         const QStringList &sListSavePath) {
  m_sListUrls = sListUrls;
  m_sListSavePath = sListSavePath;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::startDownloads() {
  m_nProgress = 0;
  m_sDownloadError.clear();

  // Create progress dialog
  m_pProgessDialog = new QProgressDialog(trUtf8("Downloading images..."),
                                         trUtf8("Cancel"), m_nProgress,
                                         m_sListUrls.size(), 0,
                                         Qt::WindowTitleHint
                                         | Qt::WindowSystemMenuHint);
  m_pProgessDialog->setWindowTitle(qApp->applicationName());
  m_pProgessDialog->setMinimumDuration(100);
  m_pProgessDialog->setModal(true);
  m_pProgessDialog->setMaximumSize(m_pProgessDialog->size());
  m_pProgessDialog->setMinimumSize(m_pProgessDialog->size());
  connect(m_pProgessDialog, SIGNAL(canceled()),
          this, SLOT(cancelDownloads()));

  if (m_sListUrls.size() == m_sListSavePath.size()
      && m_sListUrls.size() > 0) {
    for (int i = 0; i < m_sListUrls.size(); i++) {
      QUrl url = QUrl::fromEncoded(m_sListUrls[i].toLocal8Bit());
      this->doDownload(url, m_sListSavePath[i]);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::doDownload(const QUrl &url,
                             const QString &sSavePath,
                             const QString sBase) {
  QNetworkRequest request(url);
  QNetworkReply *reply = m_NwManager.get(request);

  m_listDownloadReplies.append(reply);
  m_sListRepliesPath.append(sSavePath);
  m_sListBasename.append(sBase);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::downloadFinished(QNetworkReply *pReply) {
  QIODevice *pData(pReply);
  int nIndex = m_listDownloadReplies.indexOf(pReply);
  m_pProgessDialog->setValue(m_nProgress);

  // Check for redirection
  QVariant possibleRedirectUrl = pReply->attribute(
                                   QNetworkRequest::RedirectionTargetAttribute);
  m_urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),
                                        m_urlRedirectedTo);

  // Error
  if (QNetworkReply::NoError != pReply->error()) {
    if (QNetworkReply::OperationCanceledError != pReply->error()) {
      m_sDownloadError += pData->errorString() + "\n\n";
    }
    qWarning() << "Image download error (#" << pReply->error() <<
                  "): " + pData->errorString();

    m_pProgessDialog->setValue(m_nProgress);
    m_nProgress++;
  } else {
    // No error
    QUrl url = pReply->url();
    qDebug() << "Downloading URL: " << url.toString();

    // Basename has to be set before possible redirection
    // (redirected file could have other basename)
    if (m_sListBasename[nIndex].isEmpty()) {
      m_sListBasename[nIndex] = url.toString().mid(
                                  url.toString().lastIndexOf("/") + 1);
    }

    // If the URL is not empty, we're being redirected
    if (!m_urlRedirectedTo.isEmpty()) {
      url = m_urlRedirectedTo;
      qDebug() << "Redirected to: " + url.toString();

      // New request with redirected url
      this->doDownload(url, m_sListRepliesPath[nIndex],
                       m_sListBasename[nIndex]);
    } else {
      m_urlRedirectedTo.clear();
      m_pProgessDialog->setValue(m_nProgress);
      m_nProgress++;

      // Save file
      QFile file(m_sListRepliesPath[nIndex]
                 + "/" + m_sListBasename[nIndex]);
      if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open " + file.fileName()
                      + " for writing: " + file.errorString();
      } else {
        file.write(pData->readAll());
        file.close();

        qDebug() << "Download of " + m_sListBasename[nIndex]
                    + " succeeded - saved to "
                    +  m_sListRepliesPath[nIndex];
      }
    }
  }

  m_listDownloadReplies.removeAll(pReply);
  m_sListRepliesPath.removeAt(nIndex);
  m_sListBasename.removeAt(nIndex);
  pReply->deleteLater();

  // All downloads finished
  if (m_listDownloadReplies.isEmpty()) {
    m_pProgessDialog->setValue(m_pProgessDialog->maximum());
    m_pProgessDialog->deleteLater();

    m_sListRepliesPath.clear();
    m_sListBasename.clear();
    qDebug() << "All downloads finished...";

    // Show error messages
    if (!m_sDownloadError.isEmpty()) {
      QMessageBox::warning(0, trUtf8("Download error"), m_sDownloadError);
    }

    emit finsihedImageDownload();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QUrl DownloadImg::redirectUrl(const QUrl &possibleRedirectUrl,
                              const QUrl &oldRedirectUrl) const {
  QUrl redirectUrl;
  if (!possibleRedirectUrl.isEmpty()
      && possibleRedirectUrl != oldRedirectUrl) {
    redirectUrl = possibleRedirectUrl;
  }
  return redirectUrl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::cancelDownloads() {
  foreach (QNetworkReply *pReply, m_listDownloadReplies) {
    pReply->abort();
  }

  m_listDownloadReplies.clear();
  m_sListRepliesPath.clear();
  m_sListBasename.clear();

  qDebug() << "Canceled downloads...";
  emit finsihedImageDownload();
}
