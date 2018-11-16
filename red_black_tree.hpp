#pragma once 

#include <list>
#include <tuple>
#include <memory>
#include <utility>
#include <optional>
#include <type_traits>
#include <forward_list>

enum class red_black_tree_color_t : bool {
    red_color,
    black_color,
};

template< typename T >
class red_black_tree_node_t {
public:
    red_black_tree_color_t  color;
    red_black_tree_node_t * parent_node;
    red_black_tree_node_t * left_node;
    red_black_tree_node_t * right_node;
    inline red_black_tree_node_t();
    inline const auto & get_key() const;
    template<typename ... U>
    inline T * construct_key(U && ... args) {
        key = &(key_data.emplace(std::forward<U>(args)...));
        return key;
    }
protected:
    T * key;
    std::optional< std::remove_cv_t<T> > key_data;
    red_black_tree_node_t(const red_black_tree_node_t &) = delete;
    red_black_tree_node_t(red_black_tree_node_t &&) = delete;
    red_black_tree_node_t&operator=(const red_black_tree_node_t &) = delete;
    red_black_tree_node_t&operator=(red_black_tree_node_t&&) = delete;
};

template< typename T >
inline red_black_tree_node_t<T >::red_black_tree_node_t() :
    color(red_black_tree_color_t::black_color),
    parent_node(nullptr),
    left_node(nullptr),
    right_node(nullptr),
    key(nullptr) {
}

template< typename T  >
inline const auto & red_black_tree_node_t<T>::get_key() const {
    return *(this->key);
}

template<
    typename Key,
    template<typename> class Node = red_black_tree_node_t,
    typename Less = std::less<void>,
    template<typename> class Alloc = std::allocator>
class red_black_tree {
public:
    using key_t = std::add_const_t<Key>;
    using node_t = Node<key_t>;
    using less_t = Less;
public:
    /*构造函数*/
    template<typename U>
    inline red_black_tree(U && l) :mmm_less(std::forward<U>(l)) {
    }
    inline red_black_tree() {
    }
    /*析构函数*/
    inline ~red_black_tree() {
        this->pppDestory(get_root_node());
    }
    /*获得根节点*/
    inline constexpr node_t * & get_root_node() noexcept {
        return mmm_root_node;
    }
    inline constexpr node_t * get_root_node() const noexcept {
        return mmm_root_node;
    }
    /*插入Key*/
    template<typename ... U>
    inline node_t * emplace(U && ... args) {
        auto varAns = std::make_unique< node_t >();
        varAns->construct_key(std::forward<U>(args)...);
        auto varReturn = this->pppInsert(varAns.get());
        if (varAns.get() == varReturn/*insert it ...*/) {
            ++mmm_Size;
            pppUpdateMinMaxInsert(varAns.get())/*更新最大最小节点*/;
            return varAns.release();
        }
        return varReturn;
    }
    /*寻找key*/
    template<typename U>
    inline node_t * find(U && arg) const {
        node_t * x = get_root_node();
        while (x != nullptr) {
            if (mmm_less(arg, x->get_key())) {/*node < x ?*/
                x = x->left_node;
            } else if (mmm_less(x->get_key(), arg)) {/*node > x ?*/
                x = x->right_node;
            } else {/*node == x*/
                return x;
            }
        }
        return x;
    }
    /*删除key*/
    template<typename U>
    inline void erase(U && arg) {
        auto varPos = find(std::forward<U>(arg));
        if (varPos) {
            --mmm_Size;
            pppUpdateMinMaxRemove(varPos)/*更新最大最小节点*/;
            return pppRemove(varPos);
        }
        return;
    }
    /*获得最大值*/
    inline node_t * get_max_value_node() const {
        return mmm_max_value_node;
    }
    /*获得最小值*/
    inline node_t * get_min_value_node() const {
        return mmm_min_value_node;
    }
private:
    std::size_t mmm_Size{ 0 };
    node_t * mmm_root_node{ nullptr };
    node_t * mmm_max_value_node{ nullptr };
    node_t * mmm_min_value_node{ nullptr };
    std::remove_cv_t<less_t> mmm_less;
    inline void pppUpdateMinMaxRemove(node_t * arg){
        if ( arg==mmm_max_value_node ) {
            mmm_max_value_node = arg->parent_node;
        }
        if ( arg==mmm_min_value_node ) {
            mmm_min_value_node = arg->parent_node;
        }
    }
    inline void pppUpdateMinMaxInsert(node_t * arg) {
        if (mmm_max_value_node) {
            if (rb_less(mmm_max_value_node,arg)) {
                mmm_max_value_node = arg;
            }
        }else{
            mmm_max_value_node = arg;
        }
        if (mmm_min_value_node) {
            if (rb_less(arg,mmm_min_value_node)) {
                mmm_min_value_node = arg;
            }
        }else{
            mmm_min_value_node = arg;
        }
    }
    inline node_t * pppInsert(node_t * arg) {
        return pppInsert2(get_root_node(), arg);
    }
    inline void pppDestory(node_t * arg) {
        if (arg == nullptr) {
            return;
        }
        std::size_t varDestoryCount{ 0 };
        std::forward_list<node_t *, Alloc<node_t *> > varDeleteList;
        varDeleteList.push_front(arg);
        while (false == varDeleteList.empty()) {
            arg = varDeleteList.front();
            varDeleteList.pop_front();
            ++varDestoryCount;
            if (arg->left_node) {
                varDeleteList.push_front(arg->left_node);
            }
            if (arg->right_node) {
                varDeleteList.push_front(arg->right_node);
            }
            delete arg;
        }
        mmm_Size -= varDestoryCount;
    }
    static inline void pppRightRotate(node_t * & root, node_t * y) {
        auto *x = y->left_node;
        y->left_node = x->right_node;
        if (x->right_node != nullptr) {
            x->right_node->parent_node = y;
        }
        x->parent_node = y->parent_node;
        if (y->parent_node == nullptr) {
            root = x;
        } else {
            if (y == y->parent_node->right_node) {
                y->parent_node->right_node = x;
            } else {
                y->parent_node->left_node = x;
            }
        }
        x->right_node = y;
        y->parent_node = x;
    }
    static inline void pppLeftRotate(node_t * & root, node_t * x) {
        auto *y = x->right_node;
        x->right_node = y->left_node;
        if (y->left_node != nullptr) {
            y->left_node->parent_node = x;
        }
        y->parent_node = x->parent_node;
        if (x->parent_node == nullptr) {
            root = y;
        } else {
            if (x->parent_node->left_node == x) {
                x->parent_node->left_node = y;
            } else {
                x->parent_node->right_node = y;
            }
        }
        y->left_node = x;
        x->parent_node = y;
    }
    inline static auto rb_parent(node_t * r) {
        return r->parent_node;
    };
    inline static auto rb_color(node_t * r) {
        return r->color;
    }
    inline static auto rb_is_red(node_t * r) {
        return r->color == red_black_tree_color_t::red_color;
    }
    inline static auto rb_is_black(node_t * r) {
        return r->color == red_black_tree_color_t::black_color;
    }
    inline static auto rb_set_black(node_t * r) {
        return r->color = red_black_tree_color_t::black_color;
    }
    inline static void rb_set_red(node_t * r) {
        r->color = red_black_tree_color_t::red_color;
    }
    inline static void rb_set_parent(node_t * r, node_t * p) {
        r->parent_node = p;
    }
    inline static void rb_set_color(node_t * r, red_black_tree_color_t c) {
        r->color = c;
    }
    bool rb_less(node_t * l, node_t *r) {
        return mmm_less(l->get_key(), r->get_key());
    }
    static inline void pppInsertFixUp(node_t * & root, node_t * node) {
        node_t * parent_node = nullptr;
        node_t * gparent = nullptr;
        while ((parent_node = rb_parent(node)) && rb_is_red(parent_node)) {
            gparent = rb_parent(parent_node);
            if (parent_node == gparent->left_node) {
                {
                    auto *uncle = gparent->right_node;
                    if (uncle && rb_is_red(uncle)) {
                        rb_set_black(uncle);
                        rb_set_black(parent_node);
                        rb_set_red(gparent);
                        node = gparent;
                        continue;
                    }
                }
                if (parent_node->right_node == node) {
                    node_t *tmp;
                    pppLeftRotate(root, parent_node);
                    tmp = parent_node;
                    parent_node = node;
                    node = tmp;
                }
                rb_set_black(parent_node);
                rb_set_red(gparent);
                pppRightRotate(root, gparent);
            } else {
                {
                    auto *uncle = gparent->left_node;
                    if (uncle && rb_is_red(uncle)) {
                        rb_set_black(uncle);
                        rb_set_black(parent_node);
                        rb_set_red(gparent);
                        node = gparent;
                        continue;
                    }
                }
                if (parent_node->left_node == node) {
                    node_t *tmp;
                    pppRightRotate(root, parent_node);
                    tmp = parent_node;
                    parent_node = node;
                    node = tmp;
                }
                rb_set_black(parent_node);
                rb_set_red(gparent);
                pppLeftRotate(root, gparent);
            }
        }
        rb_set_black(root);
    }
    inline node_t * pppInsert2(node_t * & root, node_t * node) {
        node_t * y = nullptr;
        node_t * x = root;
        while (x != nullptr) {
            y = x;
            if (rb_less(node, x)) {/*node < x ?*/
                x = x->left_node;
            } else if (rb_less(x, node)) {/*node > x ?*/
                x = x->right_node;
            } else {/*node == x*/
                return x;
            }
        }
        node->parent_node = y;
        if (y != nullptr) {
            if (rb_less(node, y)) {
                y->left_node = node;
            } else {
                y->right_node = node;
            }
        } else {
            root = node;
        }
        node->color = red_black_tree_color_t::red_color;
        pppInsertFixUp(root, node);
        return node;
    }
    inline void pppRemove(node_t * node) {
        std::unique_ptr<node_t> varNodeLock{ node };
        node_t * & root = get_root_node();
        node_t *child{ nullptr };
        node_t *parent_node{ nullptr };
        red_black_tree_color_t color{ red_black_tree_color_t::black_color };
        if ((node->left_node != nullptr) && (node->right_node != nullptr)) {
            auto *replace = node;
            replace = replace->right_node;
            while (replace->left_node != nullptr) {
                replace = replace->left_node;
            }
            if (rb_parent(node)) {
                if (rb_parent(node)->left_node == node) {
                    rb_parent(node)->left_node = replace;
                } else {
                    rb_parent(node)->right_node = replace;
                }
            } else {
                root = replace;
            }
            child = replace->right_node;
            parent_node = rb_parent(replace);
            color = rb_color(replace);
            if (parent_node == node) {
                parent_node = replace;
            } else {
                if (child) {
                    rb_set_parent(child, parent_node);
                }
                parent_node->left_node = child;
                replace->right_node = node->right_node;
                rb_set_parent(node->right_node, replace);
            }
            replace->parent_node = node->parent_node;
            replace->color = node->color;
            replace->left_node = node->left_node;
            node->left_node->parent_node = replace;
            if (color == red_black_tree_color_t::black_color) {
                pppRemoveFixUp(root, child, parent_node);
            }
            return;
        }
        if (node->left_node != nullptr) {
            child = node->left_node;
        } else {
            child = node->right_node;
        }
        parent_node = node->parent_node;
        color = node->color;
        if (child) {
            child->parent_node = parent_node;
        }
        if (parent_node) {
            if (parent_node->left_node == node) {
                parent_node->left_node = child;
            } else {
                parent_node->right_node = child;
            }
        } else {
            root = child;
        }
        if (color == red_black_tree_color_t::black_color) {
            pppRemoveFixUp(root, child, parent_node);
        }
    }
    static inline void pppRemoveFixUp(node_t*&root, node_t *node, node_t *parent_node) {
        node_t *other{ nullptr };
        while ((!node || rb_is_black(node)) && (node != root)) {
            if (parent_node->left_node == node) {
                other = parent_node->right_node;
                if (rb_is_red(other)) {
                    rb_set_black(other);
                    rb_set_red(parent_node);
                    pppLeftRotate(root, parent_node);
                    other = parent_node->right_node;
                }
                if ((!other->left_node || rb_is_black(other->left_node)) &&
                    (!other->right_node || rb_is_black(other->right_node))) {
                    rb_set_red(other);
                    node = parent_node;
                    parent_node = rb_parent(node);
                } else {
                    if (!other->right_node || rb_is_black(other->right_node)) {  
                        rb_set_black(other->left_node);
                        rb_set_red(other);
                        pppRightRotate(root, other);
                        other = parent_node->right_node;
                    }
                    rb_set_color(other, rb_color(parent_node));
                    rb_set_black(parent_node);
                    rb_set_black(other->right_node);
                    pppLeftRotate(root, parent_node);
                    node = root;
                    break;
                }
            } else {
                other = parent_node->left_node;
                if (rb_is_red(other)) {
                    rb_set_black(other);
                    rb_set_red(parent_node);
                    pppRightRotate(root, parent_node);
                    other = parent_node->left_node;
                }
                if ((!other->left_node || rb_is_black(other->left_node)) &&
                    (!other->right_node || rb_is_black(other->right_node))) {
                    rb_set_red(other);
                    node = parent_node;
                    parent_node = rb_parent(node);
                } else {
                    if (!other->left_node || rb_is_black(other->left_node)) {  
                        rb_set_black(other->right_node);
                        rb_set_red(other);
                        pppLeftRotate(root, other);
                        other = parent_node->left_node;
                    }
                    rb_set_color(other, rb_color(parent_node));
                    rb_set_black(parent_node);
                    rb_set_black(other->left_node);
                    pppRightRotate(root, parent_node);
                    node = root;
                    break;
                }
            }
        }
        if (node) {
            rb_set_black(node);
        }
    }
};

/*寻找这一节点下的最大值*/
template<typename node_t>
inline static node_t * maxvalue_red_black_tree_node(const node_t * _x_x) {
    auto x = (node_t *)(_x_x);
    if (x == nullptr) {
        return nullptr;
    }
    while (x->right_node != nullptr) {
        x = x->right_node;
    }
    return x;
}


/*寻找前驱节点*/
template<typename node_t>
inline static node_t * predecessor_red_black_tree_node(const node_t * _x_x) {
    auto x = (node_t *)(_x_x);
    if (x->left_node != nullptr) {
        return maxvalue_red_black_tree_node(x->left_node);
    }
    auto * y = x->parent_node;
    while ((y != nullptr) && (x == y->left_node)) {
        x = y;
        y = y->parent_node;
    }
    return y;
}

/*寻找这一节点下的最小值*/
template<typename node_t>
inline static node_t * minvalue_red_black_tree_node(const node_t * _x_x) {
    auto x = (node_t *)(_x_x);
    if (x == nullptr) {
        return nullptr;
    }
    while (x->left_node != nullptr) {
        x = x->left_node;
    }
    return x;
}

/*寻找后续节点*/
template <typename node_t>
node_t * successor_red_black_tree_node(const node_t *_x_x) {
    auto x = (node_t *)(_x_x);
    if (x->right_node != nullptr) {
        return minvalue_red_black_tree_node(x->right_node);
    }
    auto * y = x->parent_node;
    while ((y != nullptr) && (x == y->right_node)) {
        x = y;
        y = y->parent_node;
    }
    return y;
}



