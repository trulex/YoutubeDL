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

#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <QMainWindow>
#include <QProcess>
#include <QNetworkReply>

namespace Ui {
class YoutubeDL;
}

class YoutubeDL : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit YoutubeDL(QWidget *parent = 0);
    ~YoutubeDL();
    
private slots:
    void on_actionExit_triggered();

    void on_browseButton_clicked();

    void on_url_textChanged(const QString &arg1);

    void on_downloadButton_clicked();

    void printOutput();

    void printError();

    void getInfo();

    void on_cancelButton_clicked();

    void on_pauseButton_clicked();

    void on_downloadOptions_clicked();

    void getFormats();

    void slot_netwManagerFinished(QNetworkReply *reply);

    void on_audioCheckBox_stateChanged(int arg1);

    void on_actionAbout_triggered();

    void on_actionSupported_sites_triggered();

    void getSites();

    void changeEvent(QEvent *event);

private:
    Ui::YoutubeDL *ui;
    QProcess *download;
    QProcess *info;
};

#endif // YOUTUBEDL_H
