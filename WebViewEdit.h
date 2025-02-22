/************************************************************************
**
**  Copyright (C) 2019 Kevin B. Hendricks Stratford, Ontario, Canada 
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

#ifndef WEBVIEWEDIT_H
#define WEBVIEWEDIT_H

#include <memory>
#include <QEvent>
#include <QtWebEngineWidgets/QWebEngineView>
#include "WebPageEdit.h"
#include "Viewer.h"

class QSize;

class WebViewEdit : public QWebEngineView, public Viewer
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent The object's parent.
     */
    WebViewEdit(QWidget *parent = 0);
    ~WebViewEdit();

    QSize sizeHint() const;

    void CustomSetDocument(const QString &path, const QString &html);

    bool IsLoadingFinished();

    bool WasLoadOkay() { return m_LoadOkay; }

    void SetZoomFactor(float factor);

    void SetCurrentZoomFactor(float factor);

    float GetZoomFactor() const;

    void Zoom();

    void UpdateDisplay();

    /**
     * Scrolls the editor to the top.
     */
    void ScrollToTop();

    /**
     * Scrolls the editor to the specified fragment when the document is loaded.
     *
     * @param fragment The fragment ID to scroll to.
     *                 It should have the "#" character as the first character.
     */
    void ScrollToFragment(const QString &fragment);

    /**
     *  Workaround for a crappy setFocus implementation in QtWebKit.
     */
    void GrabFocus();

    // inherited
    QList<ElementIndex> GetCaretLocation();

    // methods for working with and converting QWebPaths to ElementIndex Lists 
    QList<ElementIndex> ConvertQWebPathToHierarchy(const QString & webpath) const;
    QString ConvertHierarchyToQWebPath(const QList<ElementIndex>& hierarchy);

    // inherited
    void StoreCaretLocationUpdate(const QList<ElementIndex> &hierarchy);

    // inherited
    bool ExecuteCaretUpdate();

    /**
     * Force a caret location update to the specified position.
     */
    bool ExecuteCaretUpdate(const QString &caret_location);

    QString GetCaretLocationUpdate();

    void StoreCurrentCaretLocation();

    void FormatBlock(const QString &element_name, bool preserve_attributes);

    QString GetCaretElementName();

public slots:
     void PasteText(const QString &text);
     QString GetHtml() const;
     QString GetSelectedText();
     bool InsertHtml(const QString &html);
     bool ExecCommand(const QString &command);
     bool ExecCommand(const QString &command, const QString &parameter);
     bool QueryCommandState(const QString &command);


signals:
    /**
     * Emitted whenever the zoom factor changes.
     *
     * @param new_zoom_factor The new zoom factor of the View.
     */
    void ZoomFactorChanged(float new_zoom_factor);

    void LinkClicked(const QUrl &url);

    void ShowStatusMessageRequest(const QString &message);

    void DocumentLoaded();

    void GoToPreviewLocationRequest();

protected:

    QString EscapeJSString(const QString &string);

    /**
     * Evaluates the provided javascript source code
     * and returns the result.
     *
     * @param javascript The JavaScript source code to execute.
     * @return The result from the last executed javascript statement.
     */
    QVariant EvaluateJavascript(const QString &javascript);

    /** 
     * run a javascript asynchronously with no need for return value
     */
    void DoJavascript(const QString &javascript);

protected slots:

    void UpdateFinishedState(bool okay);
    void LoadingStarted();

protected:

private slots:

    /**
     * Loads the required JavaScript on web page loads.
     */
    void WebPageJavascriptOnLoad();

    void executeCaretUpdateInternal() {
        ExecuteCaretUpdate();
    }

private:

    /**
     * Actually performs the scrolling, will only be invoked after the document has loaded.
     */
    void ScrollToFragmentInternal(const QString &fragment);

    /**
     * Builds the element-selecting JavaScript code, ignoring the text nodes.
     * Always just chains children() jQuery calls.
     *
     * @return The element-selecting JavaScript code.
     */
    QString GetElementSelectingJS_NoTextNodes(const QList<ElementIndex> &hierarchy) const;

    /**
     * Builds the element-selecting JavaScript code, ignoring all the
     * text nodes except the last one.
     * Chains children() jQuery calls, and then the contents() function
     * for the last element (the text node, naturally).
     *
     * @return The element-selecting JavaScript code.
     */
    QString GetElementSelectingJS_WithTextNode(const QList<ElementIndex> &hierarchy) const;

    /**
     * Connects all the required signals to their respective slots.
     */
    void ConnectSignalsToSlots();

    ///////////////////////////////
    // PRIVATE MEMBER VARIABLES
    ///////////////////////////////

    WebPageEdit *m_ViewWebPage;

    float m_CurrentZoomFactor;

    /**
     * The javascript source code of the jQuery library.
     */
    const QString c_jQuery;

    /**
     * The javascript source code of the jQuery
     * ScrollTo extension library.
     */
    const QString c_jQueryScrollTo;

    /**
     * The JavaScript source code used
     * to get a hierarchy of elements from
     * the caret element to the top of the document.
     */
    const QString c_GetCaretLocation;

    /**
     * The JavaScript source code routines used
     * to format a block element
     */
    const QString c_GetBlock;
    const QString c_FormatBlock;


    /**
     * Stores the JavaScript source code for the
     * caret location update.
     */
    QString m_CaretLocationUpdate;

    int m_pendingLoadCount;
    QString m_pendingScrollToFragment;

    bool m_isLoadFinished;
    bool m_LoadOkay;

};

#endif // WEBVIEWEDIT
