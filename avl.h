#include <cassert>
#include <cstddef>
#include <iostream>

template <class ValueType>
class Set {
 private:
  class Node {
   public:
    Node() {
    }

    Node(ValueType value, Set* outer_set)
        : value(std::move(value)),
          outer(outer_set),
          height(1),
          left(nullptr),
          right(nullptr) {
    }

    Node* FindNext(const ValueType& value) {
      Node* current_node = outer->root_;
      Node* result = nullptr;
      while (current_node != nullptr) {
        if (value < current_node->value) {
          result = current_node;
          current_node = current_node->left;
        } else {
          current_node = current_node->right;
        }
      }
      return result;
    }

    Node* FindPrevious(const ValueType& value) {
      Node* current_node = outer->root_;
      Node* result = nullptr;
      while (current_node != nullptr) {
        if (value <= current_node->value) {
          current_node = current_node->left;
        } else {
          result = current_node;
          current_node = current_node->right;
        }
      }
      return result;
    }

   public:
    ValueType value;
    Node *left, *right;
    int height;
    Set* outer;
  };

 public:
  Set() {
    size_ = 0;
    root_ = nullptr;
  }

  template <typename Iterator>
  Set(Iterator first, Iterator last) {
    while (first != last) {
      insert(*first);
      first++;
    }
  }

  Set(std::initializer_list<ValueType> elems) {
    for (const auto& elem : elems) {
      insert(elem);
    }
  }

  Set(const Set& s) {
    for (const auto& elem : s) {
      insert(elem);
    }
  }

  Set operator=(const Set& s) {
    if (this == &s) {
      return *this;
    }
    Clear();
    for (const auto& elem : s) {
      insert(elem);
    }
    return *this;
  }

  ~Set() {
    Clear();
  }

  size_t size() const {
    return size_;
  }

  bool empty() const {
    return size_ == 0;
  }

  void insert(const ValueType& elem) {
    root_ = Insert(root_, elem, this);
  }

  void erase(const ValueType& elem) {
    root_ = Erase(root_, elem);
  }

  class iterator {
   public:
    iterator operator++() {
      v = v->FindNext(v->value);
      return *this;
    }

    iterator operator++(int) {
      iterator was = *this;
      v = v->FindNext(v->value);
      return was;
    }

    iterator operator--() {
      v = v->FindPrevious(v->value);
      return *this;
    }

    iterator operator--(int) {
      iterator was = *this;
      v = v->FindPrevious(v->value);
      return was;
    }

    bool operator==(iterator it) const {
      return v == it.v;
    }

    bool operator!=(iterator it) const {
      return v != it.v;
    }

    iterator() {
      v = nullptr;
    }

    iterator(const iterator& it) {
      v = it.v;
    }

    iterator(Node* node) {
      v = node;
    }

    const ValueType* operator->() const {
      return &(v->value);
    }

    const ValueType operator*() const {
      return v->value;
    }

   private:
    Node* v;
  };

  iterator begin() const {
    return iterator(GetMin(root_));
  }

  iterator end() const {
    return iterator(nullptr);
  }

  iterator find(const ValueType& elem) const {
    Node* v = Find(root_, elem);
    if (v && !IsEqual(elem, v->value)) {
      v = nullptr;
    }
    return iterator(v);
  }

  iterator lower_bound(const ValueType& elem) const {
    Node* v = FindLowerBound(elem);
    return iterator(v);
  }

  void print() {
    for (const auto& elem : *this) {
      std::cout << elem << " ";
    }
    std::cout << "\n";
  }

 private:
  static bool IsEqual(const ValueType& x, const ValueType& y) {
    return !(x < y || y < x);
  }
  int GetHeight(Node* v) const {
    if (!v) {
      return 0;
    }
    return v->height;
  }

  void UpdateHeight(Node* v) {
    if (!v) {
      return;
    }
    int L = GetHeight(v->left);
    int R = GetHeight(v->right);
    v->height = std::max(L, R) + 1;
  }

  Node* RotateLeft(Node* v) {
    assert(v && v->right);
    Node* u = v->right;
    v->right = u->left;
    u->left = v;
    UpdateHeight(v);
    UpdateHeight(u);
    return u;
  }

  Node* BigRotateLeft(Node* v) {
    v->right = RotateRight(v->right);
    return RotateLeft(v);
  }

  Node* RotateRight(Node* v) {
    assert(v && v->left);
    Node* u = v->left;
    v->left = u->right;
    u->right = v;
    UpdateHeight(v);
    UpdateHeight(u);
    return u;
  }

  Node* BigRotateRight(Node* v) {
    v->left = RotateLeft(v->left);
    return RotateRight(v);
  }

  int GetBalance(Node* v) const {
    assert(v);
    return GetHeight(v->left) - GetHeight(v->right);
  }

  Node* FixBalance(Node* v) {
    if (!v) {
      return v;
    }
    UpdateHeight(v);
    int bal = GetBalance(v);
    if (bal == -2) {
      if (GetBalance(v->right) == 1) {
        return BigRotateLeft(v);
      } else {
        return RotateLeft(v);
      }
    } else if (bal == 2) {
      if (GetBalance(v->left) == 1) {
        return RotateRight(v);
      } else {
        return BigRotateRight(v);
      }
    }
    return v;
  }

  Node* Insert(Node* v, const ValueType& value, Set* outer_set) {
    if (v == nullptr) {
      v = new Node(value, outer_set);
      size_++;
      return v;
    }
    const ValueType& x = v->value;
    if (x < value) {
      v->right = Insert(v->right, value, outer_set);
    } else if (value < x) {
      v->left = Insert(v->left, value, outer_set);
    } else {
      return v;
    }
    v = FixBalance(v);
    assert(abs(GetBalance(v)) <= 1);
    return v;
  }

  Node* Find(Node* v, const ValueType& value) const {
    if (v == nullptr) {
      return v;
    }
    const ValueType& x = v->value;
    if (x < value) {
      return Find(v->right, value);
    } else if (value < x) {
      return Find(v->left, value);
    } else {
      return v;
    }
  }

  Node* FindLowerBound(const ValueType& value) const {
    Node* current_node = root_;
    Node* result = nullptr;
    while (current_node != nullptr) {
      if (value <= current_node->value) {
        result = current_node;
        current_node = current_node->left;
      } else {
        current_node = current_node->right;
      }
    }
    return result;
  }

  Node* GetMin(Node* v) const {
    if (!v) {
      return nullptr;
    }
    if (v->left) {
      return GetMin(v->left);
    } else {
      return v;
    }
  }

  Node* Erase(Node* v, const ValueType& value) {
    if (!v) {
      return nullptr;
    }
    const ValueType& x = v->value;
    if (IsEqual(value, x)) {
      if (v->left && v->right) {
        Node* u = GetMin(v->right);
        v->value = std::move(u->value);
        v->right = Erase(v->right, v->value);
      } else if (v->left) {
        Node* tmp = v;
        v = v->left;
        size_ -= 1;
        delete tmp;
      } else if (v->right) {
        Node* tmp = v;
        v = v->right;
        size_ -= 1;
        delete tmp;
      } else {
        delete v;
        size_ -= 1;
        return nullptr;
      }
      v = FixBalance(v);
      assert(abs(GetBalance(v)) <= 1);
      return v;
    }

    if (x < value) {
      v->right = Erase(v->right, value);
    } else {
      v->left = Erase(v->left, value);
    }
    v = FixBalance(v);
    assert(abs(GetBalance(v)) <= 1);
    return v;
  }

  void Clear() {
    Clear(root_);
    size_ = 0;
    root_ = nullptr;
  }

  void Clear(Node* v) {
    if (!v) {
      return;
    }
    Clear(v->left);
    Clear(v->right);
    delete v;
  }

  size_t size_ = 0;
  Node* root_ = nullptr;
};
