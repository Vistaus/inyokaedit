/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CProgessDialog.cpp
* Purpose:    Shows a modal window with download progress messages
***************************************************************************/

#include "CProgressDialog.h"
#include "ui_CProgressDialog.h"

CProgressDialog::CProgressDialog(const QString &sScriptname, const QString &sAppname, QWidget *parent, QString sDownloadFolder) :
    QDialog(parent),
    ui(new Ui::CProgressDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (sDownloadFolder == ""){
        sDownloadFolder = QDir::homePath() + "/." + sAppname;
    }

    try
    {
        myProc = new QProcess();
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myProc - bad_alloc caught: " << ba.what() << std::endl;
      exit (-1);
    }
    myProc->start(sScriptname, QStringList() << sDownloadFolder);

    // Show output
    connect(myProc, SIGNAL(readyReadStandardOutput()),this, SLOT(showMessage()) );
    connect(myProc, SIGNAL(readyReadStandardError()), this, SLOT(showErrorMessage()) );

    connect(myProc, SIGNAL(finished(int)), this, SLOT(DownloadScriptFinished()) );
}

CProgressDialog::~CProgressDialog()
{
    myProc->kill();
    delete myProc;
    myProc = NULL;
    delete ui;
    ui = NULL;
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::closeEvent(QCloseEvent *event)
{
    myProc->kill();   // Kill download process
    event->accept();  // Close window
}

// -----------------------------------------------------------------------------------------------

// Show message
void CProgressDialog::showMessage()
{
    QByteArray strdata = myProc->readAllStandardOutput();
    ui->textEditProcessOut->setTextColor(Qt::black);
    ui->textEditProcessOut->append(strdata);
}

// Show error message
void CProgressDialog::showErrorMessage()
{
    QByteArray strdata = myProc->readAllStandardError();
    ui->textEditProcessOut->setTextColor(Qt::darkGray);
    ui->textEditProcessOut->append(strdata);
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::DownloadScriptFinished()
{
    this->close();
}

// -----------------------------------------------------------------------------------------------

// Click on cancel button
void CProgressDialog::on_pushButtonClosProc_clicked()
{
    this->close();  // Send close event
}
