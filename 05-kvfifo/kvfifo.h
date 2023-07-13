#ifndef __KVFIFO_H__
#define __KVFIFO_H__

#include <concepts>    // for copy_constructible, regular, totally_ordered
#include <cstddef>     // for size_t
#include <map>         // for map
#include <memory>      // for make_shared, unique_ptr, shared_ptr, make_unique
#include <optional>    // for optional
#include <ranges>      // for transform
#include <stdexcept>   // for invalid_argument
#include <type_traits> // for declval
#include <utility>     // for pair

template<std::regular K, typename V>
  requires std::totally_ordered<K> && std::copy_constructible<V>
class kvfifo
{
public:
  constexpr kvfifo() = default;

  kvfifo(const kvfifo& other)
    : state_{ other.copy_if_tainted() }
  {
  }

  constexpr kvfifo(kvfifo&& other) noexcept
    : state_{ std::move(other.state_) }
  {
    other.state_ = state::empty_state();
  }

  ~kvfifo() noexcept = default;

  kvfifo& operator=(kvfifo other)
  {
    state_ = std::move(other.state_);
    return *this;
  }

  std::pair<const K&, V&> back()
  {
    if (state_->empty())
      throw std::invalid_argument{ "empty kvfifo" };
    else if (!state_.unique())
      state_ = std::make_shared<state>(*state_);

    state_->taint();
    return { state_->last->key_iter->first, state_->last->value };
  }

  std::pair<const K&, const V&> back() const
  {
    if (state_->empty())
      throw std::invalid_argument{ "empty kvfifo" };

    return { state_->last->key_iter->first, state_->last->value };
  }

  void clear() noexcept { state_ = state::empty_state(); }

  std::size_t count(const K& k) const
  {
    auto it = state_->find_key(k);
    if (!it)
      return 0;

    return (*it)->second.count;
  }

  bool empty() const noexcept { return state_->empty(); }

  std::pair<const K&, V&> first(const K& k)
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    if (!state_.unique()) {
      state_ = std::make_shared<state>(*state_);
      it = state_->find_key(k);
    }

    state_->taint();
    return { (*it)->first, (*it)->second.first->value };
  }

  std::pair<const K&, const V&> first(const K& k) const
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    return { (*it)->first, (*it)->second.first->value };
  }

  std::pair<const K&, V&> front()
  {
    if (state_->empty())
      throw std::invalid_argument{ "empty kvfifo" };
    else if (!state_.unique())
      state_ = std::make_shared<state>(*state_);

    state_->taint();
    return { state_->first->key_iter->first, state_->first->value };
  }

  std::pair<const K&, const V&> front() const
  {
    if (state_->empty())
      throw std::invalid_argument{ "empty kvfifo" };

    return { state_->first->key_iter->first, state_->first->value };
  }

  auto k_begin() const noexcept { return state_->keys().begin(); }

  auto k_end() const noexcept { return state_->keys().end(); }

  std::pair<const K&, V&> last(const K& k)
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    if (!state_.unique()) {
      state_ = std::make_shared<state>(*state_);
      it = state_->find_key(k);
    }

    state_->taint();
    return { (*it)->first, (*it)->second.last->value };
  }

  std::pair<const K&, const V&> last(const K& k) const
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    return { (*it)->first, (*it)->second.last->value };
  }

  void move_to_back(const K& k)
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    // From now on, we are sure that unless something bad happens, we WILL
    // modify the kvfifo

    if (!state_.unique()) {
      state_ = std::make_shared<state>(*state_);
      it = state_->find_key(k);
    } else
      state_->untaint();

    state_->move_to_back(*it);
  }

  void pop()
  {
    if (state_->empty())
      throw std::invalid_argument{ "empty kvfifo" };
    // From now on, we are sure that unless something bad happens, we WILL
    // modify the kvfifo
    else if (!state_.unique())
      state_ = std::make_shared<state>(*state_);
    else
      state_->untaint();

    state_->pop();
  }

  void pop(const K& k)
  {
    auto it = state_->find_key(k);
    if (!it)
      throw std::invalid_argument{ "key not in kvfifo" };

    // From now on, we are sure that unless something bad happens, we WILL
    // modify the kvfifo

    if (!state_.unique()) {
      state_ = std::make_shared<state>(*state_);
      it = state_->find_key(k);
    } else
      state_->untaint();

    state_->pop(*it);
  }

  void push(const K& k, const V& v)
  {
    if (!state_.unique())
      state_ = std::make_shared<state>(*state_);
    else
      state_->untaint();

    state_->push(k, v);
  }

  std::size_t size() const noexcept { return state_->size(); }

private:
  struct state
  {
    constexpr state() noexcept = default;

    state(const state& s)
    {
      for (node* it = s.first.get(); it; it = it->next.get())
        push(it->key_iter->first, it->value);
    }

    constexpr ~state() noexcept
    {
      while (first)
        first = std::move(first->next);
    }

    struct node;

    struct key_info
    {
      node* first;
      node* last;
      std::size_t count;
    };

    using key_map = std::map<K, key_info>;

    struct node
    {
      constexpr node(const V& v)
        : value{ v }
      {
      }

      typename key_map::iterator key_iter;
      V value;
      node* prev{};
      std::unique_ptr<node> next{};
      node* k_next{};
      bool tainted{ false };
    };

    constexpr std::size_t count(const K& k) const noexcept
    {
      auto it = k_map.find(k);
      if (it == k_map.end())
        return 0;
      return it->second.count;
    }

    constexpr bool empty() const noexcept { return !first; }

    static std::shared_ptr<state> empty_state()
    {
      static const auto instance = std::make_shared<state>();

      return instance;
    }

    constexpr std::optional<typename key_map::iterator> find_key(const K& k)
    {
      auto it = k_map.find(k);
      if (it == k_map.end())
        return {};
      return { it };
    }

    constexpr std::unique_ptr<node>& get_slot(node* n) noexcept
    {
      return n->prev ? n->prev->next : first;
    }

    constexpr const auto& keys() const noexcept { return keys_; }

    static constexpr auto keys_create(const key_map& k_map) noexcept
    {
      return k_map |
             std::views::transform([](const auto& x) { return x.first; });
    }

    void move_to_back(typename key_map::iterator it) noexcept
    {
      for (node* n = it->second.first; n; n = n->k_next) {
        if (n->next)
          n->next->prev = n->prev;
        auto& n_slot = get_slot(n);
        auto n_own = std::move(n_slot);
        n_slot = std::move(n->next);
        push_helper(std::move(n_own));
      }
    }

    void pop() noexcept
    {
      auto n = std::move(first); // never null
      first = std::move(n->next);
      if (first)
        first->prev = nullptr;
      else
        last = nullptr;

      pop_helper(std::move(n));
    }

    void pop(typename key_map::iterator it) noexcept
    {
      if (it->second.first == first.get())
        pop();
      else {
        auto n = std::move(it->second.first->prev->next);
        if (n->next)
          n->next->prev = n->prev;
        n->prev->next = std::move(n->next);

        if (last == n.get())
          last = n->prev;

        pop_helper(std::move(n));
      }
    }

    void pop_helper(std::unique_ptr<node> n) noexcept
    {
      auto it = n->key_iter;
      if (n.get() == it->second.last) {
        k_map.erase(it);
      } else {
        it->second.first = n->k_next;
        --it->second.count;
      }

      --size_;
    }

    void push(const K& k, const V& v)
    {
      auto n = std::make_unique<node>(v);
      auto [it, created] = k_map.try_emplace(k);

      // From this point on no exceptions may be thrown, so the operation always
      // succeeds The only issue is when the node is allocated but map
      // try_emplace fails, in this case the node will be allocated and then
      // immediately deallocated. However, it is impossible to avoid such
      // allocation order caveats.

      if (created)
        it->second = { n.get(), n.get(), 1 };
      else {
        it->second.last = it->second.last->k_next = n.get();
        ++it->second.count;
      }

      ++size_;
      n->key_iter = it;
      push_helper(std::move(n));
    }

    void push_helper(std::unique_ptr<node> n) noexcept
    {
      n->prev = last;
      if (last)
        last = (last->next = std::move(n)).get();
      else {
        first = std::move(n);
        last = first.get();
      }
    }

    constexpr std::size_t size() const noexcept { return size_; }

    constexpr void taint() noexcept { tainted_ = true; }

    constexpr bool tainted() const noexcept { return tainted_; }

    constexpr void untaint() noexcept { tainted_ = false; }

    std::unique_ptr<node> first{};
    node* last{};
    key_map k_map{};
    decltype(keys_create(k_map)) keys_{ keys_create(k_map) };
    std::size_t size_{};
    bool tainted_{ false };
  };

  std::shared_ptr<state> copy_if_tainted() const
  {
    return state_->tainted() ? std::make_shared<state>(*state_) : state_;
  }

  std::shared_ptr<state> state_{ state::empty_state() };

public:
  using k_iterator = decltype(std::declval<state&>().keys().begin());
};

#endif /* __KVFIFO_H__ */
