// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.native_test;

import android.app.Application;
import android.content.Context;

import org.chromium.base.BuildConfig;
import org.chromium.base.multidex.ChromiumMultiDexInstaller;

/**
 * Application class to be used by native_test apks.
 */
public class NativeTestApplication extends Application {
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        assert getBaseContext() != null;
        if (BuildConfig.isMultidexEnabled()) {
            ChromiumMultiDexInstaller.install(this);
        }
    }
}
