// Filename: animBundleNode.cxx
// Created by:  drose (06Mar02)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "animBundleNode.h"
#include "datagram.h"
#include "datagramIterator.h"
#include "bamReader.h"
#include "bamWriter.h"

TypeHandle AnimBundleNode::_type_handle;


////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::make_copy
//       Access: Public, Virtual
//  Description: 
////////////////////////////////////////////////////////////////////
PandaNode *AnimBundleNode::
make_copy() const {
  return new AnimBundleNode(*this);
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::safe_to_flatten
//       Access: Public, Virtual
//  Description: Returns true if it is generally safe to flatten out
//               this particular kind of Node by duplicating
//               instances, false otherwise (for instance, a Camera
//               cannot be safely flattened, because the Camera
//               pointer itself is meaningful).
////////////////////////////////////////////////////////////////////
bool AnimBundleNode::
safe_to_flatten() const {
  return false;
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::find_anim_bundle
//       Access: Published, Static
//  Description: Recursively walks the scene graph beginning at the
//               indicated node (which need not be an AnimBundleNode),
//               and returns the first AnimBundle found.  Returns NULL
//               if no AnimBundle can be found.
////////////////////////////////////////////////////////////////////
AnimBundle *AnimBundleNode::
find_anim_bundle(PandaNode *root) {
  nassertr(root != (PandaNode *)NULL, NULL);

  if (root->is_of_type(AnimBundleNode::get_class_type())) {
    AnimBundleNode *anode = DCAST(AnimBundleNode, root);
    AnimBundle *anim = anode->get_bundle();
    if (anim != (AnimBundle *)NULL) {
      return anim;
    }
  }

  Children cr = root->get_children();
  int num_children = cr.get_num_children();
  for (int i = 0; i < num_children; i++) {
    AnimBundle *anim = find_anim_bundle(cr.get_child(i));
    if (anim != (AnimBundle *)NULL) {
      return anim;
    }
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::register_with_read_factory
//       Access: Public, Static
//  Description: Tells the BamReader how to create objects of type
//               AnimBundleNode.
////////////////////////////////////////////////////////////////////
void AnimBundleNode::
register_with_read_factory() {
  BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::write_datagram
//       Access: Public, Virtual
//  Description: Writes the contents of this object to the datagram
//               for shipping out to a Bam file.
////////////////////////////////////////////////////////////////////
void AnimBundleNode::
write_datagram(BamWriter *manager, Datagram &dg) {
  PandaNode::write_datagram(manager, dg);
  manager->write_pointer(dg, _bundle);
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::complete_pointers
//       Access: Public, Virtual
//  Description: Receives an array of pointers, one for each time
//               manager->read_pointer() was called in fillin().
//               Returns the number of pointers processed.
////////////////////////////////////////////////////////////////////
int AnimBundleNode::
complete_pointers(TypedWritable **p_list, BamReader* manager) {
  int pi = PandaNode::complete_pointers(p_list, manager);
  _bundle = DCAST(AnimBundle, p_list[pi++]);
  return pi;
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::make_from_bam
//       Access: Protected, Static
//  Description: This function is called by the BamReader's factory
//               when a new object of this type is encountered
//               in the Bam file.  It should create the object
//               and extract its information from the file.
////////////////////////////////////////////////////////////////////
TypedWritable *AnimBundleNode::
make_from_bam(const FactoryParams &params) {
  AnimBundleNode *node = new AnimBundleNode;
  DatagramIterator scan;
  BamReader *manager;

  parse_params(params, scan, manager);
  node->fillin(scan, manager);

  return node;
}

////////////////////////////////////////////////////////////////////
//     Function: AnimBundleNode::fillin
//       Access: Protected
//  Description: This internal function is called by make_from_bam to
//               read in all of the relevant data from the BamFile for
//               the new PandaNode.
////////////////////////////////////////////////////////////////////
void AnimBundleNode::
fillin(DatagramIterator &scan, BamReader* manager) {
  PandaNode::fillin(scan, manager);
  manager->read_pointer(scan);
}
