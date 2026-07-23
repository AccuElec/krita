// SPDX-FileCopyrightText: 2026 KritaFluent Contributors
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KIS_FLUENT_UI_EXPORT_H
#define KIS_FLUENT_UI_EXPORT_H

#include <QtGlobal>

#ifdef KRITA_FLUENT_UI_BUILDING
#  define KRITA_FLUENT_UI_EXPORT Q_DECL_EXPORT
#else
#  define KRITA_FLUENT_UI_EXPORT Q_DECL_IMPORT
#endif

#endif // KIS_FLUENT_UI_EXPORT_H
