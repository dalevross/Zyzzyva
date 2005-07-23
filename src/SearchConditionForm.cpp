//---------------------------------------------------------------------------
// SearchConditionForm.cpp
//
// A form for specifying a search condition.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "SearchConditionForm.h"
#include "SearchSet.h"
#include "WordLineEdit.h"
#include "WordListDialog.h"
#include "WordValidator.h"
#include "Auxil.h"
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

using namespace Defs;

//---------------------------------------------------------------------------
//  SearchConditionForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchConditionForm::SearchConditionForm (QWidget* parent, const char* name,
                                          WFlags f)
    : QFrame (parent, name, f),
    letterValidator (new WordValidator (this, "letterValidator")),
    patternValidator (new WordValidator (this, "patternValidator"))

{
    patternValidator->setOptions (WordValidator::AllowQuestionMarks |
                                  WordValidator::AllowAsterisks |
                                  WordValidator::AllowCharacterClasses);

    QHBoxLayout* mainHlay = new QHBoxLayout (this, 0, SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QStringList types;
    types << Auxil::searchTypeToString (SearchCondition::PatternMatch)
          << Auxil::searchTypeToString (SearchCondition::AnagramMatch)
          << Auxil::searchTypeToString (SearchCondition::SubanagramMatch)
          << Auxil::searchTypeToString (SearchCondition::ExactLength)
          << Auxil::searchTypeToString (SearchCondition::MinLength)
          << Auxil::searchTypeToString (SearchCondition::MaxLength)
          << Auxil::searchTypeToString (SearchCondition::MustInclude)
          << Auxil::searchTypeToString (SearchCondition::MustExclude)
          << Auxil::searchTypeToString (SearchCondition::MustConsist)
          << Auxil::searchTypeToString (SearchCondition::MustBelong)
          << Auxil::searchTypeToString (SearchCondition::InWordList)
          << Auxil::searchTypeToString (SearchCondition::ExactAnagrams)
          << Auxil::searchTypeToString (SearchCondition::MinAnagrams)
          << Auxil::searchTypeToString (SearchCondition::MaxAnagrams);
          //<< Auxil::searchTypeToString (SearchCondition::MinProbability)
          //<< Auxil::searchTypeToString (SearchCondition::MaxProbability);

    typeCbox = new QComboBox (this, "typeCbox");
    Q_CHECK_PTR (typeCbox);
    typeCbox->insertStringList (types);
    connect (typeCbox, SIGNAL (activated (const QString&)),
             SLOT (typeChanged (const QString&)));
    mainHlay->addWidget (typeCbox);

    paramStack = new QWidgetStack (this, "paramStack");
    Q_CHECK_PTR (paramStack);
    mainHlay->addWidget (paramStack);

    // Frame containing just an input line
    paramLineFrame = new QFrame (paramStack, "paramLineFrame");
    Q_CHECK_PTR (paramLineFrame);
    QHBoxLayout* paramLineHlay = new QHBoxLayout (paramLineFrame, 0, SPACING,
                                                  "paramLineHlay");
    Q_CHECK_PTR (paramLineHlay);
    paramLine = new WordLineEdit (paramLineFrame, "paramLine");
    Q_CHECK_PTR (paramLine);
    paramLine->setValidator (patternValidator);
    connect (paramLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
    connect (paramLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramLineHlay->addWidget (paramLine);
    paramStack->addWidget (paramLineFrame);

    // Frame containing just a spin box
    paramSboxFrame = new QFrame (paramStack, "paramSboxFrame");
    Q_CHECK_PTR (paramSboxFrame);
    QHBoxLayout* paramSboxHlay = new QHBoxLayout (paramSboxFrame, 0, SPACING,
                                                  "paramSboxHlay");
    Q_CHECK_PTR (paramSboxHlay);
    paramSbox = new QSpinBox (paramSboxFrame, "paramSbox");
    Q_CHECK_PTR (paramSbox);
    paramSbox->setMinValue (0);
    connect (paramSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramSboxHlay->addWidget (paramSbox);
    paramStack->addWidget (paramSboxFrame);

    // Frame containing just a combo box
    paramCboxFrame = new QFrame (paramStack, "paramCboxFrame");
    Q_CHECK_PTR (paramCboxFrame);
    QHBoxLayout* paramCboxHlay = new QHBoxLayout (paramCboxFrame, 0, SPACING,
                                                  "paramCboxHlay");
    Q_CHECK_PTR (paramCboxHlay);
    paramCbox = new QComboBox (paramCboxFrame, "paramCbox");
    Q_CHECK_PTR (paramCbox);
    connect (paramCbox, SIGNAL (activated (const QString&)),
             SIGNAL (contentsChanged()));
    paramCboxHlay->addWidget (paramCbox);
    paramStack->addWidget (paramCboxFrame);

    // Frame containing spin box and input line
    paramConsistFrame = new QFrame (paramStack, "paramConsistFrame");
    Q_CHECK_PTR (paramConsistFrame);
    QHBoxLayout* paramConsistHlay = new QHBoxLayout (paramConsistFrame, 0,
                                                     SPACING,
                                                     "paramConsistHlay");
    Q_CHECK_PTR (paramConsistHlay);
    paramConsistSbox = new QSpinBox (paramConsistFrame, "paramConsist");
    Q_CHECK_PTR (paramConsistSbox);
    paramConsistSbox->setMinValue (0);
    paramConsistSbox->setMaxValue (100);
    connect (paramConsistSbox, SIGNAL (valueChanged (int)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistSbox);
    QLabel* pctLabel = new QLabel ("%", paramConsistFrame, "pctLabel");
    Q_CHECK_PTR (pctLabel);
    paramConsistHlay->addWidget (pctLabel);
    paramConsistLine = new WordLineEdit (paramConsistFrame,
                                         "paramConsistLine");
    Q_CHECK_PTR (paramConsistLine);
    paramConsistLine->setValidator (letterValidator);
    connect (paramConsistLine, SIGNAL (returnPressed()),
             SIGNAL (returnPressed()));
    connect (paramConsistLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramConsistHlay->addWidget (paramConsistLine);
    paramStack->addWidget (paramConsistFrame);

    // Frame containing disabled input line and push button for getting word
    // lists
    paramWordListFrame = new QFrame (paramStack, "paramWordListFrame");
    Q_CHECK_PTR (paramWordListFrame);
    QHBoxLayout* paramWordListHlay = new QHBoxLayout (paramWordListFrame, 0,
                                                      SPACING,
                                                      "paramWordListHlay");
    Q_CHECK_PTR (paramWordListHlay);

    paramWordListLine = new QLineEdit (paramWordListFrame,
                                       "paramWordListLine");
    Q_CHECK_PTR (paramWordListLine);
    paramWordListLine->setText ("0 words");
    paramWordListLine->setReadOnly (true);
    connect (paramWordListLine, SIGNAL (textChanged (const QString&)),
             SIGNAL (contentsChanged()));
    paramWordListHlay->addWidget (paramWordListLine);

    QPushButton* paramWordListButton = new QPushButton ("Edit List...",
                                                        paramWordListFrame,
                                                        "paramWordListButton");
    Q_CHECK_PTR (paramWordListButton);
    connect (paramWordListButton, SIGNAL (clicked()),
             SLOT (editListClicked()));
    paramWordListHlay->addWidget (paramWordListButton);
    paramStack->addWidget (paramWordListFrame);
}

//---------------------------------------------------------------------------
//  getSearchCondition
//
//! Get a SearchCondition object corresponding to the values in the form.
//
//! @return the search condition
//---------------------------------------------------------------------------
SearchCondition
SearchConditionForm::getSearchCondition() const
{
    SearchCondition condition;
    QString typeStr = typeCbox->currentText();
    condition.type = Auxil::stringToSearchType (typeStr);

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        condition.stringValue = paramLine->text();
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        case SearchCondition::MinProbability:
        case SearchCondition::MaxProbability:
        condition.intValue = paramSbox->value();
        break;

        case SearchCondition::MustConsist:
        condition.stringValue = paramConsistLine->text();
        condition.intValue = paramConsistSbox->value();
        break;

        case SearchCondition::MustBelong:
        condition.stringValue = paramCbox->currentText();
        break;

        case SearchCondition::InWordList:
        condition.stringValue = paramWordListString;
        break;

        default:
        qWarning ("Unrecognized search condition: " + typeStr);
        break;
    }

    return condition;
}

//---------------------------------------------------------------------------
//  setSearchCondition
//
//! Set the contents of the form according to the contents of a search
//! condition.
//
//! @param condition the search condition
//---------------------------------------------------------------------------
void
SearchConditionForm::setSearchCondition (const SearchCondition& condition)
{
    reset();
    typeCbox->setCurrentText (Auxil::searchTypeToString (condition.type));
    typeChanged (typeCbox->currentText());

    switch (condition.type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        paramLine->setText (condition.stringValue);
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        paramSbox->setValue (condition.intValue);
        break;

        case SearchCondition::MustConsist:
        paramConsistSbox->setValue (condition.intValue);
        paramConsistLine->setText (condition.stringValue);
        break;

        case SearchCondition::MustBelong:
        paramCbox->setCurrentText (condition.stringValue);
        break;

        case SearchCondition::InWordList:
        setWordListString (condition.stringValue);
        break;

        default: break;
    }
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the input in the form is valid.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchConditionForm::isValid() const
{
    SearchCondition::SearchType type = Auxil::stringToSearchType
        (typeCbox->currentText());

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        return matchStringIsValid (paramLine->text());

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        return paramSbox->value() > 0;

        case SearchCondition::MustConsist:
        return (paramConsistSbox->value() > 0) &&
               !paramConsistLine->text().isEmpty();

        case SearchCondition::InWordList:
        return !paramWordListString.isEmpty();

        case SearchCondition::UnknownSearchType:
        return false;

        default: return true;
    }
}

//---------------------------------------------------------------------------
//  typeChanged
//
//! Called when an option in the type dropdown box is activated.
//
//! @param string the activated option
//---------------------------------------------------------------------------
void
SearchConditionForm::typeChanged (const QString& string)
{
    SearchCondition::SearchType type = Auxil::stringToSearchType (string);

    switch (type) {
        case SearchCondition::PatternMatch:
        case SearchCondition::AnagramMatch:
        case SearchCondition::SubanagramMatch:
        case SearchCondition::MustInclude:
        case SearchCondition::MustExclude:
        paramStack->raiseWidget (paramLineFrame);
        paramLine->setValidator (((type == SearchCondition::MustExclude) ||
                                  (type == SearchCondition::MustInclude)) ?
                                 letterValidator : patternValidator);
        break;

        case SearchCondition::ExactLength:
        case SearchCondition::MinLength:
        case SearchCondition::MaxLength:
        paramSbox->setMaxValue (MAX_WORD_LEN);
        paramStack->raiseWidget (paramSboxFrame);
        break;

        case SearchCondition::ExactAnagrams:
        case SearchCondition::MinAnagrams:
        case SearchCondition::MaxAnagrams:
        paramSbox->setMaxValue (100);
        paramStack->raiseWidget (paramSboxFrame);
        break;

        case SearchCondition::MinProbability:
        case SearchCondition::MaxProbability:
        paramSbox->setMaxValue (100);
        paramStack->raiseWidget (paramSboxFrame);
        break;

        case SearchCondition::MustBelong:
        paramCbox->clear();
        paramCbox->insertItem (Auxil::searchSetToString (SetHookWords));
        paramCbox->insertItem (Auxil::searchSetToString (SetFrontHooks));
        paramCbox->insertItem (Auxil::searchSetToString (SetBackHooks));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeOneSevens));
        //paramCbox->insertItem (Auxil::searchSetToString (SetTypeTwoSevens));
        //paramCbox->insertItem (Auxil::searchSetToString (SetTypeThreeSevens));
        paramCbox->insertItem (Auxil::searchSetToString (SetTypeOneEights));
        //paramCbox->insertItem (Auxil::searchSetToString
        //                       (SetEightsFromSevenLetterStems));
        paramStack->raiseWidget (paramCboxFrame);
        break;

        case SearchCondition::MustConsist:
        paramStack->raiseWidget (paramConsistFrame);
        break;

        case SearchCondition::InWordList:
        paramStack->raiseWidget (paramWordListFrame);
        break;

        default:
        qWarning ("Unrecognized search condition: " + string);
        break;
    }

    contentsChanged();
}

//---------------------------------------------------------------------------
//  editListClicked
//
//! Called when the Edit List button is clicked.  Display a dialog that allows
//! the user to enter words or add words from a file.
//---------------------------------------------------------------------------
void
SearchConditionForm::editListClicked()
{
    WordListDialog* dialog = new WordListDialog (this, "dialog", true);
    Q_CHECK_PTR (dialog);

    dialog->setWords (paramWordListString);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        setWordListString (dialog->getWords());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  reset
//
//! Reset the form to its original, pristine state.
//---------------------------------------------------------------------------
void
SearchConditionForm::reset()
{
    paramLine->setText ("");
    paramSbox->setValue (0);
    paramCbox->setCurrentItem (0);
    paramConsistSbox->setValue (0);
    paramConsistLine->setText ("");
    typeCbox->setCurrentItem (0);
    typeChanged (typeCbox->currentText());
}

//---------------------------------------------------------------------------
//  setWordListString
//
//! Set the value of the word list string, and update the word list line to
//! reflect the number of words in the list.
//---------------------------------------------------------------------------
void
SearchConditionForm::setWordListString (const QString& string)
{
    paramWordListString = string;
    QStringList wordList = QStringList::split (" ", string);
    paramWordListLine->setText (QString::number (wordList.size()) + " words");
    paramWordListLine->home (false);
}

//---------------------------------------------------------------------------
//  matchStringIsValid
//
//! Determine whether a string is a valid match string.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
SearchConditionForm::matchStringIsValid (const QString& string) const
{
    if (string.isEmpty())
        return false;

    bool inGroup = false;
    bool groupLetters = false;
    bool negatedGroup = false;
    int len = string.length();
    for (int i = 0; i < len; ++i) {
        QChar c = string.at (i);

        if (inGroup) {
            if (c == ']') {
                if (!groupLetters)
                    return false;
                inGroup = false;
                groupLetters = false;
            }
            else if (c == '^') {
                if (groupLetters)
                    return false;
                negatedGroup = true;
            }
            else if ((c == '[') || (c == '*') || (c == '?'))
                return false;
            else
                groupLetters = true;
        }

        else {
            if (c == '[')
                inGroup = true;
            else if ((c == ']') || (c == '^'))
                return false;
        }
    }

    return !inGroup;
}
