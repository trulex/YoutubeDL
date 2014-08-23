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
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/YoutubeDL.png"));

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);
qDebug() << "NAME" << QLocale::system().name();
    QTranslator youtubedlTranslator;
    youtubedlTranslator.load("youtubedl_" + QLocale::system().name());
    a.installTranslator(&youtubedlTranslator);

    YoutubeDL w;
    w.show();
    
    return a.exec();
}
