#include "Serialization.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "Array.hpp"
#include "DoublyLinkedList.hpp"
#include "FullBinaryTree.hpp"
#include "HashTable.hpp"
#include "Queue.hpp"
#include "SinglyLinkedList.hpp"
#include "Stack.hpp"

namespace lab3 {
namespace serial {

namespace {

constexpr std::uint32_t kMaxItems = 1000000U;
constexpr std::uint32_t kMaxStringLength = 16U * 1024U * 1024U;

void writeU32(std::ostream& out, std::uint32_t value) {
    const std::uint8_t bytes[4] = {
        static_cast<std::uint8_t>(value & 0xFFU),
        static_cast<std::uint8_t>((value >> 8U) & 0xFFU),
        static_cast<std::uint8_t>((value >> 16U) & 0xFFU),
        static_cast<std::uint8_t>((value >> 24U) & 0xFFU)};
    out.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));
}

bool readU32(std::istream& in, std::uint32_t& value) {
    std::uint8_t bytes[4] = {0, 0, 0, 0};
    if (!in.read(reinterpret_cast<char*>(bytes), sizeof(bytes))) {
        return false;
    }
    value = static_cast<std::uint32_t>(bytes[0]) |
            (static_cast<std::uint32_t>(bytes[1]) << 8U) |
            (static_cast<std::uint32_t>(bytes[2]) << 16U) |
            (static_cast<std::uint32_t>(bytes[3]) << 24U);
    return true;
}

bool writeTag(std::ostream& out, const char (&tag)[5]) {
    out.write(tag, 4);
    return static_cast<bool>(out);
}

bool readTag(std::istream& in, const char (&tag)[5]) {
    char bytes[4] = {};
    return static_cast<bool>(in.read(bytes, sizeof(bytes))) &&
           std::memcmp(bytes, tag, sizeof(bytes)) == 0;
}

bool writeString(std::ostream& out, const std::string& value) {
    if (value.size() > kMaxStringLength) {
        return false;
    }
    writeU32(out, static_cast<std::uint32_t>(value.size()));
    out.write(value.data(), static_cast<std::streamsize>(value.size()));
    return static_cast<bool>(out);
}

bool readString(std::istream& in, std::string& value) {
    std::uint32_t length = 0;
    if (!readU32(in, length) || length > kMaxStringLength) {
        return false;
    }
    value.assign(length, '\0');
    if (length == 0) {
        return true;
    }
    return static_cast<bool>(in.read(&value[0], static_cast<std::streamsize>(length)));
}

bool writeSequence(std::ostream& out, const std::vector<std::string>& values) {
    if (values.size() > kMaxItems) {
        return false;
    }
    writeU32(out, static_cast<std::uint32_t>(values.size()));
    for (const auto& value : values) {
        if (!writeString(out, value)) {
            return false;
        }
    }
    return static_cast<bool>(out);
}

bool readSequence(std::istream& in, std::vector<std::string>& values) {
    std::uint32_t count = 0;
    if (!readU32(in, count) || count > kMaxItems) {
        return false;
    }
    values.clear();
    values.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::string value;
        if (!readString(in, value)) {
            return false;
        }
        values.push_back(std::move(value));
    }
    return true;
}

template <typename Value, typename Save>
bool saveToFile(const std::string& path, const Value& value, Save save) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        return false;
    }
    if (!save(out, value)) {
        out.close();
        std::remove(path.c_str());
        return false;
    }
    out.flush();
    if (!out) {
        out.close();
        std::remove(path.c_str());
        return false;
    }
    return true;
}

template <typename Value, typename Load>
bool loadFromFile(const std::string& path, Value& value, Load load) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    return load(in, value);
}

}  // namespace

bool saveArray(std::ostream& out, const Array& value) {
    return writeTag(out, "ARR1") && writeSequence(out, value.toVector());
}

bool loadArray(std::istream& in, Array& value) {
    std::vector<std::string> values;
    if (!readTag(in, "ARR1") || !readSequence(in, values)) {
        return false;
    }
    Array loaded;
    for (const auto& item : values) {
        loaded.pushBack(item);
    }
    value = loaded;
    return true;
}

bool saveList(std::ostream& out, const SinglyLinkedList& value) {
    return writeTag(out, "LST1") && writeSequence(out, value.toVector());
}

bool loadList(std::istream& in, SinglyLinkedList& value) {
    std::vector<std::string> values;
    if (!readTag(in, "LST1") || !readSequence(in, values)) {
        return false;
    }
    SinglyLinkedList loaded;
    for (const auto& item : values) {
        loaded.pushBack(item);
    }
    value = loaded;
    return true;
}

bool saveDoubleList(std::ostream& out, const DoublyLinkedList& value) {
    return writeTag(out, "DLST") && writeSequence(out, value.toVector());
}

bool loadDoubleList(std::istream& in, DoublyLinkedList& value) {
    std::vector<std::string> values;
    if (!readTag(in, "DLST") || !readSequence(in, values)) {
        return false;
    }
    DoublyLinkedList loaded;
    for (const auto& item : values) {
        loaded.pushBack(item);
    }
    value = loaded;
    return true;
}

bool saveStack(std::ostream& out, const Stack& value) {
    return writeTag(out, "STK1") && writeSequence(out, value.toVector());
}

bool loadStack(std::istream& in, Stack& value) {
    std::vector<std::string> values;
    if (!readTag(in, "STK1") || !readSequence(in, values)) {
        return false;
    }
    Stack loaded;
    for (const auto& item : values) {
        loaded.push(item);
    }
    value = loaded;
    return true;
}

bool saveQueue(std::ostream& out, const Queue& value) {
    return writeTag(out, "QUE1") && writeSequence(out, value.toVector());
}

bool loadQueue(std::istream& in, Queue& value) {
    std::vector<std::string> values;
    if (!readTag(in, "QUE1") || !readSequence(in, values)) {
        return false;
    }
    Queue loaded;
    for (const auto& item : values) {
        loaded.push(item);
    }
    value = loaded;
    return true;
}

bool saveHash(std::ostream& out, const HashTable& value) {
    const auto entries = value.entries();
    if (entries.size() > kMaxItems || !writeTag(out, "HSH1")) {
        return false;
    }
    writeU32(out, static_cast<std::uint32_t>(entries.size()));
    for (const auto& entry : entries) {
        if (!writeString(out, entry.first) || !writeString(out, entry.second)) {
            return false;
        }
    }
    return static_cast<bool>(out);
}

bool loadHash(std::istream& in, HashTable& value) {
    std::uint32_t count = 0;
    if (!readTag(in, "HSH1") || !readU32(in, count) || count > kMaxItems) {
        return false;
    }
    std::vector<std::pair<std::string, std::string>> entries;
    entries.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::string key;
        std::string item;
        if (!readString(in, key) || !readString(in, item)) {
            return false;
        }
        entries.emplace_back(std::move(key), std::move(item));
    }
    value.clear();
    for (const auto& entry : entries) {
        value.put(entry.first, entry.second);
    }
    return true;
}

bool saveTree(std::ostream& out, const FullBinaryTree& value) {
    const auto tokens = value.preorderWithNulls();
    if (tokens.size() > kMaxItems || !writeTag(out, "TRE1")) {
        return false;
    }
    writeU32(out, static_cast<std::uint32_t>(tokens.size()));
    for (const auto& token : tokens) {
        const std::uint8_t present = token.has_value() ? 1U : 0U;
        out.write(reinterpret_cast<const char*>(&present), sizeof(present));
        if (token.has_value()) {
            writeU32(out, static_cast<std::uint32_t>(*token));
        }
    }
    return static_cast<bool>(out);
}

bool loadTree(std::istream& in, FullBinaryTree& value) {
    std::uint32_t count = 0;
    if (!readTag(in, "TRE1") || !readU32(in, count) || count > kMaxItems) {
        return false;
    }
    std::vector<std::optional<int>> tokens;
    tokens.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint8_t present = 0;
        if (!in.read(reinterpret_cast<char*>(&present), sizeof(present)) || present > 1U) {
            return false;
        }
        if (present == 0U) {
            tokens.emplace_back(std::nullopt);
            continue;
        }
        std::uint32_t raw = 0;
        if (!readU32(in, raw)) {
            return false;
        }
        tokens.emplace_back(static_cast<int>(raw));
    }
    FullBinaryTree loaded;
    if (!loaded.loadPreorderWithNulls(tokens)) {
        return false;
    }
    value = std::move(loaded);
    return true;
}

bool saveArrayToFile(const std::string& path, const Array& value) {
    return saveToFile(path, value, saveArray);
}

bool loadArrayFromFile(const std::string& path, Array& value) {
    return loadFromFile(path, value, loadArray);
}

bool saveListToFile(const std::string& path, const SinglyLinkedList& value) {
    return saveToFile(path, value, saveList);
}

bool loadListFromFile(const std::string& path, SinglyLinkedList& value) {
    return loadFromFile(path, value, loadList);
}

bool saveDoubleListToFile(const std::string& path, const DoublyLinkedList& value) {
    return saveToFile(path, value, saveDoubleList);
}

bool loadDoubleListFromFile(const std::string& path, DoublyLinkedList& value) {
    return loadFromFile(path, value, loadDoubleList);
}

bool saveStackToFile(const std::string& path, const Stack& value) {
    return saveToFile(path, value, saveStack);
}

bool loadStackFromFile(const std::string& path, Stack& value) {
    return loadFromFile(path, value, loadStack);
}

bool saveQueueToFile(const std::string& path, const Queue& value) {
    return saveToFile(path, value, saveQueue);
}

bool loadQueueFromFile(const std::string& path, Queue& value) {
    return loadFromFile(path, value, loadQueue);
}

bool saveHashToFile(const std::string& path, const HashTable& value) {
    return saveToFile(path, value, saveHash);
}

bool loadHashFromFile(const std::string& path, HashTable& value) {
    return loadFromFile(path, value, loadHash);
}

bool saveTreeToFile(const std::string& path, const FullBinaryTree& value) {
    return saveToFile(path, value, saveTree);
}

bool loadTreeFromFile(const std::string& path, FullBinaryTree& value) {
    return loadFromFile(path, value, loadTree);
}

}  // namespace serial
}  // namespace lab3
