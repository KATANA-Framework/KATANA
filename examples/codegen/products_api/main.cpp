#include "generated/generated_dtos.hpp"
#include "generated/generated_handlers.hpp"
#include "generated/generated_json.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_routes.hpp"
#include "generated/generated_validators.hpp"

#include "katana/core/http_server.hpp"
#include "katana/core/reactor_pool.hpp"

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace katana;
using namespace katana::http;

// Simple thread-safe in-memory product store
class product_store {
public:
    product_store() : next_id_(1) {}

    struct stored_product {
        int64_t id;
        std::string sku;
        std::string name;
        std::string description;
        double price;
        int64_t stock;
        ProductCategory category;
        std::vector<std::string> tags;
    };

    std::optional<stored_product> create(const CreateProductRequest& req) {
        std::lock_guard lock(mutex_);

        // Check for duplicate SKU
        std::string sku_str(req.sku.data(), req.sku.size());
        for (const auto& [id, product] : products_) {
            if (product.sku == sku_str) {
                return std::nullopt; // Duplicate
            }
        }

        stored_product product;
        product.id = next_id_++;
        product.sku = sku_str;
        product.name = std::string(req.name.data(), req.name.size());
        if (req.description.has_value()) {
            product.description = std::string(req.description->data(), req.description->size());
        }
        product.price = req.price;
        product.stock = req.stock.value_or(0);
        product.category = req.category;

        if (req.tags.has_value()) {
            for (const auto& tag : *req.tags) {
                product.tags.emplace_back(tag.data(), tag.size());
            }
        }

        products_[product.id] = product;
        return product;
    }

    std::optional<stored_product> get(int64_t id) {
        std::lock_guard lock(mutex_);
        auto it = products_.find(id);
        if (it == products_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<stored_product> list(int limit, int offset) {
        std::lock_guard lock(mutex_);
        std::vector<stored_product> result;
        result.reserve(std::min(static_cast<size_t>(limit), products_.size()));

        int count = 0;
        for (const auto& [id, product] : products_) {
            if (count >= offset && result.size() < static_cast<size_t>(limit)) {
                result.push_back(product);
            }
            count++;
        }
        return result;
    }

    size_t count() {
        std::lock_guard lock(mutex_);
        return products_.size();
    }

    std::vector<stored_product> search(std::string_view query) {
        std::lock_guard lock(mutex_);
        std::vector<stored_product> result;

        std::string query_lower;
        query_lower.reserve(query.size());
        for (char c : query) {
            query_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        for (const auto& [id, product] : products_) {
            std::string name_lower;
            name_lower.reserve(product.name.size());
            for (char c : product.name) {
                name_lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }

            if (name_lower.find(query_lower) != std::string::npos) {
                result.push_back(product);
            }
        }

        return result;
    }

    bool update(int64_t id, const UpdateProductRequest& req) {
        std::lock_guard lock(mutex_);
        auto it = products_.find(id);
        if (it == products_.end()) {
            return false;
        }

        auto& product = it->second;
        if (req.name.has_value()) {
            product.name = std::string(req.name->data(), req.name->size());
        }
        if (req.description.has_value() && *req.description) {
            product.description =
                std::string((*req.description)->data(), (*req.description)->size());
        }
        if (req.price.has_value()) {
            product.price = *req.price;
        }
        if (req.stock.has_value()) {
            product.stock = *req.stock;
        }
        if (req.category.has_value()) {
            product.category = *req.category;
        }
        if (req.tags.has_value()) {
            product.tags.clear();
            for (const auto& tag : *req.tags) {
                product.tags.emplace_back(tag.data(), tag.size());
            }
        }

        return true;
    }

    bool remove(int64_t id) {
        std::lock_guard lock(mutex_);
        return products_.erase(id) > 0;
    }

    std::optional<int64_t> adjust_stock(int64_t id, int64_t delta) {
        std::lock_guard lock(mutex_);
        auto it = products_.find(id);
        if (it == products_.end()) {
            return std::nullopt;
        }

        auto& product = it->second;
        int64_t new_stock = product.stock + delta;
        if (new_stock < 0 || new_stock > 1000000) {
            return std::nullopt; // Invalid adjustment
        }

        product.stock = new_stock;
        return new_stock;
    }

private:
    std::mutex mutex_;
    std::unordered_map<int64_t, stored_product> products_;
    std::atomic<int64_t> next_id_;
};

// Handler implementation
class products_handler : public generated::api_handler {
public:
    explicit products_handler(product_store& store) : store_(store) {}

    response list_products(std::optional<int64_t> limit, std::optional<int64_t> offset) override {
        int lim = limit.value_or(10);
        int off = offset.value_or(0);

        auto products = store_.list(lim, off);
        size_t total = store_.count();

        monotonic_arena* arena = handler_context::arena();
        ProductList list(arena);
        list.total = static_cast<int64_t>(total);
        list.limit = lim;
        list.offset = off;

        // Convert stored products to DTOs
        for (const auto& sp : products) {
            Product p(arena);
            p.id = sp.id;
            p.sku = arena_string<>(sp.sku, arena_allocator<char>(arena));
            p.name = arena_string<>(sp.name, arena_allocator<char>(arena));
            if (!sp.description.empty()) {
                p.description = arena_string<>(sp.description, arena_allocator<char>(arena));
            }
            p.price = sp.price;
            p.stock = sp.stock;
            p.category = sp.category;

            if (!sp.tags.empty()) {
                p.tags.emplace(arena_allocator<arena_string<>>(arena));
                for (const auto& tag : sp.tags) {
                    p.tags->emplace_back(tag, arena_allocator<char>(arena));
                }
            }

            list.items.push_back(std::move(p));
        }

        return response::json(serialize_ProductList(list));
    }

    response create_product(const CreateProductRequest& body) override {
        auto product_opt = store_.create(body);
        if (!product_opt) {
            return response::error(problem_details::conflict(
                "product.duplicate_sku", "Product with this SKU already exists"));
        }

        const auto& sp = *product_opt;
        monotonic_arena* arena = handler_context::arena();
        Product p(arena);
        p.id = sp.id;
        p.sku = arena_string<>(sp.sku, arena_allocator<char>(arena));
        p.name = arena_string<>(sp.name, arena_allocator<char>(arena));
        if (!sp.description.empty()) {
            p.description = arena_string<>(sp.description, arena_allocator<char>(arena));
        }
        p.price = sp.price;
        p.stock = sp.stock;
        p.category = sp.category;

        if (!sp.tags.empty()) {
            p.tags.emplace(arena_allocator<arena_string<>>(arena));
            for (const auto& tag : sp.tags) {
                p.tags->emplace_back(tag, arena_allocator<char>(arena));
            }
        }

        return response::json(serialize_Product(p), 201);
    }

    response get_product(int64_t id) override {
        auto product_opt = store_.get(id);
        if (!product_opt) {
            return response::error(
                problem_details::not_found("product.not_found", "Product not found"));
        }

        const auto& sp = *product_opt;
        monotonic_arena* arena = handler_context::arena();
        Product p(arena);
        p.id = sp.id;
        p.sku = arena_string<>(sp.sku, arena_allocator<char>(arena));
        p.name = arena_string<>(sp.name, arena_allocator<char>(arena));
        if (!sp.description.empty()) {
            p.description = arena_string<>(sp.description, arena_allocator<char>(arena));
        }
        p.price = sp.price;
        p.stock = sp.stock;
        p.category = sp.category;

        if (!sp.tags.empty()) {
            p.tags.emplace(arena_allocator<arena_string<>>(arena));
            for (const auto& tag : sp.tags) {
                p.tags->emplace_back(tag, arena_allocator<char>(arena));
            }
        }

        return response::json(serialize_Product(p));
    }

    response update_product(int64_t id, const UpdateProductRequest& body) override {
        if (!store_.update(id, body)) {
            return response::error(
                problem_details::not_found("product.not_found", "Product not found"));
        }

        // Fetch updated product
        auto product_opt = store_.get(id);
        if (!product_opt) {
            return response::error(problem_details::internal_error("product.internal_error"));
        }

        const auto& sp = *product_opt;
        monotonic_arena* arena = handler_context::arena();
        Product p(arena);
        p.id = sp.id;
        p.sku = arena_string<>(sp.sku, arena_allocator<char>(arena));
        p.name = arena_string<>(sp.name, arena_allocator<char>(arena));
        if (!sp.description.empty()) {
            p.description = arena_string<>(sp.description, arena_allocator<char>(arena));
        }
        p.price = sp.price;
        p.stock = sp.stock;
        p.category = sp.category;

        if (!sp.tags.empty()) {
            p.tags.emplace(arena_allocator<arena_string<>>(arena));
            for (const auto& tag : sp.tags) {
                p.tags->emplace_back(tag, arena_allocator<char>(arena));
            }
        }

        return response::json(serialize_Product(p));
    }

    response delete_product(int64_t id) override {
        if (!store_.remove(id)) {
            return response::error(
                problem_details::not_found("product.not_found", "Product not found"));
        }
        return response::no_content();
    }

    response search_products(std::string_view query) override {
        auto products = store_.search(query);

        monotonic_arena* arena = handler_context::arena();
        arena_vector<Product> results(arena_allocator<Product>(arena));

        for (const auto& sp : products) {
            Product p(arena);
            p.id = sp.id;
            p.sku = arena_string<>(sp.sku, arena_allocator<char>(arena));
            p.name = arena_string<>(sp.name, arena_allocator<char>(arena));
            if (!sp.description.empty()) {
                p.description = arena_string<>(sp.description, arena_allocator<char>(arena));
            }
            p.price = sp.price;
            p.stock = sp.stock;
            p.category = sp.category;

            if (!sp.tags.empty()) {
                p.tags.emplace(arena_allocator<arena_string<>>(arena));
                for (const auto& tag : sp.tags) {
                    p.tags->emplace_back(tag, arena_allocator<char>(arena));
                }
            }

            results.push_back(std::move(p));
        }

        // Serialize array of products
        std::string json = "[";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) {
                json += ",";
            }
            json += serialize_Product(results[i]);
        }
        json += "]";

        return response::json(json);
    }

    response adjust_stock(int64_t id, const StockAdjustment& body) override {
        auto new_stock_opt = store_.adjust_stock(id, body.delta);
        if (!new_stock_opt) {
            auto product_opt = store_.get(id);
            if (!product_opt) {
                return response::error(
                    problem_details::not_found("product.not_found", "Product not found"));
            }
            return response::error(problem_details::bad_request(
                "stock.invalid_adjustment", "Stock adjustment would result in invalid value"));
        }

        std::string json = R"({"new_stock":)" + std::to_string(*new_stock_opt) + "}";
        return response::json(json);
    }

private:
    product_store& store_;
};

int main() {
    try {
        const char* port_str = std::getenv("PORT");
        uint16_t port = port_str ? static_cast<uint16_t>(std::atoi(port_str)) : 8082;

        product_store store;
        products_handler handler(store);

        auto router_fn = generated::make_router(handler);

        http_server_config config;
        config.port = port;
        config.num_threads = 0; // Auto-detect CPU cores

        reactor_pool pool(config.num_threads);
        http_server server(pool, config, router_fn);

        std::cout << "Products API server starting on port " << port << "\n";
        std::cout << "Endpoints:\n";
        std::cout << "  GET    /products?limit=10&offset=0\n";
        std::cout << "  POST   /products\n";
        std::cout << "  GET    /products/{id}\n";
        std::cout << "  PUT    /products/{id}\n";
        std::cout << "  DELETE /products/{id}\n";
        std::cout << "  GET    /products/search?query=...\n";
        std::cout << "  POST   /products/{id}/stock\n";
        std::cout << "Press Ctrl+C to stop.\n\n";

        server.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
