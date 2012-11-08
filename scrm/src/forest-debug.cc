#include "forest.h"

/******************************************************************
 * Debugging Utils
 *****************************************************************/

void Forest::createExampleTree() {
  this->nodes_.clear();
  this->set_local_tree_length(0);
  this->set_total_tree_length(0);
  Node* leaf1 = new Node(0);
  Node* leaf2 = new Node(0);
  Node* leaf3 = new Node(0);
  Node* leaf4 = new Node(0);
  this->addNode(leaf1);
  this->addNode(leaf2);
  this->addNode(leaf3);
  this->addNode(leaf4);

  Node* node12 = new Node(1);
  this->addNodeToTree(node12, NULL, leaf1, leaf2);
  
  Node* node34 = new Node(3);
  this->addNodeToTree(node34, NULL, leaf3, leaf4);
  
  Node* root = new Node(10);
  this->addNodeToTree(root, NULL, node12, node34);
  
  //Build the fake tree above
  Node* ultimate_root = new Node(FLT_MAX, false);
  this->set_ultimate_root(ultimate_root);
  this->addNode(ultimate_root);

  ultimate_root->set_lower_child(root);
  root->set_parent(ultimate_root);

  assert( this->checkTree() );
}

void Forest::addNodeToTree(Node *node, Node *parent, Node *lower_child, Node *higher_child) {
  this->addNode(node);

  if (parent != NULL) {
    node->set_parent(parent);
    if (parent->lower_child() == NULL) parent->set_lower_child(node);
    else {
      if (parent->lower_child()->height() > node->height()) {
        parent->set_higher_child(parent->lower_child());
        parent->set_lower_child(node);
      } else {
        parent->set_higher_child(node);
      }
    }
    this->inc_tree_length(node->height_above(), node->active());
  }

  if (lower_child != NULL) {
    node->set_lower_child(lower_child);
    lower_child->set_parent(node);
    this->inc_tree_length(lower_child->height_above(), lower_child->active());
  }

  if (higher_child != NULL) {
    node->set_higher_child(higher_child);
    higher_child->set_parent(node);
    this->inc_tree_length(higher_child->height_above(), higher_child->active());
  }
}

bool Forest::checkNodesSorted() {
  double cur_height = 0;
  for (std::vector<Node*>::iterator it = this->getNodeFwIterator(); 
       it!=this->getNodesEnd(); ++it) {

    if ((*it)->height() < cur_height) {
      dout << "Error: Nodes not sorted" << std::endl;
      return(0);
    } else {
      cur_height = (*it)->height();
    }

  }
  return(1);
}

bool Forest::checkTreeLength() {
  double local_length = 0;
  double total_length = 0;
  this->calcTreeLength(&local_length, &total_length);

  if ( !areSame(total_length, total_tree_length()) ) {
    dout << "Error: total tree length is " << this->total_tree_length() << " ";
    dout << "but should be " << total_length << std::endl;
    return(0);
  }
  if ( !areSame(local_length, local_tree_length()) ) {
    dout << "Error: local tree length is " << this->local_tree_length() << " ";
    dout << "but should be " << local_length << std::endl;
    return(0);
  }
  return(1);
}

bool Forest::checkTree(Node *root) {
  if (root == NULL) {
    //Default when called without argument
    root = this->ultimate_root();

    //Also check if nodes are sorted by height in this case
    assert(this->checkNodesSorted());
    assert(this->checkTreeLength());
  }
  assert( root != NULL);

  Node* h_child = root->higher_child();
  Node* l_child = root->lower_child();

  //Do we have two childs?
  if (h_child != NULL && l_child != NULL) {
    if (h_child->height() < l_child->height()) { 
      dout << root << ": Child Nodes in wrong order" << std::endl;
      dout << root << ": higher child " << h_child << " at " << h_child->height() << std::endl;
      dout << root << ": lower child " << l_child << " at " << l_child->height() << std::endl;
      return 0;
    }
  }
  //Do we have only one child?
  else if ( !(h_child == NULL && l_child == NULL) ) {
    //This is only allowed for root and fake nodes
    if ( !(root->is_root() || root->is_fake()) ) { 
      dout << root << ": Has only one child" << std::endl;
      return 0;
    }
  }
  
  bool child1 = 1;
  if (h_child != NULL) {
    if (h_child->parent() != root) {
      dout << h_child << ": is child of non-parent" << std::endl;
      return 0;
    }
    child1 = checkTree(h_child);
  }

  bool child2 = 1;
  if (l_child != NULL) {
    if (l_child->parent() != root) {
      dout << l_child << ": is child of non-parent" << std::endl;
      return 0;
    }
    child2 = checkTree(l_child);
  }

  return child1*child2;
}




/******************************************************************
 * Tree Printing
 *****************************************************************/
bool Forest::printTree() {
  std::vector<Node*>::reverse_iterator it;
  std::vector<Node*>::iterator cit;
  Node* current_node;
  int lines_left, lines_right, position;
  double current_height = FLT_MAX;
  
  std::vector<Node*> branches;
  for (int i=0; i < this->countNodes(); ++i) branches.push_back(NULL);

  for (it = nodes_.rbegin(); it < nodes_.rend(); ++it) {
    current_node = *it;

    lines_left = countLinesLeft(current_node);
    lines_right = countLinesRight(current_node);

    if ( current_node == this->ultimate_root() ) {
      position = countBelowLinesLeft(current_node) + lines_left;
      branches[position] = current_node;
    } else {
       position = 0;
       for (cit = branches.begin(); cit < branches.end(); ++cit) {
         if ( *cit == current_node ) break;
         ++position;
       }
    }

    //Print one line of "|" between each event
    if (current_node->height() != FLT_MAX && current_node->height() != current_height) {
      dout << std::endl;
      for (cit = branches.begin(); cit < branches.end(); ++cit) {
        if (*cit == NULL) dout << " ";
        else dout << "|";
      }
      dout << std::endl;
    }
   
    branches[position - lines_left] = current_node->lower_child();
    branches[position + lines_right] = current_node->higher_child();
    branches[position] = NULL;
    
    if (current_node->height() == FLT_MAX) continue;

    if (areSame(current_height, current_node->height())) {
      dout << " " << current_node;
      continue;
    }

    current_height = current_node->height();

    for (int i=0; i < branches.size(); ++i) {
      if ( i < position - lines_left) {
        if ( branches[i] == NULL ) dout << " ";
        else if (areSame(branches[i]->height(), current_node->height())) 
          dout << "+";
        else dout << "|";
      } 
      else if ( i < position) dout << "-";
      else if ( i == position) { 
        if (current_node->active()) dout << "+";
        else dout << "°";
      }
      else if ( i <= position + lines_right) dout << "-";
      else {
        if ( branches[i] == NULL ) dout << " ";
        else if (areSame(branches[i]->height(), current_node->height())) 
          dout << "+";
        else dout << "|";
      }
    }

    dout << " " << current_node->height() << " " << current_node ;
    ////dout << " " << countLinesLeft(current_node);

  }
  //Returns true, so that it can be place in asserts
  dout << std::endl;
  return true;
}

int Forest::countLinesLeft(Node *node) {
  if ( node->lower_child() == NULL ) return 0;
  return ( 1 + countBelowLinesRight(node) );
}

int Forest::countLinesRight(Node *node) {
  if ( node->lower_child() == NULL ) return 0;
  return ( 1 + countBelowLinesLeft(node) );
}

int Forest::countBelowLinesLeft(Node *node) {
  if ( node->lower_child() == NULL ) return 0;
  else return ( countLinesLeft(node->lower_child()) + countBelowLinesLeft(node->lower_child()) );
}

int Forest::countBelowLinesRight(Node *node) {
  if ( node->higher_child() == NULL ) return 0;
  else return ( countLinesRight(node->higher_child()) + countBelowLinesRight(node->higher_child()) );
}

void Forest::printNodes() {
  for(int i = 0; i < this->countNodes(); ++i) {
    dout << "Addr: " << this->nodes()[i] << " | ";
    dout << "Height: " << this->nodes()[i]->height() << " | ";
    if (this->nodes()[i]->is_ultimate_root())
      dout << "Parent: " << "NONE" << " | ";
    else
      dout << "Parent: " << this->nodes()[i]->parent() << " | ";
    dout << "h_child: " << this->nodes()[i]->higher_child() << " | ";
    dout << "l_child: " << this->nodes()[i]->lower_child() << std::endl;
  }
}

bool areSame(double a, double b) {
      return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
      //return std::fabs(a - b) < 0.0001;
}