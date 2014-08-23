#-------------------------------------------------
#This file is part of YoutubeDL.
#
#YoutubeDL is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#YoutubeDL is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with YoutubeDL.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------

#-------------------------------------------------
#
# Project created by QtCreator 2013-08-22T14:34:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YoutubeDL
TEMPLATE = app


SOURCES += main.cpp\
        youtubedl.cpp

HEADERS  += youtubedl.h

FORMS    += youtubedl.ui

RC_ICONS = YoutubeDL.ico
ICON = YoutubeDL.icns

RESOURCES += \
    YoutubeDL.qrc

TRANSLATIONS = youtubedl_sl.ts
