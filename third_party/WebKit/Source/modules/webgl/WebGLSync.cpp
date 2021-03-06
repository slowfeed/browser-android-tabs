// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/webgl/WebGLSync.h"

#include "gpu/command_buffer/client/gles2_interface.h"
#include "modules/webgl/WebGL2RenderingContextBase.h"
#include "public/platform/Platform.h"
#include "public/platform/TaskType.h"

namespace blink {

WebGLSync::WebGLSync(WebGL2RenderingContextBase* ctx,
                     GLsync object,
                     GLenum object_type)
    : WebGLSharedObject(ctx),
      sync_status_(GL_UNSIGNALED),
      object_(object),
      object_type_(object_type) {
  if (ctx->canvas()) {
    task_runner_ =
        ctx->canvas()->GetDocument().GetTaskRunner(TaskType::kUnthrottled);
  } else {
    // Fallback for OffscreenCanvas (no frame scheduler)
    task_runner_ = Platform::Current()->CurrentThread()->GetTaskRunner();
  }
  ScheduleAllowCacheUpdate();
}

WebGLSync::~WebGLSync() {
  RunDestructor();
}

void WebGLSync::UpdateCache(gpu::gles2::GLES2Interface* gl) {
  if (sync_status_ == GL_SIGNALED) {
    return;
  }

  if (!allow_cache_update_) {
    return;
  }

  // We can only update the cached result when control returns to the browser.
  allow_cache_update_ = false;
  GLsizei length = -1;
  GLint value = 0;
  gl->GetSynciv(object_, GL_SYNC_STATUS, 1, &length, &value);
  if (value) {
    sync_status_ = value;
  }
  if (sync_status_ != GL_SIGNALED) {
    ScheduleAllowCacheUpdate();
  }
}

GLint WebGLSync::GetCachedResult(GLenum pname) {
  switch (pname) {
    case GL_OBJECT_TYPE:
      return object_type_;
    case GL_SYNC_STATUS:
      return sync_status_;
    case GL_SYNC_CONDITION:
      return GL_SYNC_GPU_COMMANDS_COMPLETE;
    case GL_SYNC_FLAGS:
      return 0;
  }

  NOTREACHED();
  return 0;
}

bool WebGLSync::IsSignaled() const {
  return (sync_status_ == GL_SIGNALED);
}

void WebGLSync::ScheduleAllowCacheUpdate() {
  if (task_handle_.IsActive())
    return;
  task_handle_ = PostCancellableTask(
      *task_runner_, FROM_HERE,
      WTF::Bind(&WebGLSync::AllowCacheUpdate, WrapWeakPersistent(this)));
}

void WebGLSync::AllowCacheUpdate() {
  allow_cache_update_ = true;
}

void WebGLSync::DeleteObjectImpl(gpu::gles2::GLES2Interface* gl) {
  gl->DeleteSync(object_);
  object_ = nullptr;
}

}  // namespace blink
