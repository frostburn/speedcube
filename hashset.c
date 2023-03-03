// Hash set with open indexing
typedef struct {
  size_t *elements;
  size_t num_elements;
  size_t empty_value;
  size_t mask;
  int magnitude;
} HashSet;

HashSet init_hashset(int magnitude, size_t empty_value) {
  HashSet result;

  result.magnitude = magnitude;
  size_t size = 1ULL << magnitude;
  result.elements = malloc(size * sizeof(size_t));
  for (size_t i = 0; i < size; ++i) {
    result.elements[i] = empty_value;
  }
  result.empty_value = empty_value;
  result.num_elements = 0;
  result.mask = size - 1;
  return result;
}

void free_hashset(HashSet *set) {
  free(set->elements);
}

void hashset_grow(HashSet *set) {
  size_t old_size = 1ULL << set->magnitude;
  size_t new_bit = old_size;
  set->magnitude++;
  size_t size = 1ULL << set->magnitude;
  set->elements = realloc(set->elements, size * sizeof(size_t));
  set->mask = size - 1;

  // Clear allocated memory
  for (size_t i = old_size; i < size; ++i) {
    set->elements[i] = set->empty_value;
  }

  // Move elements with the new bit to the freshly allocated region
  for (size_t i = 0; i < old_size; ++i) {
    size_t element = set->elements[i];
    if (element == set->empty_value) {
      continue;
    }
    if (element & new_bit) {
      set->elements[i] = set->empty_value;
      size_t index = element & set->mask;
      for (;;) {
        if (set->elements[index] == set->empty_value) {
          set->elements[index] = element;
          break;
        }
        index = (index + 1) & set->mask;
      }
    }
  }

  // Make the old region contiguous for open indexing
  for (size_t i = 0; i < old_size; ++i) {
    size_t element = set->elements[i];
    if (element == set->empty_value) {
      continue;
    }
    set->elements[i] = set->empty_value;
    size_t index = element & set->mask;
    for (;;) {
      if (set->elements[index] == set->empty_value) {
        set->elements[index] = element;
        break;
      }
      index = (index + 1) & set->mask;
    }
  }
}

void hashset_add(HashSet *set, size_t element) {
  size_t index = element & set->mask;
  for (;;) {
    if (set->elements[index] == set->empty_value) {
      set->elements[index] = element;
      set->num_elements++;
      break;
    }
    if (set->elements[index] == element) {
      break;
    }
    index = (index + 1) & set->mask;
  }
  if (10 * set->num_elements > 9 * set->mask) {
    hashset_grow(set);
  }
}

bool hashset_has(HashSet *set, size_t element) {
  size_t index = element & set->mask;
  for (;;) {
    if (set->elements[index] == set->empty_value) {
      return false;
    }
    if (set->elements[index] == element) {
      return true;
    }
    index = (index + 1) & set->mask;
  }
}
