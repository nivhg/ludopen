/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "suggest.h"

Suggest::Suggest(QComboBox *parent): QObject(parent), editor(parent)
{
    this->NbOfRows2Display=NbOfRows2Display;
    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(2);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)),SLOT(doneCompletion()));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(700);
    connect(timer, SIGNAL(timeout()), SLOT(autoSuggest()));
    editor->setEditable(true);
    connect(editor, SIGNAL(currentTextChanged(QString)), timer, SLOT(start()));
    connect(parent, SIGNAL(SignalMousePressEvent()), SLOT(autoSuggest()));
}

Suggest::~Suggest()
{
    delete popup;
}

void Suggest::MAJResults(QVector<QVector<QString> > *Results, int NbOfRows2Display)
{
    this->Results=*Results;
    this->NbOfRows2Display=NbOfRows2Display;
}

bool Suggest::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        editor->setFocus();
        popup->hide();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {

        bool consumed = false;
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
        case Qt::Key_Escape:
            editor->setFocus();
            popup->hide();
            consumed = true;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->setFocus();
            editor->event(ev);
            popup->hide();
            break;
        }

        return consumed;
    }

    return false;
}

void Suggest::showCompletion(const QVector<QVector<QString> > &Vector)
{

    if (Vector.isEmpty())
        return;

    const QPalette &pal = editor->palette();
    QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

    popup->setUpdatesEnabled(false);
    popup->clear();
    for (int i = 0; i < Vector.count(); ++i) {
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem(popup);
        for(int j=0;j<Vector[i].count();j++)
        {
            item->setText(j, Vector[i][j]);
        }
    }
    popup->setCurrentItem(popup->topLevelItem(0));
    popup->setColumnWidth(0,300);
    popup->resizeColumnToContents(1);
    popup->setUpdatesEnabled(true);

    int h = popup->sizeHintForRow(0) * qMin(7, Vector.count()) + 3;
    popup->resize(450, h);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->setFocus();
    popup->show();
}

void Suggest::doneCompletion(QString value)
{
    timer->stop();
    popup->hide();
    editor->setFocus();
    if(value != "")
    {
        editor->setCurrentText(value);
        popup->clear();
    }
    else
    {
        QTreeWidgetItem *item = popup->currentItem();
        if (item) {
            editor->setCurrentText(item->text(SearchResults[0].length()-1));
            popup->clear();
        }
    }
    this->preventSuggest();
    QMetaObject::invokeMethod(editor, "SignalSuggestionFini");
}

void Suggest::autoSuggest()
{
    QString str = editor->currentText();
    unsigned int i (0) ;

    //Remize à  zéro de du Vecteur SearchResults
    this->SearchResults.clear();

    //On vérifie que la suite de lettres entrées dans LE_Nom correpondent aux Noms du vecteur VecteurMembre
    //ou que la suite de chiffre entrées dans LE_Code correspondent aux CodeMembres du Vecteur Results
    bool EstCeNumerique;
    str.toInt(&EstCeNumerique);
    // Si le texte saisie est un nombre
    if( EstCeNumerique )
    {
        doneCompletion(str);
    }
    else
    {
        for(i = 0 ; i < this->Results.size() ; i++)
        {
            // Retrait des accents pour pouvoir faire des recherches de membres avec accents
            QString sNom=this->Results[i][0].toLower().replace(QRegExp("[éèë]"), "e");
            sNom.replace(QRegExp("[à]"), "a");
            QString sPrenom=this->Results[i][1].toLower().replace(QRegExp("[éèë]"), "e");
            sPrenom.replace(QRegExp("[à]"), "a");
            //Si le Nom, le prénom ou les 2 correspondent, on l'ajoute dans le vecteur SearchResults
            if( sNom.indexOf( str.toLower() ) != string::npos )
            {
                this->SearchResults.push_back(Results[i]);
            }
            else if( sPrenom.indexOf( str.toLower() ) != string::npos )
            {
                this->SearchResults.push_back(Results[i]);
            }
        }
        // Si le nom ou le prénom sont le seul résultat correct, on complète le champs
        if(this->SearchResults.size()==1&&(this->SearchResults[0][0].toLower()==str.toLower()||this->SearchResults[0][1].toLower()==str.toLower()))
        {
            doneCompletion(this->SearchResults.at(0).at(SearchResults[0].length()-1));
        }
        else
        {
            showCompletion(this->SearchResults);
        }
    }
}

void Suggest::preventSuggest()
{
    timer->stop();
}
