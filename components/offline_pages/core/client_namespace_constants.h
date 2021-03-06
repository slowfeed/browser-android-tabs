// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_OFFLINE_PAGES_CORE_CLIENT_NAMESPACE_CONSTANTS_H_
#define COMPONENTS_OFFLINE_PAGES_CORE_CLIENT_NAMESPACE_CONSTANTS_H_

#include "build/build_config.h"

namespace offline_pages {

// Currently used for fallbacks like tests.
extern const char kDefaultNamespace[];

// Any changes to these well-known namespaces should also be reflected in
// OfflinePagesNamespaceEnumeration (histograms.xml) for consistency.
// New namespaces should be put at the end of this list and a corresponding
// enum value should be added in OfflinePagesNamespaceEnumeration.
extern const char kBookmarkNamespace[];
extern const char kLastNNamespace[];
extern const char kAsyncNamespace[];
extern const char kCCTNamespace[];
extern const char kDownloadNamespace[];
extern const char kNTPSuggestionsNamespace[];
extern const char kSuggestedArticlesNamespace[];
extern const char kBrowserActionsNamespace[];

// Enum of namespaces used by metric collection.
// See OfflinePagesNamespaceEnumeration in enums.xml for histogram usages.
// Changes to this enum should be in sync with the changes to the namespace
// constants above.
enum class OfflinePagesNamespaceEnumeration {
  DEFAULT = 0,
  BOOKMARK = 1,
  LAST_N = 2,
  ASYNC_LOADING = 3,
  CUSTOM_TABS = 4,
  DOWNLOAD = 5,
  NTP_SUGGESTION = 6,
  SUGGESTED_ARTICLES = 7,
  BROWSER_ACTIONS = 8,
  // NOTE: always keep this entry at the end. Add new result types only
  // immediately above this line. Make sure to update the corresponding
  // histogram enum accordingly.
  RESULT_COUNT,
};

}  // namespace offline_pages

#endif  // COMPONENTS_OFFLINE_PAGES_CORE_CLIENT_NAMESPACE_CONSTANTS_H_
