/**
 * \file spellchecker.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2018 The InyokaEdit developers
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
 * Spell checker using Hunspell
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#include "./spellchecker.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextStream>
#include <QStringList>

SpellChecker::~SpellChecker() {
}

void SpellChecker::initPlugin(QWidget *pParent, TextEditor *pEditor,
                              const QDir userDataDir,
                              const QString sSharePath) {
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;
  Q_UNUSED(pParent);

#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pHunspell = NULL;
  m_pEditor = pEditor;
  m_UserDataDir = userDataDir;
  m_sSharePath = sSharePath;

  m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_sDictPath = m_pSettings->value("DictionaryPath", "").toString();
  this->setDictPath();
  m_sDictLang = m_pSettings->value("SpellCheckerLanguage",
                                   "de_DE").toString();
  m_sCommunity = m_pSettings->value("InyokaCommunity",
                                    "ubuntuusers_de").toString();
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString SpellChecker::getPluginName() const {
  return PLUGIN_NAME;
}

QString SpellChecker::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);
  if ("en" == sLang) {
    return;
  }

  if (!m_translator.load(":/" + QString(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QString(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
    if (!m_translator.load(QString(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" + QString(PLUGIN_NAME).toLower() +
                    "_" + sLang + ".qm";
      return;
    }
  }

  if (!qApp->installTranslator(&m_translator)) {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString SpellChecker::getCaption() const {
  return trUtf8("Spell checker");
}
QIcon SpellChecker::getIcon() const {
  return QIcon::fromTheme("tools-check-spelling",
                          QIcon(":/spellchecker.png"));
}

bool SpellChecker::includeMenu() const {
  return true;
}
bool SpellChecker::includeToolbar() const {
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::setDictPath() {
  m_sListDicts.clear();

  if (m_sDictPath.isEmpty() || !QDir(m_sDictPath).exists()) {
    // Standard path for Hunspell
    if (QDir("/usr/share/hunspell").exists()) {
      m_sDictPath = "/usr/share/hunspell/";
    } else if (QDir("/usr/local/share/hunspell").exists()) {
      m_sDictPath = "/usr/local/share/hunspell/";
      // Otherwise look for MySpell dictionary
    } else if (QDir("/usr/share/myspell/dicts").exists()) {
      m_sDictPath = "/usr/share/myspell/dicts/";
    } else if (QDir("/usr/local/share/myspell/dicts").exists()) {
      m_sDictPath = "/usr/local/share/myspell/dicts/";
    } else {
      // Fallback and for Windows look in app dir
      m_sDictPath = qApp->applicationDirPath() + "/dicts/";
    }
  }
  if (!m_sDictPath.endsWith('/')) {
    m_sDictPath.append('/');
  }
  // Group already set in costructor
  // m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_pSettings->setValue("DictionaryPath", m_sDictPath);
  // m_pSettings->endGroup();

  QFileInfoList fiListFiles = QDir(m_sDictPath).entryInfoList(
                                QDir::NoDotAndDotDot | QDir::Files);
  foreach (QFileInfo fi, fiListFiles) {
    m_sListDicts << fi.baseName();
  }
  m_sListDicts.removeDuplicates();
  // qDebug() << "Dictionary files:" << m_sListDicts;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SpellChecker::initDictionaries() {
  if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
      || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
    qWarning() << "Spell checker dictionary file does not exist:"
               << m_sDictPath + m_sDictLang << "*.dic *.aff";
    QMessageBox::warning(0, qApp->applicationName(),
                         "Spell checker dictionary file does not exist!\n"
                         "Trying to load fallback dictionary.");

    // Try to load english fallback
    m_sDictLang = "en_GB";
    if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
        || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
      qWarning() << "Spell checker fallback does not exist:"
                 << m_sDictPath + m_sDictLang << "*.dic *.aff";
      m_sDictLang = "en-GB";
      if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
          || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
        qWarning() << "Spell checker fallback does not exist:"
                   << m_sDictPath + m_sDictLang << "*.dic *.aff";
        QMessageBox::warning(0, qApp->applicationName(),
                             "Spell checker fallback "
                             + m_sDictLang + " doesn't exist as well.");
        return false;
      }
    }
  }

  // Init user dictionary
  m_sUserDict = m_UserDataDir.absolutePath() + "/userDict_"
                + m_sDictLang + ".txt";
  if (!QFile::exists(m_sUserDict)) {
    QFile userDictFile(m_sUserDict);
    if (userDictFile.open(QIODevice::WriteOnly)) {
      userDictFile.close();
    } else {
      QMessageBox::warning(0, qApp->applicationName(),
                           "User dictionary file couldn't be opened.");
      qWarning() << "User dictionary file could not be opened/created:"
                 << m_sUserDict;
    }
  }

  QString sDictFile(m_sDictPath + m_sDictLang + ".dic");
  QString sAffixFile(m_sDictPath + m_sDictLang + ".aff");
  QByteArray dictFilePathBA = sDictFile.toLocal8Bit();
  QByteArray affixFilePathBA = sAffixFile.toLocal8Bit();

  // qDebug() << "Using dictionary:" << sDictFile;

  // Detect encoding analyzing the SET option in the affix file
  m_sEncoding = "ISO8859-1";
  QFile _affixFile(sAffixFile);
  if (_affixFile.open(QIODevice::ReadOnly)) {
    QTextStream stream(&_affixFile);
    QRegExp enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*",
                         Qt::CaseInsensitive);
    for (QString sLine = stream.readLine();
         !sLine.isEmpty();
         sLine = stream.readLine()) {
      if (enc_detector.indexIn(sLine) > -1) {
        m_sEncoding = enc_detector.cap(1);
        // qDebug() << QString("Encoding set to ") + _encoding;
        break;
      }
    }
    _affixFile.close();
  } else {
    QMessageBox::warning(0, qApp->applicationName(),
                         "Dictionary could not be opened.");
    qWarning() << "Dictionary could not be opened:" << sAffixFile;
    return false;
  }
  m_pCodec = QTextCodec::codecForName(
               this->m_sEncoding.toLatin1().constData());

  if (m_pHunspell != NULL) {
    delete m_pHunspell;
  }
  m_pHunspell = NULL;
  m_pHunspell = new Hunspell(affixFilePathBA.constData(),
                             dictFilePathBA.constData());

  this->loadAdditionalDict(m_sUserDict);
  this->loadAdditionalDict(m_sSharePath + "/community/" +
                           m_sCommunity + "/ExtendedDict.txt");
  this->loadAdditionalDict(qApp->applicationDirPath() + "/ExtendedDict.txt");
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::loadAdditionalDict(const QString &sFilename) {
  QFile DictonaryFile(sFilename);
  if (DictonaryFile.exists()) {
    if (DictonaryFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&DictonaryFile);
      for (QString sWord = stream.readLine();
           !sWord.isEmpty();
           sWord = stream.readLine()) {
        putWord(sWord);
      }
      DictonaryFile.close();
    } else {
      qWarning() << "Dictionary" << sFilename << "could not be opened.";
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::callPlugin() {
  if (!this->initDictionaries()) {
    return;
  }

  m_pCheckDialog = new SpellCheckDialog(this, 0);

  QTextCharFormat highlightFormat;
  highlightFormat.setBackground(QBrush(QColor("#ff6060")));
  highlightFormat.setForeground(QBrush(QColor("#000000")));
  // Alternative format
  // highlightFormat.setUnderlineColor(QColor("red"));
  // highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

  // Save the position of the current cursor
  m_oldCursor = m_pEditor->textCursor();

  // Create a new cursor to walk through the text
  QTextCursor cursor(m_pEditor->document());

  SpellCheckDialog::SpellCheckAction spellResult = SpellCheckDialog::None;

  // Don't call cursor.beginEditBlock(), as this prevents the redraw
  // after changes to the content
  // cursor.beginEditBlock();
  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString sWord(cursor.selectedText());

    // Workaround for better recognition of words punctuation etc.
    // does not belong to words
    while (!sWord.isEmpty()
           && !sWord.at(0).isLetter()
           && cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      sWord = cursor.selectedText();
    }

    if (!sWord.isEmpty() && !this->spell(sWord)) {
      QTextCursor tmpCursor(cursor);
      tmpCursor.setPosition(cursor.anchor());
      m_pEditor->setTextCursor(tmpCursor);
      m_pEditor->ensureCursorVisible();

      // Highlight the unknown word
      QTextEdit::ExtraSelection es;
      es.cursor = cursor;
      es.format = highlightFormat;

      QList<QTextEdit::ExtraSelection> esList;
      esList << es;
      m_pEditor->setExtraSelections(esList);
      QCoreApplication::processEvents();

      // Ask user what to do
      spellResult = m_pCheckDialog->checkWord(sWord);

      // Reset the word highlight
      esList.clear();
      m_pEditor->setExtraSelections(esList);
      QCoreApplication::processEvents();

      if (spellResult == SpellCheckDialog::AbortCheck) {
        break;
      }

      switch (spellResult) {
        case SpellCheckDialog::ReplaceOnce:
          cursor.insertText(m_pCheckDialog->replacement());
          break;

        default:
          break;
      }
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }

  if (m_pCheckDialog != NULL) {
    delete m_pCheckDialog;
  }
  m_pCheckDialog = NULL;

  // cursor.endEditBlock();
  m_pEditor->setTextCursor(m_oldCursor);

  if (spellResult != SpellCheckDialog::AbortCheck) {
    QMessageBox::information(0, qApp->applicationName(),
                             trUtf8("Spell check has finished."));
  }
}

void SpellChecker::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SpellChecker::spell(const QString &sWord) {
  // Encode from Unicode to the encoding used by current dictionary
  return m_pHunspell->spell(m_pCodec->fromUnicode(sWord).constData()) != 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QStringList SpellChecker::suggest(const QString &sWord) {
  char **suggestWordList;

  // Encode from Unicode to the encoding used by current dictionary
  int nSuggestions = m_pHunspell->suggest(&suggestWordList,
                                          m_pCodec->fromUnicode(sWord)
                                          .constData());
  QStringList sListSuggestions;
  for (int i = 0; i < nSuggestions; ++i) {
    sListSuggestions << m_pCodec->toUnicode(suggestWordList[i]);
    free(suggestWordList[i]);
  }
  return sListSuggestions;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::ignoreWord(const QString &sWord) {
  this->putWord(sWord);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::putWord(const QString &sWord) {
  m_pHunspell->add(m_pCodec->fromUnicode(sWord).constData());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::addToUserWordlist(const QString &sWord) {
  this->putWord(sWord);
  if (!m_sUserDict.isEmpty()) {
    QFile userDictonaryFile(m_sUserDict);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << sWord << "\n";
      userDictonaryFile.close();
    } else {
      QMessageBox::warning(0, "Spell checker",
                           "User dictionary " + m_sUserDict
                           + " could not be opened for appending a "
                             "new word.");
      qWarning() << "User dictionary" << m_sUserDict
                 << "could not be opened for appending a new word.";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SpellChecker::hasSettings() const {
  return false;
}

void SpellChecker::showSettings() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void SpellChecker::setEditorlist(QList<TextEditor *> listEditors) {
  Q_UNUSED(listEditors);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::showAbout() {
  QMessageBox aboutbox(NULL);
  aboutbox.setWindowTitle(trUtf8("Info"));
  aboutbox.setIconPixmap(QPixmap(":/spellchecker.png"));
  aboutbox.setText(QString("<p><b>%1</b><br />"
                           "%2</p>"
                           "<p>%3<br />"
                           "%4</p>"
                           "<p><i>%5</i></p>")
                   .arg(this->getCaption())
                   .arg(trUtf8("Version") + ": " + PLUGIN_VERSION)
                   .arg(PLUGIN_COPY)
                   .arg(trUtf8("Licence") + ": " +
                        "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>")
                   .arg(trUtf8("Spell checker based on "
                               "<a href=\"http://hunspell.sourceforge.net/\">"
                               "Hunspell</a>.")));
  aboutbox.exec();
}
