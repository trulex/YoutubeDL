/*
*This file is part of YoutubeDL.
*
*YoutubeDL is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*YoutubeDL is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with YoutubeDL.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "youtubedl.h"
#include "ui_youtubedl.h"
#include <QDir>
#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QProcess>
#include <QByteArray>
#include <QMessageBox>

YoutubeDL::YoutubeDL(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::YoutubeDL) {
    ui->setupUi(this);
    this->setMenuIcons();

    ui->downloadProgressBar->setValue(0);
    ui->path->setText(QDir::currentPath());
    ui->downloadOptionsFrame->setVisible(false);
    ui->downloadOptions->setDisabled(true);
    QStringList audioFormats;
    audioFormats << "best" << "aac" << "vorbis" << "mp3" << "m4a" << "opus" << "wav";
    ui->audioFormatCombo->addItems(audioFormats);
    for (int var = 0; var < 10; ++var) {
        ui->audioQualityCombo->addItem(QString::number(var));
    }
}

YoutubeDL::~YoutubeDL() {
    delete ui;
}

void YoutubeDL::setMenuIcons() {
    ui->actionExit->setIcon(QIcon::fromTheme(QStringLiteral("application-exit"), QIcon(":images/Actions-application-exit-icon.png")));
    ui->actionPaste->setIcon(QIcon::fromTheme(QStringLiteral("edit-paste"), QIcon(":images/Actions-edit-paste-icon.png")));
    ui->actionClear->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear"), QIcon(":images/Actions-edit-clear-icon.png")));
    ui->actionAbout->setIcon(QIcon::fromTheme(QStringLiteral("help-about"), QIcon(":images/Status-dialog-information-icon.png")));
    ui->actionSupported_sites->setIcon(QIcon(":images/Actions-help-contents-icon.png"));
}

void YoutubeDL::on_actionExit_triggered() {
    qApp->exit();
}

void YoutubeDL::on_browseButton_clicked() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    QString path = QFileDialog::getExistingDirectory();
    if (!path.isNull()) {
        ui->path->setText(path);
    }
}

void YoutubeDL::on_url_textChanged(const QString &arg1) {
    // Check if URL is correct and valid
    if (arg1.isEmpty() || !QUrl(arg1).isValid()) {
        info->kill();
        ui->pasteButton->setEnabled(true);
        ui->downloadButton->setDisabled(true);
        ui->cancelButton->setDisabled(true);
        ui->pauseButton->setDisabled(true);
        ui->downloadOptions->setDisabled(true);
        ui->downloadProgressBar->setValue(0);
        ui->downloadProgressBar->setFormat("0%");
        ui->titleDescLabel->setText("");
        ui->imageLabel->setPixmap(QPixmap());
        ui->downloadOptionsFrame->setHidden(true);
        ui->audioCheckBox->setChecked(false);
        ui->audioFormatCombo->setDisabled(true);
        ui->audioQualityCombo->setDisabled(true);
        ui->keepVideoCheckBox->setDisabled(true);
        ui->keepVideoCheckBox->setChecked(false);
    } else {
        ui->pasteButton->setDisabled(true);
        // Get video info
        QString program = "youtube-dl";
        QStringList arguments;
        arguments <<"--get-thumbnail"<<"--get-title"<<"--get-description" << arg1;
        info = new QProcess(this);
        info->start(program, arguments);
        QObject::connect(info, SIGNAL(finished(int)), this, SLOT(getInfo()));
        QObject::connect(info, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
        ui->titleDescLabel->setText("Fetching video/audio info...");
    }
}

void YoutubeDL::printError() {
    QString errorInfo(info->readAllStandardError());
    if (errorInfo.length() > 0) {
        ui->titleDescLabel->setText("Error occured. Please check the URL.");
        ui->pasteButton->setEnabled(true);
    }
}

void YoutubeDL::getInfo()
{
    QString videoInfo(info->readAllStandardOutput());
    if (videoInfo.length() > 0 && NULL != videoInfo) {
        //QStringList split = videoInfo.split("\n");
        QStringList split = videoInfo.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        QString title = split.at(0);
        QString thumbnail = split.at(1);
        if (thumbnail.contains("https",Qt::CaseInsensitive))
            thumbnail.remove(4,1);
        split.removeAt(0);
        split.removeAt(0);
        QString description;
        foreach(QString item, split) {
            description.append(item);
            description.append("<br>");
        }

        ui->downloadOptions->setEnabled(true);

        QNetworkAccessManager *m_netwManager = new QNetworkAccessManager(this);
        connect(m_netwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_netwManagerFinished(QNetworkReply*)));
        QNetworkRequest request(thumbnail);
        m_netwManager->get(request);
        ui->titleDescLabel->setText("<b>"+title+"</b>"+"<br><br>"+description);
        ui->downloadButton->setEnabled(true);
        ui->pasteButton->setEnabled(true);
    }
}

void YoutubeDL::slot_netwManagerFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    pixmap=pixmap.scaledToWidth(200);
    ui->imageLabel->setPixmap(pixmap);
}


void YoutubeDL::on_downloadButton_clicked()
{
    /**
     * Call youtube-dl with selected arguments
     */
    QString program = "youtube-dl";
    QStringList arguments;
    if (ui->downloadOptionsFrame->isEnabled()) {
        QString rate = ui->downloadRate->text();
        if (!rate.isEmpty()) {
            arguments << "-r" << rate;
        }
        if (ui->audioCheckBox->isChecked()) {
            arguments << "-x" << "--audio-format" << ui->audioFormatCombo->currentText() << "--audio-quality" << ui->audioQualityCombo->currentText();
            if (ui->keepVideoCheckBox->isChecked())
                arguments << "-k";
        }
        arguments << "-o" << ui->path->text()+"/%(title)s.%(ext)s" << ui->url->text();
    } else {
        arguments << "-o" << ui->path->text()+"/%(title)s.%(ext)s" << ui->url->text();
    }
    download = new QProcess(this);
    download->start(program, arguments);
    QObject::connect(download, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    ui->pauseButton->setEnabled(true);
    ui->cancelButton->setEnabled(true);
    ui->downloadProgressBar->setFormat("Download starting...");
    ui->downloadButton->setDisabled(true);
}
void YoutubeDL::printOutput()
{
    QString line(download->readAllStandardOutput());
    qDebug() << line;
    QString progress;
    QString timeRemaining;
    if(line.contains(QString("[download]"), Qt::CaseInsensitive)){
        QRegExp rx("(\\d+\\.\\d+%)");
        timeRemaining = line.right(5);
        rx.indexIn(line);
        if(!rx.cap(0).isEmpty()) {
            progress = rx.cap(0);
            progress.chop(3);
            ui->downloadProgressBar->setValue(progress.toInt());
            ui->downloadProgressBar->setFormat(timeRemaining);
        } else {
            if (!ui->downloadProgressBar->value())
                ui->downloadProgressBar->setFormat("This has already been downloaded.");
        }
        if (line.contains("100.0%")) {
            ui->downloadProgressBar->setFormat("Download finished");
            ui->downloadButton->setDisabled(true);
            ui->cancelButton->setDisabled(true);
            ui->pauseButton->setDisabled(true);
        }
    }
}

void YoutubeDL::on_cancelButton_clicked()
{
    download->kill();
    ui->downloadProgressBar->setValue(0);
    ui->downloadProgressBar->setFormat("0%");
}

void YoutubeDL::on_pauseButton_clicked()
{
    if (ui->pauseButton->text().compare("Pause")==0) {
        download->kill();
        ui->pauseButton->setText("Resume");
    } else {
        on_downloadButton_clicked();
        ui->pauseButton->setText("Pause");
    }
}

void YoutubeDL::on_downloadOptions_clicked()
{
    if (ui->downloadOptionsFrame->isVisible()) {
        ui->downloadOptionsFrame->setVisible(false);
    } else {
        ui->downloadOptions->setDisabled(true);
        ui->downloadOptionsFrame->setVisible(true);
        ui->downloadOptionsFrame->setEnabled(true);
        /*ui->videoFormatCombo->setEnabled(true);
        if (ui->url->text().contains("youtube",Qt::CaseInsensitive) || ui->url->text().contains("youtu.be",Qt::CaseInsensitive)) {
            QString program = "youtube-dl";
            QStringList arguments;
            arguments <<"-F"<< ui->url->text();
            info = new QProcess(this);
            info->start(program, arguments);
            QObject::connect(info, readyReadStandardOutput(), this, SLOT(getFormats()));
        } else {
            ui->videoFormatCombo->setDisabled(true);
        } maybe one day */
        ui->videoFormatCombo->setDisabled(true);
        ui->audioFormatCombo->setDisabled(true);
        ui->audioQualityCombo->setDisabled(true);
        ui->keepVideoCheckBox->setDisabled(true);
    }
}
/**
 * @brief YoutubeDL::getFormats
 * For selecting video format when downloading from Youtube.
 * Not finished yet.
 */
void YoutubeDL::getFormats()
{
    QString videoInfo(info->readAllStandardOutput());

    qDebug() << videoInfo;
    QStringList formats = videoInfo.split("\n");
    qDebug() << formats;
    ui->videoFormatCombo->addItems(formats);
}

void YoutubeDL::on_audioCheckBox_stateChanged(int arg1)
{
    if (!arg1) {
        ui->audioFormatCombo->setDisabled(true);
        ui->audioQualityCombo->setDisabled(true);
        ui->keepVideoCheckBox->setDisabled(true);
    } else {
        ui->audioFormatCombo->setEnabled(true);
        ui->audioQualityCombo->setEnabled(true);
        ui->keepVideoCheckBox->setEnabled(true);
    }
}

void YoutubeDL::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About YouTubeDL"),
    tr("This is a GUI for the program youtube-dl.<br/><br/>Author: Darko Janković"));
}

void YoutubeDL::on_actionSupported_sites_triggered()
{
    QString program = "youtube-dl";
    QStringList arguments;
    arguments <<"--list-extractors";
    info = new QProcess(this);
    info->start(program, arguments);
    QObject::connect(info, SIGNAL(readyReadStandardOutput()), this, SLOT(getSites()));
}

void YoutubeDL::getSites()
{
    QString videoInfo(info->readAllStandardOutput());
    videoInfo.replace("\n",", ");
    videoInfo.remove(videoInfo.lastIndexOf(","),1);
    QMessageBox::about(this, tr("Supported sites"), videoInfo);
}

void YoutubeDL::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
    }
}
