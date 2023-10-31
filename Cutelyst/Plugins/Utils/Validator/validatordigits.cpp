﻿/*
 * SPDX-FileCopyrightText: (C) 2017-2023 Matthias Fehring <mf@huessenbergnetz.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "validatordigits_p.h"

using namespace Cutelyst;

ValidatorDigits::ValidatorDigits(const QString &field,
                                 const QVariant &length,
                                 const Cutelyst::ValidatorMessages &messages,
                                 const QString &defValKey)
    : ValidatorRule(*new ValidatorDigitsPrivate(field, length, messages, defValKey))
{
}

ValidatorDigits::~ValidatorDigits() = default;

ValidatorReturnType ValidatorDigits::validate(Context *c, const ParamsMultiMap &params) const
{
    ValidatorReturnType result;

    Q_D(const ValidatorDigits);

    const QString v   = value(params);
    bool ok           = false;
    const int _length = d->extractInt(c, params, d->length, &ok);
    if (!ok) {
        qCDebug(C_VALIDATOR).noquote()
                << debugString(c)
                << "Invalid comparison length";
        result.errorMessage = validationDataError(c);
        return result;
    }

    if (!v.isEmpty()) {

        if (Q_LIKELY(ValidatorDigits::validate(v, _length))) {
            if ((_length > 0) && (v.length() != _length)) {
                result.errorMessage = validationError(c, _length);
                qCDebug(C_VALIDATOR).noquote()
                        << debugString(c)
                        << "Does not contain exactly" << _length << "digits:" << v.length() << "!=" << _length;
            } else {
                result.value.setValue(v);
            }
        } else {
            result.errorMessage = validationError(c, _length);
            qCDebug(C_VALIDATOR).noquote().nospace()
                    << debugString(c)
                    << " Does not only contain digits: \"" << v << "\"";
        }

    } else {
        defaultValue(c, &result);
    }

    return result;
}

bool ValidatorDigits::validate(const QString &value, int length)
{
    bool valid = true;

    for (const QChar &ch : value) {
        const ushort &uc = ch.unicode();
        if (!((uc >= ValidatorRulePrivate::ascii_0) && (uc <= ValidatorRulePrivate::ascii_9))) {
            valid = false;
            break;
        }
    }

    if (valid && (length > 0) && (length != value.length())) {
        valid = false;
    }

    return valid;
}

QString ValidatorDigits::genericValidationError(Context *c, const QVariant &errorData) const
{
    QString error;

    const QString _label = label(c);
    const int _length    = errorData.toInt();

    if (_label.isEmpty()) {
        if (_length > 0) {
            error = c->translate(
                "Cutelyst::ValidatorDigits", "Must contain exactly %n digit(s).", "", _length);
        } else {
            error = c->translate("Cutelyst::ValidatorDigits", "Must only contain digits.");
        }
    } else {
        if (_length > 0) {
            //: %1 will be replaced by the field label
            error = c->translate("Cutelyst::ValidatorDigits",
                                 "The “%1” field must contain exactly %n digit(s).",
                                 "",
                                 _length)
                        .arg(_label);
        } else {
            //: %1 will be replaced by the field label
            error = c->translate("Cutelyst::ValidatorDigits",
                                 "The “%1” field must only contain digits.")
                        .arg(_label);
        }
    }

    return error;
}
