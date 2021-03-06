// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/testing/StaticSelection.h"

#include "core/editing/SelectionTemplate.h"

namespace blink {

// static
StaticSelection* StaticSelection::FromSelectionInFlatTree(
    const SelectionInFlatTree& seleciton) {
  return new StaticSelection(seleciton);
}

StaticSelection::StaticSelection(const SelectionInFlatTree& seleciton)
    : anchor_node_(seleciton.Base().ComputeContainerNode()),
      anchor_offset_(seleciton.Base().ComputeOffsetInContainerNode()),
      focus_node_(seleciton.Extent().ComputeContainerNode()),
      focus_offset_(seleciton.Extent().ComputeOffsetInContainerNode()) {}

void StaticSelection::Trace(blink::Visitor* visitor) {
  visitor->Trace(anchor_node_);
  visitor->Trace(focus_node_);
  ScriptWrappable::Trace(visitor);
}

}  //  namespace blink
