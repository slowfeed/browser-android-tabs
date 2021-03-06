/*
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ContextMenuController_h
#define ContextMenuController_h

#include <memory>
#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "core/CoreExport.h"
#include "core/layout/HitTestResult.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebMenuSourceType.h"

namespace blink {

class ContextMenu;
class ContextMenuItem;
class ContextMenuProvider;
class Document;
class LocalFrame;
class MouseEvent;
class Page;
struct WebContextMenuData;

class CORE_EXPORT ContextMenuController final
    : public GarbageCollectedFinalized<ContextMenuController> {
 public:
  static ContextMenuController* Create(Page*);
  ~ContextMenuController();
  void Trace(blink::Visitor*);

  ContextMenu* GetContextMenu() const { return context_menu_.get(); }
  void ClearContextMenu();

  void DocumentDetached(Document*);

  void HandleContextMenuEvent(MouseEvent*);
  void ShowContextMenuAtPoint(LocalFrame*,
                              float x,
                              float y,
                              ContextMenuProvider*);

  void ContextMenuItemSelected(const ContextMenuItem*);

  Node* ContextMenuNodeForFrame(LocalFrame*);

  void SetHitTestResultForTests(const HitTestResult& hit_test_result) {
    hit_test_result_ = hit_test_result;
  }

 private:
  friend class ContextMenuControllerTest;

  explicit ContextMenuController(Page*);

  std::unique_ptr<ContextMenu> CreateContextMenu(MouseEvent*);
  std::unique_ptr<ContextMenu> CreateContextMenu(LocalFrame*,
                                                 const LayoutPoint&);
  void ShowContextMenu(MouseEvent*);

  // Returns whether a Context Menu was actually shown.
  bool ShowContextMenu(const ContextMenu*, WebMenuSourceType);
  bool ShouldShowContextMenuFromTouch(const WebContextMenuData&);

  Member<Page> page_;
  std::unique_ptr<ContextMenu> context_menu_;
  Member<ContextMenuProvider> menu_provider_;
  HitTestResult hit_test_result_;
  DISALLOW_COPY_AND_ASSIGN(ContextMenuController);
};

}  // namespace blink

#endif  // ContextMenuController_h
