/************************************************************************
 **
 **  Copyright (C) 2019  Kevin B. Hendricks, Stratford Ontario Canada
 **
 **  This file is part of PageEdit.
 **
 **  PageEdit is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  PageEdit is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with PageEdit.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include <QByteArray>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineSettings>
#include <QApplication>

#include "SettingsStore.h"
#include "Inspector.h"


static const QString SETTINGS_GROUP = "inspect_dialog";


Inspector::Inspector(QWidget *parent) :
    QDockWidget(parent),
    m_inspectView(new QWebEngineView(this)),
    m_view(nullptr),
    m_LoadingFinished(false),
    m_LoadOkay(false)
{
    QWidget *basewidget = new QWidget(this);
    QLayout *layout = new QVBoxLayout(basewidget);
    basewidget->setLayout(layout);
    layout->addWidget(m_inspectView);
    layout->setContentsMargins(0, 0, 0, 0);
    basewidget->setObjectName("PrimaryFrame");
    setWidget(basewidget);
    // QtWebEngine WebInspector needs to run javascript in MainWorld
    // so override the app default but just for the inspector
    m_inspectView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    LoadSettings();
    setWindowTitle(tr("Inspect Page or Element"));
    connect(m_inspectView->page(), SIGNAL(loadFinished(bool)), this, SLOT(UpdateFinishedState(bool)));
    connect(m_inspectView->page(), SIGNAL(loadStarted()), this, SLOT(LoadingStarted()));
}

Inspector::~Inspector()
{
    if (m_inspectView) {
        m_inspectView->close();
        m_view = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        m_inspectView->page()->setInspectedPage(nullptr);
#endif
        delete m_inspectView;
        m_inspectView = nullptr;
    }
}

void Inspector::LoadingStarted()
{
    m_LoadingFinished = false;
    m_LoadOkay = false;
}

void Inspector::UpdateFinishedState(bool okay)
{
    m_LoadingFinished = true;
    m_LoadOkay = okay;
}

void Inspector::InspectPageofView(QWebEngineView* view)
{
    m_view = view;

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    if (m_view) {
        m_inspectView->page()->setInspectedPage(m_view->page());
    } 
#else
    if (m_view) {
        QString not_supported = tr("The Inspector functionality is not supported before Qt 5.11");
	QString response = "<html><head><title>Warning</title></head><body><p>" + 
	                   not_supported + "</p></body></html>";
        m_inspectView->setHtml(response);
	show();
    }
#endif
}

void Inspector::StopInspection()
{
    SaveSettings();
    m_view = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    m_inspectView->page()->setInspectedPage(nullptr);
#else
    m_inspectView->setHtml("<html><head><title></title></head><body></body></html>");
#endif
}


QSize Inspector::sizeHint()
{
  return QSize(400,200);
}

void Inspector::closeEvent(QCloseEvent* event)
{
    // qDebug() << "Inspector Close Event";
    StopInspection();
    QDockWidget::closeEvent(event);
}

void Inspector::LoadSettings()
{
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    QByteArray geometry = settings.value("geometry").toByteArray();

    if (!geometry.isNull()) {
        restoreGeometry(geometry);
    }
    settings.endGroup();
}

void Inspector::SaveSettings()
{
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    settings.setValue("geometry", saveGeometry());
    QApplication::restoreOverrideCursor();
    settings.endGroup();
}

