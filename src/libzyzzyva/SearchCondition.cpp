//---------------------------------------------------------------------------
// SearchCondition.cpp
//
// A class to represent a word search condition.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include "SearchCondition.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

const QString XML_TOP_ELEMENT = "condition";
const QString XML_TYPE_ATTR = "type";
const QString XML_STRING_ATTR = "string";
const QString XML_MIN_ATTR = "min";
const QString XML_MAX_ATTR = "max";
const QString XML_NEGATED_ATTR = "negated";
const QString XML_OLD_NUMBER_ATTR = "number";
const QString XML_OLD_PERCENT_ATTR = "percent";

//---------------------------------------------------------------------------
//  asString
//
//! Return a string representing the search spec.
//
//! @return the string representation
//---------------------------------------------------------------------------
QString
SearchCondition::asString() const
{
    if (type == UnknownSearchType)
        return QString::null;

    QString str;
    if (negated)
        str = "NOT ";

    str += Auxil::searchTypeToString (type) + ": ";

    switch (type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        case Prefix:
        case Suffix:
        case IncludeLetters:
        case BelongToGroup:
        str += stringValue;
        break;

        case InWordList:
        str += "...";
        break;

        case Length:
        case NumAnagrams:
        case ProbabilityOrder:
        str += "Min " + QString::number (minValue) + ", Max "
            + QString::number (maxValue);
        break;

        case Probability:
        // XXX: Multiply by the correct factor here!
        str += "Min " + QString::number (minValue * 1) + ", Max "
            + QString::number (maxValue * 1);
        break;

        case ConsistOf:
        str += "Min " + QString::number (minValue * 1) + "%, Max "
            + QString::number (maxValue * 1) + "% " + stringValue;
        break;

        default: break;
    }

    return str;
}

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the search spec.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
SearchCondition::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement (XML_TOP_ELEMENT);

    if (type == UnknownSearchType)
        return topElement;

    topElement.setAttribute (XML_TYPE_ATTR, Auxil::searchTypeToString (type));

    switch (type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        topElement.setAttribute (XML_STRING_ATTR, stringValue);
        break;

        case Prefix:
        case Suffix:
        case IncludeLetters:
        case BelongToGroup:
        case InWordList:
        topElement.setAttribute (XML_STRING_ATTR, stringValue);
        topElement.setAttribute (XML_NEGATED_ATTR, negated);
        break;

        case Length:
        case NumAnagrams:
        case ProbabilityOrder:
        topElement.setAttribute (XML_MIN_ATTR, minValue);
        topElement.setAttribute (XML_MAX_ATTR, maxValue);
        break;

        case Probability:
        // XXX: Multiply by the correct factor here!
        topElement.setAttribute (XML_MIN_ATTR, (minValue * 1));
        topElement.setAttribute (XML_MAX_ATTR, (maxValue * 1));
        break;

        case ConsistOf:
        topElement.setAttribute (XML_MIN_ATTR, minValue);
        topElement.setAttribute (XML_MAX_ATTR, maxValue);
        topElement.setAttribute (XML_STRING_ATTR, stringValue);
        break;

        default: break;
    }

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! search condition.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
SearchCondition::fromDomElement (const QDomElement& element)
{
    if ((element.tagName() != XML_TOP_ELEMENT) ||
        (!element.hasAttribute (XML_TYPE_ATTR)))
    {
        return false;
    }

    SearchCondition tmpCondition;
    tmpCondition.type = Auxil::stringToSearchType (element.attribute
                                                   (XML_TYPE_ATTR));
    if (tmpCondition.type == UnknownSearchType)
        return false;

    bool ok = false;

    switch (tmpCondition.type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        if (!element.hasAttribute (XML_STRING_ATTR))
            return false;
        tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        break;

        case Prefix:
        case Suffix:
        case IncludeLetters:
        case BelongToGroup:
        case InWordList:
        if (!element.hasAttribute (XML_STRING_ATTR))
            return false;
        tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        if (element.hasAttribute (XML_NEGATED_ATTR)) {
            int n = element.attribute (XML_NEGATED_ATTR).toInt (&ok);
            if (ok)
                tmpCondition.negated = n ? true : false;
        }
        break;

        case Length:
        case NumAnagrams:
        case ProbabilityOrder:
        if (!element.hasAttribute (XML_MIN_ATTR) ||
            !element.hasAttribute (XML_MAX_ATTR))
            return false;
        tmpCondition.minValue =
            element.attribute (XML_MIN_ATTR).toInt (&ok);
        if (!ok)
            return false;
        tmpCondition.maxValue =
            element.attribute (XML_MAX_ATTR).toInt (&ok);
        if (!ok)
            return false;
        break;

        case Probability:
        if (!element.hasAttribute (XML_MIN_ATTR) ||
            !element.hasAttribute (XML_MAX_ATTR))
            return false;
        tmpCondition.minValue =
            element.attribute (XML_MIN_ATTR).toInt (&ok);
        if (!ok)
            return false;
        // XXX: Divide by the correct factor here!
        tmpCondition.minValue /= 1;
        tmpCondition.maxValue =
            element.attribute (XML_MAX_ATTR).toInt (&ok);
        if (!ok)
            return false;
        // XXX: Divide by the correct factor here!
        tmpCondition.maxValue /= 1;
        break;

        case ConsistOf: {
            if (!element.hasAttribute (XML_STRING_ATTR))
                return false;

            if (!element.hasAttribute (XML_OLD_PERCENT_ATTR) &&
                (!element.hasAttribute (XML_MIN_ATTR) ||
                !element.hasAttribute (XML_MAX_ATTR)))
            {
                return false;
            }
            int intValue =
                element.attribute (XML_OLD_PERCENT_ATTR).toInt (&ok);
            // Old style Consist condition
            if (ok) {
                tmpCondition.minValue = intValue;
                tmpCondition.maxValue = 100;
            }
            else {
                tmpCondition.minValue =
                    element.attribute (XML_MIN_ATTR).toInt (&ok);
                if (!ok)
                    return false;
                tmpCondition.maxValue =
                    element.attribute (XML_MAX_ATTR).toInt (&ok);
                if (!ok)
                    return false;
            }
            tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        }
        break;

        // Obsolete condition types
        case OldDoesNotTakePrefix:
        case OldDoesNotTakeSuffix:
        case OldMustExclude:
        case OldNotInWordList:
        if (!element.hasAttribute (XML_STRING_ATTR))
            return false;
        tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        if (tmpCondition.type == OldDoesNotTakePrefix) {
            tmpCondition.type = Prefix;
            tmpCondition.negated = true;
        }
        else if (tmpCondition.type == OldDoesNotTakeSuffix) {
            tmpCondition.type = Suffix;
            tmpCondition.negated = true;
        }
        else if (tmpCondition.type == OldMustExclude) {
            tmpCondition.type = IncludeLetters;
            tmpCondition.negated = true;
        }
        else if (tmpCondition.type == OldNotInWordList) {
            tmpCondition.type = InWordList;
            tmpCondition.negated = true;
        }
        break;

        // Obsolete condition types
        case OldExactLength:
        case OldMinLength:
        case OldMaxLength:
        case OldExactAnagrams:
        case OldMinAnagrams:
        case OldMaxAnagrams: {
            if (!element.hasAttribute (XML_OLD_NUMBER_ATTR))
                return false;
            int intValue = element.attribute (XML_OLD_NUMBER_ATTR).toInt (&ok);
            if (!ok)
                return false;
            if (tmpCondition.type == OldExactLength) {
                tmpCondition.type = Length;
                tmpCondition.minValue = intValue;
                tmpCondition.maxValue = intValue;
            }
            else if (tmpCondition.type == OldMinLength) {
                tmpCondition.type = Length;
                tmpCondition.minValue = intValue;
                tmpCondition.maxValue = MAX_WORD_LEN;
            }
            else if (tmpCondition.type == OldMaxLength) {
                tmpCondition.type = Length;
                tmpCondition.minValue = 0;
                tmpCondition.maxValue = intValue;
            }
            if (tmpCondition.type == OldExactAnagrams) {
                tmpCondition.type = NumAnagrams;
                tmpCondition.minValue = intValue;
                tmpCondition.maxValue = intValue;
            }
            else if (tmpCondition.type == OldMinAnagrams) {
                tmpCondition.type = NumAnagrams;
                tmpCondition.minValue = intValue;
                tmpCondition.maxValue = 999999;
            }
            else if (tmpCondition.type == OldMaxAnagrams) {
                tmpCondition.type = NumAnagrams;
                tmpCondition.minValue = 0;
                tmpCondition.maxValue = intValue;
            }
        }
        break;

        default: break;
    }

    *this = tmpCondition;
    return true;
}