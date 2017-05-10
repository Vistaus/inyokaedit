/**
 * \file CXmlParser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Class definition.
 */

#ifndef INYOKAEDIT_CXMLPARSER_H_
#define INYOKAEDIT_CXMLPARSER_H_

#include <QXmlSimpleReader>

/**
 * \class CXmlParser
 * \brief Delivering menu structure from xml file.
 */
class CXmlParser {
  class CHandler;

 public:
  CXmlParser();
  bool parseXml(const QString &sXmlFile);

  QString getMenuName() const;
  QString getPath() const;
  QStringList getGroupNames() const;
  QStringList getGroupIcons() const;
  QList<QStringList> getElementNames() const;
  QList<QStringList> getElementInserts() const;
  QList<QStringList> getElementIcons() const;

 private:
  QXmlInputSource *m_pXmlSource;  /**< Pointer to xml input source file */
  CHandler *m_pHandler;  /**< Handle to xml parser module */

  QString m_sMenuName;
  QString m_sPath;
  QStringList m_sListGroups;
  QStringList m_sListGroupIcons;
  QList<QStringList> m_sListNames;
  QList<QStringList> m_sListInserts;
  QList<QStringList> m_sListIcons;
};

// ----------------------------------------------------------------------------

/**
 * \class CXmlParser::Handler
 * \brief Reading definitions from xml file.
 */
class CXmlParser::CHandler : public QXmlDefaultHandler {
  friend class CXmlParser;

 public:
  /**
    * \brief Called at document start
    * \return True
    */
  bool startDocument();

  /**
    * \brief Found end of a xml element
    * \param name Element name
    * \return True
    */
  bool endElement(const QString&, const QString&, const QString &name);

  /**
    * \brief Found start of a xml element
    * \param name Element name
    * \param attrs Attribute name
    * \return True
    */
  bool startElement(const QString&, const QString&, const QString &name,
                    const QXmlAttributes &attrs);

 private:
  bool m_bInElement;

  QString m_tmpMenuName;
  QString m_tmpPath;
  QStringList m_tmpListNames;
  QStringList m_tmpListInserts;
  QStringList m_tmpListIcons;

  QString m_sMenuName_2;
  QString m_sPath_2;
  QStringList m_sListGroups_2;
  QStringList m_sListGroupIcons_2;
  QList<QStringList> m_sListNames_2;
  QList<QStringList> m_sListInserts_2;
  QList<QStringList> m_sListIcons_2;
};

#endif  // INYOKAEDIT_CXMLPARSER_H_