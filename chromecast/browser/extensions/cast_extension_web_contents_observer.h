// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMECAST_BROWSER_EXTENSIONS_CAST_EXTENSION_WEB_CONTENTS_OBSERVER_H_
#define CHROMECAST_BROWSER_EXTENSIONS_CAST_EXTENSION_WEB_CONTENTS_OBSERVER_H_

#include "base/macros.h"
#include "content/public/browser/web_contents_user_data.h"
#include "extensions/browser/extension_web_contents_observer.h"

namespace extensions {

// The cast_shell version of ExtensionWebContentsObserver.
class CastExtensionWebContentsObserver
    : public ExtensionWebContentsObserver,
      public content::WebContentsUserData<CastExtensionWebContentsObserver> {
 public:
  ~CastExtensionWebContentsObserver() override;

 private:
  friend class content::WebContentsUserData<CastExtensionWebContentsObserver>;

  explicit CastExtensionWebContentsObserver(content::WebContents* web_contents);

  DISALLOW_COPY_AND_ASSIGN(CastExtensionWebContentsObserver);
};

}  // namespace extensions

#endif  // CHROMECAST_BROWSER_EXTENSIONS_CAST_EXTENSION_WEB_CONTENTS_OBSERVER_H_
