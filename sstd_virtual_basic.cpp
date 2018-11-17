#include "sstd_virtual_basic.hpp"
#include <cassert>

const sstd_virtual_basic::sstd_type_index & sstd_virtual_basic::sstd_get_type_index() const {
    using sstd_this_type_ = std::remove_cv_t<
        std::remove_reference_t< decltype(*this) > >;
    return ::sstd_get_type_index<sstd_this_type_>();
}

sstd_virtual_basic::sstd_virtual_basic() noexcept {
    mmm_objects = new items_map_t_t;
    sstd_add_object_cast(this->sstd_get_type_index(), this);
}

sstd_virtual_basic::~sstd_virtual_basic() {
    destory_objects_in_this();
    delete mmm_named_objects;
    delete mmm_objects_in_this;
    delete mmm_objects;
    delete mmm_mutex;
}

void sstd_virtual_basic::sstd_add_object_cast(const sstd_type_index & k, void * v) {
    mmm_objects->get_type_data()->emplace(k, v);
}

void * sstd_virtual_basic::sstd_find_object(const sstd_type_index & k) const {
    const auto & varMap = std::as_const(*(mmm_objects->get_type_data()));
    auto varPos = varMap.find(k);
    if (varPos == varMap.end()) {
        return nullptr;
    }
    return varPos->second;
}

sstd_virtual_basic::maped_named_objects_t * sstd_virtual_basic::get_named_objects() noexcept {
    if (mmm_named_objects == nullptr) {
        mmm_named_objects = new maped_named_objects_t_t;
    }
    return mmm_named_objects->get_type_data();
}

sstd_virtual_basic::mmm_objects_in_this_t * sstd_virtual_basic::get_objects_list() noexcept {
    if (mmm_objects_in_this == nullptr) {
        mmm_objects_in_this = new mmm_objects_in_this_t_t;
    }
    return mmm_objects_in_this->get_type_data();
}

void sstd_virtual_basic::destory_objects_in_this() noexcept {
    if (mmm_objects_in_this) {
        auto p = mmm_objects_in_this->get_type_data();
        while (p->empty() == false) {
            p->pop_front();
        }
    }
}

void sstd_virtual_basic::insert_object(std::unique_ptr< private_object_t  > && arg) noexcept {
    get_objects_list()->push_front(std::move(arg));
}

void sstd_virtual_basic::insert_named_object(const std::string_view &name, std::unique_ptr< private_object_t  > && arg) noexcept {
    get_named_objects()->emplace(name, arg.get());
    get_objects_list()->push_front(std::move(arg));
}

void * sstd_virtual_basic::sstd_find_named_object(const std::string_view &name) const noexcept {
    auto varMap = const_cast<sstd_virtual_basic *>(this)->get_named_objects();
    auto varPos = varMap->find(name);
    if (varPos == varMap->end()) {
        return nullptr;
    }
    return varPos->second->get_data();
}

void _01_00_private_sstd_virtual_basic::named_object::set_name(const std::string_view & name) {
    *(mmm_name->get_type_data()) = name;
}

std::string_view _01_00_private_sstd_virtual_basic::named_object::get_name() const {
    return *(mmm_name->get_type_data());
}

_01_00_private_sstd_virtual_basic::named_object::named_object() noexcept {
    mmm_name = new string_type;
}

_01_00_private_sstd_virtual_basic::named_object::~named_object() noexcept {
    delete mmm_name;
}

_01_00_private_sstd_virtual_basic::object::~object() noexcept {
}

_01_00_private_sstd_virtual_basic::object::object() noexcept {
}

std::shared_ptr< std::recursive_mutex > sstd_virtual_basic::sstd_get_class_mutex() const noexcept {
    auto varAns = mmm_mutex.load();
    if (varAns) {
        return *(varAns->get_type_data());
    }

    {
        auto varData = new mutex_t_t;
        (*(varAns->get_type_data())) = std::make_shared< std::recursive_mutex >();
        mutex_t_t * varTmp{ nullptr };
        const bool varSetValue = mmm_mutex.compare_exchange_strong(varTmp, varAns);
        if (varSetValue) {
            varAns = varData;
            assert(varAns == mmm_mutex.load());
        } else {
            delete varData;
            varAns = varTmp;
            assert(varAns == nullptr);
        }
    }

    return *(varAns->get_type_data());
}








