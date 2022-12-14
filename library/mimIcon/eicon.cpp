/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "eicon.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QSettings>
#include <QCache>
#include <QtCore/QCoreApplication>
#include <QApplication>
//#include "qiconfix/qiconloader_p.h"
//#include "eiconengin.h"
QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(EIcon, EIconInstance)
void EIcon::seTemeName(const QString  &name)
{
    QIcon::setThemeName(name);
}

EIcon *EIcon::instance()
{
    return EIconInstance();
}

QString themePath(const QString &path)
{
    foreach (QString p, QIcon::themeSearchPaths())
    {
        QDir dirS;
       // qDebug()<<"themePath" << p+"/"+path;
        if(dirS.exists(p+"/"+path)){
          //  qDebug()<<"return" << p+"/"+path;
            return  p+"/"+path;
        }

    }
    return path;
}
//______________________________________________________________________________________
QIcon EIcon::fromTheme(const QString  &iconName, const QString &fallback)
{


    QIcon iconF;


    ///------file Name--if exists
    if(QFile::exists(iconName) && !iconName.isEmpty()){

        QString sufix=QFileInfo(iconName).suffix();
        if(sufix.toLower()=="png"
                ||sufix.toLower()=="svg"
                ||sufix.toLower()=="xpm"
                ||sufix.toLower()=="svgz"){
            iconF=QIcon(iconName);
            if(!iconF.isNull())
                return iconF;
        }
    }


    if(iconName.isEmpty())
        return QIcon::fromTheme("unknown");

    if(instance()->cachIcon.contains(iconName))
        return instance()->cachIcon.value(iconName);

    else if(QIcon::hasThemeIcon(iconName))
        iconF =  QIcon::fromTheme(iconName);


    else if(hasPathIcon(iconName,THEM_PIXMAP))
        iconF =iconFromThemePath(iconName,THEM_PIXMAP);


    else if(hasPathIcon(iconName,THEM_BACK))
        iconF =  iconFromThemePath(iconName,themePath(THEM_BACK));


    if(iconF.availableSizes().isEmpty()){
        if(!fallback.isEmpty() && QIcon::hasThemeIcon(fallback))
            iconF =  QIcon::fromTheme(fallback);
    }

    if(!iconF.availableSizes().isEmpty()){
        instance()->cachIcon.insert(iconF.name(),iconF);
        return iconF;
    }

    iconF=QIcon(":/icons/"+iconName);
    if(!iconF.pixmap(128).isNull())  return iconF;

    iconF=QIcon(":/icons/"+fallback);
    if(!iconF.pixmap(128).isNull())   return iconF;

    return QIcon::fromTheme(APP_EXE);

}
//-------------------------------------------------------------------
bool EIcon::hasPathIcon(const QString &name,const QString &path)
{

    QString mPath=path;

    foreach (QString p, QIcon::themeSearchPaths())
    {
        QDir dirS(p);
        if(dirS.exists(p+"/"+path))
        {
            mPath=  p+"/"+path;
            // qDebug()<<"hasPathIcon"<<name<< mPath;

            break;
        }

    }
    //qDebug()<<"hasPathIcon return"<<name<< mPath;
    return searchIcon(name,mPath);

}

bool EIcon::searchIcon(const QString &name,const QString &path)
{
    QStringList files = QDir(path).entryList(QStringList(name+".*"),
                                             QDir::Files | QDir::NoDotAndDotDot);

    if(files.count()>0){
        // qDebug()<<"searchIcon"<<name<< path<<files;
        return true;
    }

    QDirIterator it( path,QDir::AllDirs |QDir::NoDotAndDotDot ,
                     QDirIterator::Subdirectories);

    while(it.hasNext())
    {
        // qDebug()<<"it.filePath()"<<it.filePath();
        it.next();
        if(hasPathIcon(name,it.filePath()))
            return true;
    }

    return false;
}
//______________________________________________________________________________________
QIcon addIconFile(QIcon m_icon,const QString m_iconName,const QString &curPath)
{
    QStringList listStr;
    listStr<<m_iconName+".png" <<m_iconName+".xpm"
          <<m_iconName+".svg"<<m_iconName+".svgz";

    QStringList files = QDir(curPath).entryList(QStringList(listStr),
                                                QDir::Files | QDir::NoDotAndDotDot);

    foreach (QString s, files) {
        QString f=  QDir(curPath).absoluteFilePath(s);
        QSize size=QPixmap(f).size();
        if(!size.isNull())
            m_icon.addFile(f,size);

    }
    return m_icon;
}

QIcon EIcon::iconFromThemePath(const QString &m_iconName,const QString &path)
{
    // qDebug()<<"iconFromThemePath"<<m_iconName<<path;
    QIcon m_icon ;

    m_icon=  addIconFile(m_icon,m_iconName,path);


    QDirIterator it(path,QDir::AllDirs |
                    QDir::NoDotAndDotDot , QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        it.next();
        m_icon=  addIconFile(m_icon,m_iconName,it.filePath());

    }

    return m_icon;
}

QT_END_NAMESPACE
