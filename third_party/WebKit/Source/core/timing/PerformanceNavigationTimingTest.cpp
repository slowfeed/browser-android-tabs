// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/timing/PerformanceNavigationTiming.h"

#include "core/testing/PageTestBase.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/WebKit/public/mojom/page/page_visibility_state.mojom-blink.h"

namespace blink {

class PerformanceNavigationTimingTest : public PageTestBase {
 protected:
  AtomicString GetNavigationType(NavigationType type, Document* document) {
    return PerformanceNavigationTiming::GetNavigationType(type, document);
  }
};

TEST_F(PerformanceNavigationTimingTest, GetNavigationType) {
  GetPage().SetVisibilityState(mojom::PageVisibilityState::kPrerender, false);
  AtomicString returned_type =
      GetNavigationType(kNavigationTypeBackForward, &GetDocument());
  EXPECT_EQ(returned_type, "prerender");

  GetPage().SetVisibilityState(mojom::PageVisibilityState::kHidden, false);
  returned_type = GetNavigationType(kNavigationTypeBackForward, &GetDocument());
  EXPECT_EQ(returned_type, "back_forward");

  GetPage().SetVisibilityState(mojom::PageVisibilityState::kVisible, false);
  returned_type =
      GetNavigationType(kNavigationTypeFormResubmitted, &GetDocument());
  EXPECT_EQ(returned_type, "navigate");
}
}  // namespace blink
