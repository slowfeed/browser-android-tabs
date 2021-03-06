// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/keyboard_lock/NavigatorKeyboardLock.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/frame/LocalFrame.h"
#include "platform/bindings/V8Binding.h"
#include "platform/heap/Persistent.h"
#include "platform/wtf/Assertions.h"
#include "platform/wtf/Functional.h"
#include "services/service_manager/public/cpp/interface_provider.h"

namespace blink {

NavigatorKeyboardLock::NavigatorKeyboardLock(Navigator& navigator)
    : Supplement<Navigator>(navigator) {}

NavigatorKeyboardLock& NavigatorKeyboardLock::From(Navigator& navigator) {
  NavigatorKeyboardLock* supplement =
      Supplement<Navigator>::From<NavigatorKeyboardLock>(navigator);
  if (!supplement) {
    supplement = new NavigatorKeyboardLock(navigator);
    ProvideTo(navigator, supplement);
  }
  return *supplement;
}

// static
ScriptPromise NavigatorKeyboardLock::keyboardLock(
    ScriptState* state,
    Navigator& navigator,
    const Vector<String>& keycodes) {
  return NavigatorKeyboardLock::From(navigator).keyboardLock(state, keycodes);
}

ScriptPromise NavigatorKeyboardLock::keyboardLock(
    ScriptState* state,
    const Vector<String>& keycodes) {
  DCHECK(state);
  if (request_keylock_resolver_) {
    // TODO(joedow): Reject with a DOMException once it has been defined in the
    // spec. See https://github.com/w3c/keyboard-lock/issues/18.
    return ScriptPromise::Reject(
        state, V8String(state->GetIsolate(),
                        "Last keyboardLock() has not finished yet."));
  }

  if (!EnsureServiceConnected()) {
      return ScriptPromise::Reject(
          state, V8String(state->GetIsolate(), "Current frame is detached."));
  }

  request_keylock_resolver_ = ScriptPromiseResolver::Create(state);
  service_->RequestKeyboardLock(
      keycodes, WTF::Bind(&NavigatorKeyboardLock::LockRequestFinished,
                          WrapPersistent(this)));
  return request_keylock_resolver_->Promise();
}

void NavigatorKeyboardLock::keyboardUnlock() {
  if (!EnsureServiceConnected()) {
    // Current frame is detached.
    return;
  }

  service_->CancelKeyboardLock();
}

// static
void NavigatorKeyboardLock::keyboardUnlock(Navigator& navigator) {
  NavigatorKeyboardLock::From(navigator).keyboardUnlock();
}

bool NavigatorKeyboardLock::EnsureServiceConnected() {
  if (!service_) {
    LocalFrame* frame = GetSupplementable()->GetFrame();
    if (!frame) {
      return false;
    }
    frame->GetInterfaceProvider().GetInterface(mojo::MakeRequest(&service_));
  }

  DCHECK(service_);
  return true;
}

void NavigatorKeyboardLock::LockRequestFinished(
    mojom::KeyboardLockRequestResult result) {
  DCHECK(request_keylock_resolver_);
  // TODO(joedow): Reject with a DOMException once it has been defined in the
  // spec.
  if (result == mojom::KeyboardLockRequestResult::SUCCESS) {
    request_keylock_resolver_->Resolve();
  } else {
    request_keylock_resolver_->Reject();
  }
  request_keylock_resolver_ = nullptr;
}

// static
const char NavigatorKeyboardLock::kSupplementName[] = "NavigatorKeyboardLock";

void NavigatorKeyboardLock::Trace(blink::Visitor* visitor) {
  visitor->Trace(request_keylock_resolver_);
  Supplement<Navigator>::Trace(visitor);
}

}  // namespace blink
