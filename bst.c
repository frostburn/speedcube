typedef struct Node {
  Cube *cube;
  struct Node *left;
  struct Node *right;
} Node;

Cube* get(Node *node, Cube *cube) {
  if (node == NULL) {
    return NULL;
  }

  int cmp = compare(cube, node->cube);
  if (cmp == 0) {
    return node->cube;
  } else if (cmp < 0) {
    return get(node->left, cube);
  } else {
    return get(node->right, cube);
  }
}

void insert(Node *root, Node *leaf) {
  int cmp = compare(leaf->cube, root->cube);
  if (cmp < 0) {
    if (root->left == NULL) {
      root->left = leaf;
    } else {
      insert(root->left, leaf);
    }
    return;
  } else if (cmp > 0) {
    if (root->right == NULL) {
      root->right = leaf;
    } else {
      insert(root->right, leaf);
    }
    return;
  }

  fprintf(stderr, "Bad insert\n");
  exit(EXIT_FAILURE);
}

size_t height(Node *root) {
  if (root == NULL) {
    return 0;
  }
  size_t left_height = height(root->left);
  size_t right_height = height(root->right);
  if (left_height > right_height) {
    return left_height + 1;
  }
  return right_height + 1;
}

int cmp_nodes(const void *a, const void *b) {
  Node *x = (Node*) a;
  Node *y = (Node*) b;
  return compare(x->cube, y->cube);
}

Node* rebuild(Node *nodes, size_t size) {
  if (!size) {
    return NULL;
  }
  size_t root_index = size / 2;
  size_t left_index = root_index / 2;
  size_t right_size = size - root_index - 1;
  size_t right_index = right_size / 2 + root_index + 1;

  Node *root = nodes + root_index;
  if (left_index == root_index) {
    root->left = NULL;
  } else {
    root->left = nodes + left_index;
    rebuild(nodes, root_index);
  }
  if (right_index == root_index || right_index >= size) {
    root->right = NULL;
  } else {
    root->right = nodes + right_index;
    rebuild(nodes + root_index + 1, right_size);
  }
  return root;
}

Node* balance(Node *nodes, size_t num_nodes) {
  qsort(nodes, num_nodes, sizeof(Node), cmp_nodes);
  return rebuild(nodes, num_nodes);
}
