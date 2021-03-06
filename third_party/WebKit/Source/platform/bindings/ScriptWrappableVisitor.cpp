// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform/bindings/ScriptWrappableVisitor.h"

#include "platform/Supplementable.h"
#include "platform/bindings/DOMWrapperMap.h"
#include "platform/bindings/TraceWrapperBase.h"

namespace blink {

void ScriptWrappableVisitor::TraceWrappers(
    DOMWrapperMap<ScriptWrappable>* wrapper_map,
    const ScriptWrappable* key) const {
  Visit(wrapper_map, key);
}

void ScriptWrappableVisitor::DispatchTraceWrappers(
    const TraceWrapperBase* wrapper_base) const {
  wrapper_base->TraceWrappers(this);
}

void ScriptWrappableVisitor::DispatchTraceWrappersForSupplement(
    const TraceWrapperBaseForSupplement* wrapper_base) const {
  wrapper_base->TraceWrappers(this);
}
}  // namespace blink
